#include <boost/asio/read.hpp>

#include "util/conversion.hpp"
#include "util/debug.hpp"

#include "tcp_connection.hpp"
#include "parser.hpp"

namespace paxos { namespace detail {



/*! static */ void
parser::write_command (
   tcp_connection_ptr   connection,
   command const &      command)
{
   std::string binary_string = command::to_string (command);
   uint32_t size             = binary_string.size ();

   std::string buffer        = util::conversion::to_byte_array (size) + binary_string;

   connection->write (buffer);
}


/*! static */ void
parser::read_command (
   tcp_connection_ptr   connection,
   callback_function    callback)
{
   boost::shared_array <char> buffer (new char[4]);

   PAXOS_DEBUG ("reading command size from connection = " << connection.get ());

   boost::asio::async_read (
      connection->socket (), 
      boost::asio::buffer (buffer.get (), 4), 
      std::bind (&parser::read_command_parse_size,
                   
                 connection,
                   
                 std::placeholders::_1,
                 std::placeholders::_2,
                   
                 buffer,
                   
                 callback));
}

/*! static */ void
parser::read_command_parse_size (
   tcp_connection_ptr                   connection,
   boost::system::error_code const &    error,
   size_t                               bytes_transferred,
   boost::shared_array <char>           bytes_buffer,
   callback_function                    callback)
{
   if (error)
   {
      callback (detail::error_connection_close,
                command ());
   }
   else
   {
      PAXOS_ASSERT (bytes_transferred == 4);
      
      std::string bytes_raw (bytes_buffer.get (), 4);
      uint32_t    bytes = util::conversion::from_byte_array <uint32_t> (bytes_raw);

      boost::shared_array <char> command_buffer (new char[bytes]);   

      PAXOS_DEBUG ("reading command data from connection = " << connection.get ());

      //! Now send a request for the amount of bytes we just parsed
      boost::asio::async_read (
         connection->socket (), 
         boost::asio::buffer (command_buffer.get (), bytes), 
         std::bind (&parser::read_command_parse_command,
                    connection,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    command_buffer,
                    callback));
   }
}

/*! static */ void
parser::read_command_parse_command (
   tcp_connection_ptr                   connection,
   boost::system::error_code const &    error,
   size_t                               bytes_transferred,
   boost::shared_array <char>           buffer,
   callback_function                    callback)
{
   if (error)
   {
      callback (detail::error_connection_close,
                command ());
   }
   else
   {
      std::string byte_array (buffer.get (),
                              bytes_transferred);

      PAXOS_DEBUG ("callback for connection = " << connection.get ());

      callback (boost::none,
                command::from_string (byte_array));
   }
}



}; };
