#include "../../../quorum/quorum.hpp"
#include "../../../paxos_state.hpp"
#include "../../../command.hpp"
#include "../../../parser.hpp"
#include "../../../util/debug.hpp"

#include "request.hpp"

namespace paxos { namespace detail { namespace strategies { namespace basic_paxos { namespace protocol {

/*! static */ void
request::step1 (      
   tcp_connection::pointer      client_connection,
   detail::command const &      command,
   detail::quorum::quorum &     quorum,
   detail::paxos_state &        proposal_id)
{

   PAXOS_ASSERT (quorum.our_leader () == quorum.our_endpoint ());

   /*!
     At the start of any request, we should, as defined in the Paxos protocol, increment
     our current proposal id.
    */
   ++(proposal_id.proposal_id ());

   /*!
     Keeps track of the current state / which servers have responded, etc.
    */
   boost::shared_ptr <struct state> state (new struct state ());

   /*!
     Tell all nodes within this quorum to prepare this request.
    */
   for (quorum::server & server : quorum.live_servers ())
   {
      step2 (client_connection,
             quorum.our_endpoint (),
             server.endpoint (),
             server.connection (),
             proposal_id.proposal_id (),
             command.workload (),
             state);
   }
}


/*! static */ void
request::step2 (
   tcp_connection::pointer                      client_connection,
   boost::asio::ip::tcp::endpoint const &       leader_endpoint,   
   boost::asio::ip::tcp::endpoint const &       follower_endpoint,
   tcp_connection::pointer                      follower_connection,
   uint64_t                                     proposal_id,
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
   command.set_proposal_id (proposal_id);
   command.set_host_endpoint (leader_endpoint);


   PAXOS_DEBUG ("step2 writing command");

   parser::write_command (follower_connection,
                          command);

   PAXOS_DEBUG ("step2 reading command");   

   /*!
     We expect either an 'ack' or a 'reject' response to this command.
    */
   parser::read_command (follower_connection,
                         boost::bind (&request::step4,
                                      client_connection,
                                      leader_endpoint,
                                      follower_endpoint,
                                      follower_connection,
                                      proposal_id,
                                      byte_array,
                                      _1,
                                      state));
}

/*! static */ void
request::step3 (      
   tcp_connection::pointer   leader_connection,
   detail::command const &   command,
   detail::quorum::quorum &  quorum,
   detail::paxos_state &     state)
{
   detail::command response;

   if (command.host_endpoint () == quorum.our_endpoint ())
   {
      /*!
        This is the leader sending the 'prepare' to itself, always ACK
       */
      response.set_type (command::type_request_promise);
   }
   else if (command.proposal_id () > state.proposal_id ())
   {
      state.proposal_id () = command.proposal_id ();
      response.set_type (command::type_request_promise);
   }
   else
   {
      response.set_type (command::type_request_fail);
   }


   PAXOS_DEBUG ("step3 writing command");   

   parser::write_command (leader_connection,
                          response);
}


/*! static */ void
request::step4 (
   tcp_connection::pointer                      client_connection,
   boost::asio::ip::tcp::endpoint const &       leader_endpoint,
   boost::asio::ip::tcp::endpoint const &       follower_endpoint,
   tcp_connection::pointer                      follower_connection,
   uint64_t                                     proposal_id,
   std::string                                  byte_array,
   detail::command const &                      command,
   boost::shared_ptr <struct state>             state)
{
   PAXOS_ASSERT (state->connections[follower_endpoint] == follower_connection);

   switch (command.type ())
   {
         case command::type_request_promise:
            state->accepted[follower_endpoint] = response_ack;
            break;

         case command::type_request_fail:
            state->accepted[follower_endpoint] = response_reject;
            break;

         default:
            /*!
              Protocol error!
            */
            PAXOS_UNREACHABLE ();
   };



   bool everyone_responded = (state->connections.size () == state->responses.size ());

   bool everyone_promised  = true;
   for (auto const & i : state->accepted)
   {
      everyone_promised  = everyone_promised && i.second == response_ack;
   }


   if (everyone_responded == true && everyone_promised == false)
   {
      /*!
        This means that every host has given a response, yet not everyone has actually
        accepted our proposal id. 

        We will send an error command to the client informing about the failed command.
       */
      PAXOS_DEBUG ("step4 writing error command");   

      detail::command response;
      response.set_type (command::type_request_error);
      response.set_error_code (paxos::error_incorrect_proposal);
      parser::write_command (client_connection,
                             response);
   }
   else if (everyone_promised == true)
   {
      /*!
        This means that all nodes in the quorum have responded, and they all agree with
        the proposal id, yay!

        Now that all these nodes have promised to accept any request with the specified
        proposal id, let's send them an accept command.
       */

      for (auto & i : state->connections)
      {
         step5 (client_connection,
                leader_endpoint,
                i.first,
                i.second,
                proposal_id,
                byte_array,
                state);
      }
   }
}

/*! static */ void
request::step5 (
   tcp_connection::pointer                      client_connection,
   boost::asio::ip::tcp::endpoint const &       leader_endpoint,
   boost::asio::ip::tcp::endpoint const &       follower_endpoint,
   tcp_connection::pointer                      follower_connection,
   uint64_t                                     proposal_id,
   std::string const &                          byte_array,
   boost::shared_ptr <struct state>             state)
{
   
   PAXOS_ASSERT (state->connections[follower_endpoint] == follower_connection);
   PAXOS_ASSERT (state->accepted[follower_endpoint] == response_ack);


   command command;
   command.set_type (command::type_request_accept);
   command.set_proposal_id (proposal_id);
   command.set_host_endpoint (leader_endpoint);
   command.set_workload (byte_array);

   PAXOS_DEBUG ("step5 writing command");   
   
   parser::write_command (follower_connection,
                          command);

   PAXOS_DEBUG ("step5 reading command");   

   /*!
     We expect a response to this command.
    */
   parser::read_command (follower_connection,
                         boost::bind (&request::step7,
                                      client_connection,
                                      leader_endpoint,
                                      follower_endpoint,
                                      follower_connection,
                                      _1,
                                      state));

}


/*! static */ void
request::step6 (      
   tcp_connection::pointer      leader_connection,
   detail::command const &      command,
   detail::quorum::quorum &     quorum,
   detail::paxos_state &        state)
{
   /*!
     If the proposal id's do not match, something went terribly wrong! Most likely a switch
     of leaders during the operation.

     For now, we'll just assert on it, until the problem actually arises in the wild.
    */
   PAXOS_ASSERT (command.proposal_id () == state.proposal_id ());


   detail::command response;
   response.set_type (command::type_request_accepted);
   response.set_workload (
      state.processor () (command.workload ()));

   PAXOS_DEBUG ("step6 writing command");   

   parser::write_command (leader_connection,
                          response);

   
}


/*! static */ void
request::step7 (
   tcp_connection::pointer                      client_connection,
   boost::asio::ip::tcp::endpoint const &       leader_endpoint,
   boost::asio::ip::tcp::endpoint const &       follower_endpoint,
   tcp_connection::pointer                      follower_connection,
   detail::command const &                      command,
   boost::shared_ptr <struct state>             state)
{
   PAXOS_ASSERT (state->connections[follower_endpoint] == follower_connection);
   PAXOS_ASSERT (state->accepted[follower_endpoint] == response_ack);
   PAXOS_ASSERT (state->responses.find (follower_endpoint) == state->responses.end ());

   /*!
     Let's store the response we received!
   */
   state->responses[follower_endpoint] = command.workload ();

   std::string workload;

   PAXOS_ASSERT (state->connections.size () == state->accepted.size ());
   if (state->connections.size () == state->responses.size ())
   {
      bool all_same_response = true;
      
      for (auto const & i : state->responses)
      {
         /*!
           One of the requirements of our protocol is that if one node N1 replies
           to proposal P with response R, node N2 must have the exact same response
           for the same proposal.

           The code below validates this requirement.
         */
         if (workload.empty () == true)
         {
            workload = i.second;
         }
         else if (workload != i.second)
         {
            all_same_response = false;
         }
      }

      if (all_same_response == true)
      {
         /*!
           Send a copy of the last command to the client, since the workload should be the
           same for all responses.
         */
         PAXOS_DEBUG ("step7 writing command");   

         parser::write_command (client_connection,
                                command);
      }
      else
      {
         /*!
           We're going to inform the client about an inconsistent response here. Perhaps
           he can recover from there.
         */
         PAXOS_DEBUG ("step7 writing error command");   

         detail::command response;
         response.set_type (command::type_request_error);
         response.set_error_code (paxos::error_inconsistent_response);
         parser::write_command (client_connection,
                                response);
      }

   }
}


}; }; }; }; };
