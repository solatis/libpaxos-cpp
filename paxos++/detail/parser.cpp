#include <boost/asio/read.hpp>

#include "util/conversion.hpp"
#include "parser.hpp"

namespace paxos { namespace detail {



/*! static */ void
parser::write_command (
   tcp_connection::pointer      connection,
   command const &              command)
{
   std::string binary_string = command::to_string (command);
   uint32_t size             = binary_string.size ();

   std::string buffer        = util::conversion::to_byte_array (size) + binary_string;

   connection->write (buffer);
}


/*! static */ void
parser::read_command (
   tcp_connection::pointer      connection,
   callback_function            callback)
{
   boost::shared_array <char> buffer (new char[4]);

   boost::asio::async_read (
      connection->socket (), 
      boost::asio::buffer (buffer.get (), 4), 
      boost::bind (&parser::read_command_parse_size,

                   connection,

                   _1,
                   _2,

                   buffer,

                   callback));
}

/*! static */ void
parser::read_command_parse_size (
   tcp_connection::pointer              connection,
   boost::system::error_code const &    error,
   size_t                               bytes_transferred,
   boost::shared_array <char>           bytes_buffer,
   callback_function                    callback)
{
   if (error)
   {
      PAXOS_WARN ("An error occured while reading command size: " << error.message ());
      return;
   }

   PAXOS_ASSERT (bytes_transferred == 4);

   std::string bytes_raw (bytes_buffer.get (), 4);
   uint32_t    bytes = util::conversion::from_byte_array <uint32_t> (bytes_raw);

   boost::shared_array <char> command_buffer (new char[bytes]);   

   //! Now send a request for the amount of bytes we just parsed
   boost::asio::async_read (
      connection->socket (), 
      boost::asio::buffer (command_buffer.get (), bytes), 
      boost::bind (&parser::read_command_parse_command,
                   
                   connection,
                   _1,
                   _2,
                   command_buffer,
                   callback));
}

/*! static */ void
parser::read_command_parse_command (
   tcp_connection::pointer              connection,
   boost::system::error_code const &    error,
   size_t                               bytes_transferred,
   boost::shared_array <char>           buffer,
   callback_function                    callback)
{
   /*!
     At this point, we have a command, so we can cancel any timeouts running on the connection.
    */
   connection->cancel_timeout ();

   if (error)
   {
      PAXOS_WARN ("An error occured while reading command: " << error.message ());
      return;
   }

   std::string byte_array (buffer.get (),
                           bytes_transferred);

   callback (command::from_string (byte_array));
}



}; };
