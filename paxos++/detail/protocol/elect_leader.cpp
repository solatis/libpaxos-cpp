#include <boost/asio/placeholders.hpp>

#include "../../quorum.hpp"

#include "command.hpp"
#include "protocol.hpp"
#include "elect_leader.hpp"

namespace paxos { namespace detail { namespace protocol {

elect_leader::elect_leader (
   detail::protocol::protocol & protocol)
   : protocol_ (protocol)
{
}

void
elect_leader::start ()
{
   step1 ();
}

void
elect_leader::receive_leader_claim (
   tcp_connection::pointer     connection,
   command const &      command)
{
   step3 (connection,
          command);
}

void
elect_leader::step1 ()
{
   if (protocol_.quorum ().determine_leader () == protocol_.quorum ().self ().endpoint_)
   {
      PAXOS_DEBUG ("i am the leader, telling all other nodes!");


      boost::shared_ptr <struct state> state (new struct state ());

      //! Claim victory!
      for (auto const & i : protocol_.quorum ().servers ())
      {
         if (i.second.state () == remote_server::state_dead)
         {
            /*!
              We aren't expecting a response from any dead nodes, and they will
              not participate in this leader election anyway.
            */
            PAXOS_DEBUG ("skipping dead node " << i.first);
            continue;
         }

         boost::asio::ip::tcp::endpoint const & endpoint = i.first;      
         PAXOS_ASSERT (state->responses.find (endpoint) == state->responses.end ());
         state->responses[endpoint] = response_none;

         tcp_connection::pointer connection = tcp_connection::create (protocol_.io_service ());
         
         connection->socket ().async_connect (endpoint,
                                              boost::bind (&elect_leader::step2,
                                                           this,
                                                           boost::ref (endpoint),
                                                           connection,
                                                           boost::asio::placeholders::error,
                                                           state));
      }
   }
}


void
elect_leader::step2 (
   boost::asio::ip::tcp::endpoint const &       endpoint,
   tcp_connection::pointer                      connection,
   boost::system::error_code const &            error,
   boost::shared_ptr <struct state>             state)
{
   if (error)
   {
      PAXOS_WARN ("An error occured while establishing a connection");
      return;
   }


   command command;
   command.set_type (command::type_leader_claim);
   command.set_host_id (protocol_.quorum ().self ().id_);
   command.set_host_endpoint (protocol_.quorum ().self ().endpoint_);


   
   /*!
     Send this command to the other side, which will enter in handshake::receive_leader_claim
     as defined in protocol.cpp's handle_command () function.
    */
   protocol_.write_command (command,
                            connection);
     
   /*!
     And now we expect a response from the other side in response to our handshake request.
    */
   connection->start_timeout (boost::posix_time::milliseconds (3000));
   protocol_.read_command (connection,
                           boost::bind (&elect_leader::step4,
                                        this,
                                        boost::ref (endpoint),
                                        connection,
                                        _1,
                                        state));
}

void
elect_leader::step3 (
   tcp_connection::pointer      connection,
   command const &              command)
{
   detail::protocol::command ret;

   if (protocol_.quorum ().determine_leader () == command.host_endpoint ())
   {
      ret.set_type (command::type_leader_claim_ack);      
   }
   else
   {
      /*!
        This means something went really wrong! One node in the quorum thinks it's
        a leader, while another node thinks someone *else* is the leader!

        This means that the representation of the quorum in both nodes is different,
        but they *are* able to communicate with each other!
       */
      PAXOS_WARN ("Quorum in inconsistent state? Rejecting leadership claim");
      ret.set_type (command::type_leader_claim_reject);
   }

   protocol_.write_command (ret,
                            connection);
}


void
elect_leader::step4 (
   boost::asio::ip::tcp::endpoint const &       endpoint,
   tcp_connection::pointer                      connection,
   command const &                              command,
   boost::shared_ptr <struct state>             state)
{
   PAXOS_ASSERT (state->responses.find (endpoint) != state->responses.end ());
   PAXOS_ASSERT (state->responses[endpoint] == response_none);

   switch (command.type ())
   {
         case command::type_leader_claim_ack:
            PAXOS_DEBUG ("received ack on leader!");
            state->responses[endpoint] = response_ack;
            break;

         case command::type_leader_claim_reject:
            PAXOS_WARN ("received reject on leader!");
            state->responses[endpoint] = response_reject;
            break;
            
         default:
            //! This should *never* occur!
            PAXOS_UNREACHABLE ();
   };

   bool everyone_agrees = true;
   for (auto const & i : state->responses)
   {
      everyone_agrees = everyone_agrees && i.second == response_ack;
   }

   if (everyone_agrees == true)
   {
      PAXOS_INFO ("i am the new leader!");
      protocol_.quorum ().set_leader (protocol_.quorum ().self ().endpoint_);
   }
}

}; }; };
