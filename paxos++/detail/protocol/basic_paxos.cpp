#include "../quorum.hpp"

#include "command.hpp"
#include "protocol.hpp"
#include "basic_paxos.hpp"

namespace paxos { namespace detail { namespace protocol {

basic_paxos::basic_paxos (
   detail::protocol::protocol & protocol)
   : protocol_ (protocol),
     proposal_id_ (0)
{
}


void
basic_paxos::start (
   tcp_connection::pointer      client_connection,
   command const &              command)
{
   /*!
     A new paxos request can only be received by a leader, otherwise the client sent
     the request to the wrong node.
    */
   PAXOS_ASSERT (protocol_.quorum ().we_are_the_leader () == true);

   /*!
     As defined by the Paxos protocol, for each new request, we must increment the proposal
     id.
    */
   ++proposal_id_;

   /*!
     Now, tell all nodes in the quorum to prepare this request.
    */   
   boost::shared_ptr <struct state> state (new struct state ());

   for (auto const & i : protocol_.quorum ().servers ())
   {
      boost::asio::ip::tcp::endpoint const & endpoint = i.first;      

      switch (i.second.state ())
      {
            case remote_server::state_dead:
               PAXOS_WARN ("Skipping node in dead state: " << i.first);
               break;

            default:
               send_prepare (client_connection,
                             endpoint,
                             i.second.connection (),
                             command.workload (),
                             state);
               break;
      };
   }
}


void
basic_paxos::send_prepare (
   tcp_connection::pointer                      client_connection,
   boost::asio::ip::tcp::endpoint const &       server_endpoint,
   tcp_connection::pointer                      server_connection,
   std::string const &                          byte_array,
   boost::shared_ptr <struct state>             state)
{
   /*!
     By default, our server hasn't sent a response yet.
    */
   PAXOS_ASSERT (state->accepted.find (server_endpoint) == state->accepted.end ());
   state->accepted[server_endpoint] = response_none;

   command command;
   command.set_type (command::type_request_prepare);
   command.set_proposal_id (proposal_id_);


   PAXOS_DEBUG ("sending prepare to node " << server_endpoint);

   protocol_.write_command (command,
                            server_connection);

   protocol_.read_command (server_connection,
                           boost::bind (&basic_paxos::receive_promise,
                                        this,
                                        client_connection,
                                        server_endpoint,
                                        server_connection,
                                        byte_array,
                                        _1,
                                        state));
   
}


void
basic_paxos::receive_prepare (
   tcp_connection::pointer      connection,
   command const &              command)
{
   PAXOS_ASSERT (protocol_.quorum ().we_are_the_leader () == false);

   PAXOS_DEBUG ("received prepare, connection = " << connection.get ());

   detail::protocol::command ret;

   if (command.proposal_id () > proposal_id_)
   {
      proposal_id_ = command.proposal_id ();
      ret.set_type (command::type_request_promise);
   }
   else
   {
      ret.set_type (command::type_request_fail);
   }

   protocol_.write_command (ret,
                            connection);
}

void
basic_paxos::receive_promise (
   tcp_connection::pointer                      client_connection,
   boost::asio::ip::tcp::endpoint const &       server_endpoint,
   tcp_connection::pointer                      server_connection,
   std::string                                  byte_array,
   command const &                              command,
   boost::shared_ptr <struct state>             state)
{
   PAXOS_ASSERT (protocol_.quorum ().we_are_the_leader () == true);


   switch (command.type ())
   {
         case command::type_request_promise:
            state->accepted[server_endpoint] = response_ack;
            break;

         case command::type_request_fail:
            state->accepted[server_endpoint] = response_reject;
            break;

         default:
            PAXOS_UNREACHABLE ();
   };


   bool everyone_responded = true;
   bool everyone_promised  = true;
   for (auto const & i : state->accepted)
   {
      everyone_responded = everyone_responded && (i.second == response_ack || i.second == response_reject);
      everyone_promised  = everyone_promised && i.second == response_ack;
   }

   if (everyone_responded == true && everyone_promised == false)
   {
      /*!
        This means that every host has given a response, yet not everyone has actually
        accepted our proposal id. 

        We will send an error command to the client informing about the failed command.
       */
      detail::protocol::command tmp_command;
      tmp_command.set_type (command::type_request_error);
      tmp_command.set_error_code (paxos::error_incorrect_proposal);
      protocol_.write_command (tmp_command,
                               client_connection);
   }
   else if (everyone_promised == true)
   {
      /*!
        This means that all nodes in the quorum have responded, and they all agree with
        the proposal id, yay!

        Now that all these nodes have promised to accept any request with the specified
        proposal id, let's send them an accept command.
       */
      PAXOS_INFO ("all nodes promised to accept!");

      for (auto const & i : state->accepted)
      {
         send_accept (client_connection,
                      i.first,
                      protocol_.quorum ().lookup (i.first).connection (),
                      byte_array,
                      state);
      }

      /*! 
        Since we are the leader, we are not part of the registered quorum, and we
        need to process our workload manually.

        \todo This is actually a bit of a kludge. The better workaround would be to
              really have a separate leader, and perhaps have the leader maintain
              a connection with its own follower. Less performance but more elegant
              design.
      */
      detail::protocol::command tmp_command;
      tmp_command.set_type (command::type_request_accepted);
      tmp_command.set_workload (
         protocol_.process_workload (command.workload ()));

      state->accepted[protocol_.quorum ().self ().endpoint_] = response_ack;

      receive_accepted (client_connection,
                        protocol_.quorum ().self ().endpoint_,
                        tmp_command,
                        state);

   }
   
}

void
basic_paxos::send_accept (
   tcp_connection::pointer                      client_connection,
   boost::asio::ip::tcp::endpoint const &       server_endpoint,
   tcp_connection::pointer                      server_connection,
   std::string const &                          byte_array,
   boost::shared_ptr <struct state>             state)
{
   PAXOS_ASSERT (state->accepted[server_endpoint] == response_ack);


   command command;
   command.set_type (command::type_request_accept);
   command.set_workload (byte_array);


   PAXOS_DEBUG ("sending accept to node " << server_endpoint);

   protocol_.write_command (command,
                            server_connection);

   protocol_.read_command (server_connection,
                           boost::bind (&basic_paxos::receive_accepted,
                                        this,
                                        client_connection,
                                        server_endpoint,
                                        _1,
                                        state));   
}


void
basic_paxos::receive_accept (
   tcp_connection::pointer      connection,
   command const &              input_command)
{

   command command;
   command.set_type (command::type_request_accepted);
   command.set_workload (
      protocol_.process_workload (input_command.workload ()));

   protocol_.write_command (command,
                            connection);
}



void
basic_paxos::receive_accepted (
   tcp_connection::pointer                      client_connection,
   boost::asio::ip::tcp::endpoint const &       server_endpoint,
   command const &                              command,
   boost::shared_ptr <struct state>             state)
{

   PAXOS_ASSERT (state->responses.find (server_endpoint) == state->responses.end ());
   state->responses[server_endpoint] = command.workload ();

   std::string response;

   if (state->responses.size () == state->accepted.size ())
   {
      PAXOS_INFO ("All responses have been received, yay!");

      bool all_same_response = true;
      for (auto const & i : state->responses)
      {
         /*!
           One of the requirements of our protocol is that if one node N1 replies
           to proposal P with response R, node N2 must have the exact same response
           for the same proposal.

           The code below validates this requirement.
          */
         if (response.empty () == true)
         {
            response = i.second;
         }
         else if (response != i.second)
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
         protocol_.write_command (command,
                                  client_connection);
      } 
      else
      {
         /*!
           We're going to inform the client about an inconsistent response here. Perhaps
           he can recover from there.
          */
         detail::protocol::command tmp_command;
         tmp_command.set_type (command::type_request_error);
         tmp_command.set_error_code (paxos::error_inconsistent_response);
         protocol_.write_command (tmp_command,
                                  client_connection);
      }
   }

}



}; }; };
