#include <boost/asio/placeholders.hpp>

#include "../../quorum.hpp"

#include "command.hpp"
#include "protocol.hpp"
#include "announce_leadership.hpp"

namespace paxos { namespace detail { namespace protocol {

announce_leadership::announce_leadership (
   detail::protocol::protocol & protocol)
   : protocol_ (protocol)
{
}

void
announce_leadership::start ()
{
   step1 ();
}

void
announce_leadership::receive_leader (
   tcp_connection::pointer      connection,
   command const &              command)
{
   step3 (connection,
          command);
}

void
announce_leadership::step1 ()
{
   //! This can only be initiated when we actually are the current leader
   PAXOS_ASSERT (protocol_.quorum ().we_are_the_leader () == true);

   for (auto const & i : protocol_.quorum ().servers ())
   {
      if (i.second.state () == remote_server::state_dead)
      {
         /*!
           Skip all dead nodes; you might think that since we are now establishing
           connections with all other nodes it would be an excellent time to try
           to connect to them, but you're wrong: the state_dead should only be
           altered by the handshake.
          */
         continue;
      }

      boost::asio::ip::tcp::endpoint const & endpoint = i.first;      

      tcp_connection::pointer connection = tcp_connection::create (protocol_.io_service ());
         
      connection->socket ().async_connect (endpoint,
                                           boost::bind (&announce_leadership::step2,
                                                        this,
                                                        boost::ref (endpoint),
                                                        connection,
                                                        boost::asio::placeholders::error));
   }
}


void
announce_leadership::step2 (
   boost::asio::ip::tcp::endpoint const &       endpoint,
   tcp_connection::pointer                      connection,
   boost::system::error_code const &            error)
{
   if (error)
   {
      PAXOS_WARN ("An error occured while establishing a connection");
      return;
   }


   command command;
   command.set_type (command::type_leader_announce);
   command.set_host_id (protocol_.quorum ().self ().id_);
   command.set_host_endpoint (protocol_.quorum ().self ().endpoint_);

   
   /*!
     Send this command to the other side, which will enter in announce_leaders::receive_leader
     as defined in protocol.cpp's handle_command () function.
    */
   protocol_.write_command (command,
                            connection);
     
   /*!
     And ensure this connection is now associated with this host
    */
   protocol_.quorum ().lookup (endpoint).set_connection (connection);
   protocol_.quorum ().lookup (endpoint).set_state (detail::remote_server::state_follower);
}

void
announce_leadership::step3 (
   tcp_connection::pointer      connection,
   command const &              command)
{
   PAXOS_ASSERT (protocol_.quorum ().current_leader () == command.host_endpoint ());

   /*!
     We do not have to say anything back, just store the connection
    */
   protocol_.quorum ().lookup (command.host_endpoint ()).set_connection (connection);
   protocol_.quorum ().adjust_our_state (detail::remote_server::state_follower);
}

}; }; };
