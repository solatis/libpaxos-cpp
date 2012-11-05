#include <functional>

#include <boost/uuid/uuid_io.hpp>

#include "../../../../durable/storage.hpp"
#include "../../../quorum/server_view.hpp"
#include "../../../paxos_context.hpp"
#include "../../../command.hpp"
#include "../../../parser.hpp"
#include "../../../tcp_connection.hpp"
#include "../../../util/debug.hpp"

#include "strategy.hpp"

namespace paxos { namespace detail { namespace strategy { namespace basic_paxos { namespace protocol {


strategy::strategy (
   durable::storage &   storage)
   : storage_ (storage)
{
}

/*! virtual */ void
strategy::initiate (      
   tcp_connection_ptr                   client_connection,
   detail::command const &              command,
   detail::quorum::server_view &        quorum,
   detail::paxos_context &              global_state,
   queue_guard_type                     queue_guard)
{
   /*!
     If we do not have a the majority of servers alive, it is likely we are having a netsplit 
     and we should never make any progress, to prevent the situation where there are multiple
     representations of the truth in multiple datacenters.
    */
   if (quorum.has_majority () == false)
   {
      this->handle_error (detail::error_no_majority,
                          quorum,
                          client_connection);
      return;
   }

   /*!
     Keeps track of the current state / which servers have responded, etc.
    */
   boost::shared_ptr <struct state> state (new struct state ());

   /*!
     Note that this will ensure the queue guard is in place for as long as the request is
     being processed.
    */
   state->queue_guard = queue_guard;

   std::vector <boost::asio::ip::tcp::endpoint> live_servers = quorum.live_servers ();
   if (live_servers.empty () == true)
   {
      handle_error (detail::error_no_leader,
                    quorum,
                    client_connection);
      return;
   }

   /*!
     Tell all nodes within this quorum to prepare this request.
    */
   for (boost::asio::ip::tcp::endpoint const & endpoint : live_servers)
   {
      detail::quorum::server & server = quorum.lookup_server (endpoint);

      PAXOS_ASSERT (server.has_connection () == true);

      PAXOS_DEBUG ("sending paxos request to server " << endpoint);

      send_prepare (client_connection,
                    command,
                    server.endpoint (),
                    server.connection (),
                    quorum,
                    global_state,
                    command.workload (),
                    state);
   }
}


/*! virtual */ void
strategy::send_prepare (
   tcp_connection_ptr                           client_connection,
   detail::command const &                      client_command,
   boost::asio::ip::tcp::endpoint const &       follower_endpoint,
   tcp_connection_ptr                           follower_connection,
   detail::quorum::server_view &                quorum,
   detail::paxos_context &                      global_state,
   std::string const &                          byte_array,
   boost::shared_ptr <struct state>             state)
{

   /*!
     Ensure to claim an entry in our state so that in a later point in time, we know
     which servers have responded and which don't.
    */
   PAXOS_ASSERT (state->connections.find (follower_endpoint) == state->connections.end ());
   state->connections[follower_endpoint] = follower_connection;


   /*!
     And now always send a 'prepare' proposal to the server. This will validate our proposal
     id at the other server's end.
    */
   command command;

   command.set_type (command::type_request_prepare);
   command.set_next_proposal_id (this->proposal_id () + 1);

   this->add_local_host_information (quorum, command);


   PAXOS_DEBUG ("step2 writing command");

   follower_connection->write_command (command);

   PAXOS_DEBUG ("step2 reading command from follower = " << follower_connection.get ());   

   /*!
     We expect either an 'ack' or a 'reject' response to this command.
    */
   follower_connection->read_command (
      std::bind (&strategy::receive_promise,
                 this,
                 std::placeholders::_1,
                 client_connection,
                 client_command,
                 follower_endpoint,
                 follower_connection,
                 std::ref (quorum),
                 std::ref (global_state),
                 byte_array,
                 std::placeholders::_2,
                 state));
}

/*! virtual */ void
strategy::prepare (      
   tcp_connection_ptr                   leader_connection,
   detail::command const &              command,
   detail::quorum::server_view &        quorum,
   detail::paxos_context &              state)
{
   this->process_remote_host_information (command,
                                          quorum);
   detail::command response;

   PAXOS_DEBUG ("self = " << quorum.our_endpoint () << ", "
                "state.proposal_id () = " << this->proposal_id () << ", "
                "command.proposal_id () = " << command.next_proposal_id ());



   PAXOS_DEBUG ("command.host_endpoint () = " << command.host_endpoint () << ", command.host_id () = " << command.host_id ());

   boost::optional <boost::asio::ip::tcp::endpoint> leader = quorum.who_is_our_leader ();

   if (leader.is_initialized () == false)
   {
      /*!
        We do not know who the leader is
       */
      PAXOS_WARN ("we do not know who the leader is!");
      response.set_type (command::type_request_fail);
      response.set_error_code (detail::error_no_leader);
   }
   else if (*leader != command.host_endpoint ())
   {
      /*!
        This request is coming from a host that is not the leader
       */
      PAXOS_WARN ("request coming from host that is not the leader: "  << *leader << " [" << quorum.lookup_server (*leader).id () << "," << quorum.lookup_server (*leader).highest_proposal_id () << "] != " << command.host_endpoint () << " [" << quorum.lookup_server (command.host_endpoint ()).id () << "," << quorum.lookup_server (command.host_endpoint ()).highest_proposal_id () << "]");
      response.set_type (command::type_request_fail);
      response.set_error_code (detail::error_no_leader);
   }
   else if (command.next_proposal_id () > this->proposal_id ())
   {
      PAXOS_DEBUG ("normal proposal, accepting, command = " << command.next_proposal_id () << ", state = " << this->proposal_id ());

      response.set_type (command::type_request_promise);
   }
   else
   {
      PAXOS_WARN ("incorrect proposal id!");
      response.set_type (command::type_request_fail);
      response.set_error_code (detail::error_incorrect_proposal);
   }

   response.set_next_proposal_id (this->proposal_id ());

   this->add_local_host_information (quorum, response);


   PAXOS_DEBUG ("step3 writing command");   

   leader_connection->write_command (response);
}


/*! virtual */ void
strategy::receive_promise (
   boost::optional <enum detail::error_code>    error,
   tcp_connection_ptr                           client_connection,
   detail::command                              client_command,
   boost::asio::ip::tcp::endpoint const &       follower_endpoint,
   tcp_connection_ptr                           follower_connection,
   detail::quorum::server_view &                quorum,
   detail::paxos_context &                      global_state,
   std::string                                  byte_array,
   detail::command const &                      command,
   boost::shared_ptr <struct state>             state)
{
   if (error)
   {
      PAXOS_WARN ("An error occured while receiving promise from " << follower_endpoint << ": " << detail::to_string (*error));

      quorum.connection_died (follower_endpoint);
      state->accepted[follower_endpoint]    = response_reject;
      state->error_codes[follower_endpoint] = *error;
   }
   else
   {
      this->process_remote_host_information (command,
                                             quorum);

      PAXOS_ASSERT_EQ (state->connections[follower_endpoint], follower_connection);

      PAXOS_DEBUG ("step4 self = " << quorum.our_endpoint () << ", received command from follower " << follower_endpoint);

      switch (command.type ())
      {
            case command::type_request_promise:
               state->accepted[follower_endpoint] = response_ack;
               break;

            case command::type_request_fail:
               /*!
                 This is a corner case, that means a follower has rejected our proposal. This
                 likely means that the other follower has a more recent proposal id, and thus
                 thinks we are not the leader.

                 Since we have just processed the other host's information, it is likely that
                 we will not consider ourselves a leader anymore after this request.
                */
               state->accepted[follower_endpoint]    = response_reject;
               state->error_codes[follower_endpoint] = command.error_code ();
               break;

            default:
               /*!
                 Protocol error!
               */
               PAXOS_UNREACHABLE ();
      };
   }

   if (state->connections.size () == state->accepted.size ())
   {
   
      boost::optional <enum error_code> last_error;

      for (auto const & i : state->accepted)
      {
         if (i.second == response_reject)
         {
            PAXOS_ASSERT (state->error_codes.find (i.first) != state->error_codes.end ());
            last_error = state->error_codes.find (i.first)->second;

            PAXOS_ASSERT_NE (*last_error, detail::no_error);
         }
      }

      if (last_error.is_initialized () == false)
      {
         /*!
           We shouldn't have any errors if everyone promised. This check is handy since
           it verifies we do not have any error codes floating around.
         */
         PAXOS_ASSERT_EQ (state->error_codes.empty (), true);
         
         /*!
           This means that all nodes in the quorum have responded, and they all agree with
           the proposal id, yay!

           Now that all these nodes have promised to accept any request with the specified
           proposal id, let's send them an accept command.
         */
         
         for (auto & i : state->connections)
         {
            send_accept (client_connection,
                         client_command,
                         i.first,
                         i.second,
                         quorum,
                         std::ref (global_state),
                         byte_array,
                         state);
         }
      }
      else
      {
         /*!
           This means that every host has given a response, but some errors have occured
           that prevented everyone from sending a promise.
           
           We will send an error command to the client informing about the failed command.
         */
         handle_error (*last_error,
                       quorum,
                       client_connection);
      }
   }
}

/*! virtual */ void
strategy::send_accept (
   tcp_connection_ptr                           client_connection,
   detail::command const &                      client_command,
   boost::asio::ip::tcp::endpoint const &       follower_endpoint,
   tcp_connection_ptr                           follower_connection,
   detail::quorum::server_view &                quorum,
   detail::paxos_context &                      global_state,
   std::string const &                          byte_array,
   boost::shared_ptr <struct state>             state)
{  
   PAXOS_ASSERT_EQ (state->connections[follower_endpoint], follower_connection);
   PAXOS_ASSERT_EQ (state->accepted[follower_endpoint], response_ack);


   command command;
   command.set_type (command::type_request_accept);

   /*!
     It is possible that the follower lags behind. If this is the case, let's
     send it the history too.
    */
   int64_t follower_highest_proposal_id = 
      quorum.lookup_server (follower_endpoint).highest_proposal_id ();

   /*!
     Note that the storage mechanism is *not* required to retrieve all data, it
     can just retrieve a portion. This prevents the whole quorum from locking up
     if we need to transfer lots of data to a single follower.
    */
   command.set_proposed_workload (
      storage_.retrieve (follower_highest_proposal_id));

   if (command.proposed_workload ().empty () == true
       || command.proposed_workload ().rbegin ()->first == this->proposal_id ())
   {
      /*!
        This means that either there was no historical data available for the 
        follower (the most likely case, because that means the follower is up-to-date),
        or it just means the next request will catch him up completely.

        Either way, let's store our currently proposed value too!
       */
      command.add_proposed_workload (this->proposal_id () + 1,
                                     byte_array);
   }   

   /*!
     The leader communicates the lowest proposal id currently processed by all hosts
     when sending an accept command, because:
     - the followers can then discard any history that has been processed by all hosts already;
     - the leader is the only host in the quorum that always has the most up-to-date view of
       the quorum (the followers do not communicate with each other).
    */
   command.set_lowest_proposal_id (quorum.lowest_proposal_id ());

   this->add_local_host_information (quorum, command);

   PAXOS_DEBUG ("step5 writing command");   

   follower_connection->write_command (command);
   
   PAXOS_DEBUG ("step5 reading command");   

   /*!
     We expect a response to this command.
    */
   follower_connection->read_command (
      std::bind (&strategy::receive_accepted,
                 this,
                 std::placeholders::_1,
                 client_connection,
                 client_command,
                 follower_endpoint,
                 std::ref (quorum),
                 std::placeholders::_2,
                 state));

}


/*! virtual */ void
strategy::accept (      
   tcp_connection_ptr                   leader_connection,
   detail::command const &              command,
   detail::quorum::server_view &        quorum,
   detail::paxos_context &              state)
{
   this->process_remote_host_information (command,
                                          quorum);

   detail::command response;
   
   /*!
     Default to an 'accepted' response
   */
   response.set_type (command::type_request_accepted);

   PAXOS_ASSERT_EQ (command.proposed_workload ().empty (), false);

   for (auto const & i : command.proposed_workload ())
   {
      PAXOS_DEBUG ("follower " << quorum.our_endpoint () << " storing proposed workload for id = " << i.first << ", our highest proposal_id = " << this->proposal_id ());

      PAXOS_ASSERT_EQ (i.first, this->proposal_id () + 1);

      /*! 
        First, process the workload and set it as output of the response
      */
      response.add_proposed_workload (i.first,
                                      state.processor () (i.first,
                                                          i.second));

      PAXOS_ASSERT_EQ (response.proposed_workload ().rbegin ()->second.empty (), false);
      
      /*!
        Now that the workload has been processed, store the currently accepted 
        proposal/value in our durable storage backend so other nodes can catch up 
        if they're disconnected for a short timespan.
      */
      storage_.accept (i.first,
                       i.second,
                       command.lowest_proposal_id ());

      /*!
        This is a bit of a hack, but we need to let the quorum know that our own
        proposal id has also increased, otherwise its leader election algorithm
        will not have all data required to determine the proper leader.
      */
      quorum.lookup_server (quorum.our_endpoint ()).set_highest_proposal_id (this->proposal_id ());

      /*!
        At this point, the proposal id should've been incremented by 1, since the
        proposal id is derived from storage_.
       */
      PAXOS_ASSERT_EQ (i.first, this->proposal_id ());
   }
   

   PAXOS_DEBUG ("step6 writing command");

   this->add_local_host_information (quorum, response);

   leader_connection->write_command (response);
}


/*! virtual */ void
strategy::receive_accepted (
   boost::optional <enum detail::error_code>    error,
   tcp_connection_ptr                           client_connection,
   detail::command                              client_command,
   boost::asio::ip::tcp::endpoint const &       follower_endpoint,
   detail::quorum::server_view &                quorum,
   detail::command const &                      command,
   boost::shared_ptr <struct state>             state)
{
   if (error)
   {
      PAXOS_WARN ("An error occured while receiving accepted from " << follower_endpoint << ": " << detail::to_string (*error));

      quorum.connection_died (follower_endpoint);

      state->accepted[follower_endpoint]    = response_reject;
      state->error_codes[follower_endpoint] = *error;
      state->responses[follower_endpoint]   = std::string ();
   }
   else
   {
      this->process_remote_host_information (command,
                                             quorum);

      /*!
        The state is set to 'accepted' in the previous promise phase, otherwise we
        shouldn't have reached this step at all.
       */
      PAXOS_ASSERT_EQ (state->accepted[follower_endpoint], response_ack);
      PAXOS_ASSERT (state->responses.find (follower_endpoint) == state->responses.end ());
      PAXOS_ASSERT (state->error_codes.find (follower_endpoint) == state->error_codes.end ());

      switch (command.type ())
      {
            case command::type_request_accepted:
               state->accepted[follower_endpoint]    = response_ack;

               PAXOS_ASSERT_EQ (command.proposed_workload ().empty (), false);

               /*!
                 Always store the response we received, since we also use that entry to see
                 whether all hosts have already replied. 
               */
               state->responses[follower_endpoint]   = 
                  command.proposed_workload ().rbegin ()->second;

               PAXOS_ASSERT_EQ (state->responses[follower_endpoint].empty (), false);

               break;

            case command::type_request_fail:
               state->accepted[follower_endpoint]    = response_reject;
               state->error_codes[follower_endpoint] = command.error_code ();
               state->responses[follower_endpoint]   = std::string ();
               break;

            default:
               /*!
                 Protocol error!
               */
               PAXOS_UNREACHABLE ();
      };
   }


   PAXOS_DEBUG ("leader got response = " << state->responses[follower_endpoint] << ", follower = " << follower_endpoint);



   std::string workload;

   if (state->connections.size () == state->responses.size ())
   {
      boost::optional <enum error_code> last_error;

      for (auto const & i : state->accepted)
      {
         if (i.second == response_reject)
         {
            /*!
              We always have recorded an error code in case something fails.
             */
            PAXOS_ASSERT (state->error_codes.find (i.first) != state->error_codes.end ());
            last_error = state->error_codes.find (i.first)->second;

            PAXOS_ASSERT_NE (*last_error, detail::no_error);
         }
      }
      
      if (last_error.is_initialized () == false)
      {
         /*!
           One of the requirements of our protocol is that if one node N1 replies
           to proposal P with response R, node N2 must have the exact same response
           for the same proposal.
           
           The code below validates this requirement.
         */
         for (auto const & i : state->responses)
         {
            if (workload.empty () == true)
            {
               workload = i.second;
               PAXOS_ASSERT (workload.empty () == false);
            }
            else if (workload != i.second)
            {
               last_error = detail::error_inconsistent_response;
            }

         }
      }

      if (last_error.is_initialized () == false)
      {
         PAXOS_DEBUG ("step7 writing command");   
         detail::command response;
         response.set_type (command::type_request_accepted);
         response.set_workload (workload);

         this->add_local_host_information (quorum,
                                           response);

         client_connection->write_command (response);
      }
      else
      {
         /*!
           Ok, so, at this point we know that an error has occured. Now, there could be
           multiple errors at stake, but we only get the chance to report one error to
           the client.

           What we will do is simply with the last error we have seen.
          */
         PAXOS_DEBUG ("step7 writing error command");

         handle_error (*last_error,
                       quorum,
                       client_connection);
      }
   }
}


/*! virtual */ void
strategy::handle_error (
   enum detail::error_code      error,
   quorum::server_view const &  quorum,
   tcp_connection_ptr           client_connection)
{
   detail::command response;
   response.set_type (command::type_request_error);
   response.set_error_code (error);
   
   this->add_local_host_information (quorum,
                                     response);

   client_connection->write_command (response);   
}



/*! virtual */ void
strategy::add_local_host_information (
   quorum::server_view const &  quorum,
   detail::command &            output)
{
   detail::quorum::server const & server = quorum.lookup_server (quorum.our_endpoint ());
   output.set_host_id (server.id ());
   output.set_host_endpoint (server.endpoint ());
   output.set_highest_proposal_id (this->proposal_id ());
}

/*! virtual */ void
strategy::process_remote_host_information (
   detail::command const &      command,
   quorum::server_view &        output)
{
   detail::quorum::server & server = 
      output.lookup_server (command.host_endpoint ());

   server.set_id (command.host_id ());
   server.set_highest_proposal_id (command.highest_proposal_id ());
}


/*! virtual */ int64_t
strategy::proposal_id ()
{
   return storage_.highest_proposal_id ();
}

}; }; }; }; };
