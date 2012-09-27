#include <vector>

#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/detail/endian.hpp>

#include "../util/debug.hpp"
#include "../util/conversion.hpp"
#include "../../quorum.hpp"
#include "../connection_pool.hpp"

#include "pb/adapter.hpp"
#include "pb/command.pb.h"

#include "protocol.hpp"

namespace paxos { namespace detail { namespace protocol {

protocol::protocol (
   paxos::detail::connection_pool &     connection_pool,
   paxos::quorum &                      quorum)
   : connection_pool_ (connection_pool),
     quorum_ (quorum),

     elect_leader_ (*this)
{
}

paxos::detail::connection_pool &
protocol::connection_pool ()
{
   return connection_pool_;
}

paxos::quorum &
protocol::quorum ()
{
   return quorum_;
}

void
protocol::bootstrap ()
{
   PAXOS_ASSERT (elect_leader_.size () == 0);
   
   elect_leader_.create ().start ();
}


void
protocol::new_connection (
   tcp_connection &     connection)
{
   read_command (connection,
                 boost::bind (&protocol::handle_command,
                              this,
                              boost::ref (connection),
                              _1));
}

void
protocol::handle_command (
   tcp_connection &     connection,
   pb::command const &  command)
{
   PAXOS_DEBUG ("got new command!");
   
   
}


/*! static */ void
protocol::write_command (
   pb::command const &          command,
   detail::tcp_connection &     destination)
{
   std::string binary_string = pb::adapter::to_string (command);
   uint32_t size             = binary_string.size ();

   std::string buffer        = util::conversion::to_byte_array (size) + binary_string;

   PAXOS_DEBUG ("sending command of " << buffer.size () << " bytes to other remote connection");

   destination.write (buffer);
}


/*! static */ void
protocol::read_command (
   tcp_connection &                                     connection,
   boost::function <void (pb::command const &)> const & callback)
{
   boost::shared_array <char> buffer (new char[4]);

   boost::asio::async_read (
      connection.socket (), 
      boost::asio::buffer (buffer.get (), 4), 
      boost::bind (&protocol::read_command_parse_size,
                   /*!
                     \warning this assumes connection still is valid when parse_command
                              is called. This can be dangerous! Make sure it does not
                              race with cleaning up the connection_pool!
                   */
                   boost::ref (connection),

                   _1,
                   _2,

                   buffer,

                   /*! 
                     This is EXTREMELY ugly, but since we completely lose our
                     stack at this point, we can't rely on output to remain valid
                     when our parse_command () function is called back.
                     
                     Our solution? Wrap the callback function in a shared_ptr.
                     It's ugly, and needs a more elegant solution.
                     
                     \todo Fix more elegantly. The real problem lies in the fact
                           that our callback is an indirection from parse_command. Perhaps
                           find a way to copy our output callback by value?
                   */
                   boost::shared_ptr <read_command_callback_type> (new read_command_callback_type (callback))));
}



/*! static */ void
protocol::read_command_parse_size (
   tcp_connection &                                     connection,
   boost::system::error_code const &                    error,
   size_t                                               bytes_transferred,
   boost::shared_array <char>                           bytes_buffer,
   boost::shared_ptr <read_command_callback_type>       callback)
{
   PAXOS_ASSERT (bytes_transferred == 4);

   std::string bytes_raw (bytes_buffer.get (), 4);
   uint32_t    bytes = util::conversion::from_byte_array <uint32_t> (bytes_raw);

   boost::shared_array <char> command_buffer (new char[bytes]);   

   //! Now send a request for the amount of bytes we just parsed
   boost::asio::async_read (
      connection.socket (), 
      boost::asio::buffer (command_buffer.get (), bytes), 
      boost::bind (&read_command_parse_command,
                   boost::ref (connection),
                   _1,
                   _2,
                   command_buffer,
                   callback));
}

/*! static */ void
protocol::read_command_parse_command (
   tcp_connection &                                     connection,
   boost::system::error_code const &                    error,
   size_t                                               bytes_transferred,
   boost::shared_array <char>                           buffer,
   boost::shared_ptr <read_command_callback_type>       callback)
{
   std::string byte_array (buffer.get (),
                           bytes_transferred);

   (*callback) (pb::adapter::from_string (byte_array));
}


}; }; };
