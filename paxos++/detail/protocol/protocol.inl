#include <iterator>

namespace paxos { namespace detail { namespace protocol {

template <typename Callback>
inline /*! static */ void
protocol::read_command (
   tcp_connection &     connection,
   Callback const &     output)
{
   boost::shared_array <char> buffer (new char[4]);

   boost::asio::async_read (
      connection.socket (), 
      boost::asio::buffer (buffer.get (), 4), 
      boost::bind (&protocol::parse_size <Callback>,
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
                   boost::shared_ptr <Callback> (new Callback (output))));
}



template <typename Callback>
inline /*! static */ void
protocol::parse_size (
   tcp_connection &                     connection,
   boost::system::error_code const &    error,
   size_t                               bytes_transferred,
   boost::shared_array <char>           bytes_buffer,
   boost::shared_ptr <Callback>         callback)
{
   PAXOS_ASSERT (bytes_transferred == 4);

   std::string bytes_raw (bytes_buffer.get (), 4);
   uint32_t    bytes = util::conversion::from_byte_array <uint32_t> (bytes_raw);

   boost::shared_array <char> command_buffer (new char[bytes]);   

   //! Now send a request for the amount of bytes we just parsed
   boost::asio::async_read (
      connection.socket (), 
      boost::asio::buffer (command_buffer.get (), bytes), 
      boost::bind (&parse_command <Callback>,
                   boost::ref (connection),
                   _1,
                   _2,
                   command_buffer,
                   callback));
}

template <typename Callback>
inline /*! static */ void
protocol::parse_command (
   tcp_connection &                     connection,
   boost::system::error_code const &    error,
   size_t                               bytes_transferred,
   boost::shared_array <char>           buffer,
   boost::shared_ptr <Callback>         callback)
{
   std::string byte_array (buffer.get (),
                           bytes_transferred);

   (*callback) (pb::adapter::from_string (byte_array));
}



}; }; };
