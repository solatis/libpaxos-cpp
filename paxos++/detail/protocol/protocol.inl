#include <iterator>

namespace paxos { namespace detail { namespace protocol {

template <typename Callback>
inline /*! static */ void
protocol::read_command (
   tcp_connection &     input,
   Callback const &     output)
{
   typedef boost::asio::buffers_iterator <boost::asio::streambuf::const_buffers_type> iterator_type;

   boost::asio::async_read_until (
      input.socket (), 
      input.read_buffer (), 
      match_command <iterator_type>,
      boost::bind (&protocol::parse_command <Callback>,
                   /*!
                     \warning this assumes input still is valid when parse_command
                     is called. This can be dangerous! Make sure it does not
                     race with cleaning up the connection_pool!
                   */
                   boost::ref (input),

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

template <typename Iterator>
inline /*! static */ std::pair <Iterator, bool>
protocol::match_command (
   Iterator     begin,
   Iterator     end)
{
   BOOST_STATIC_ASSERT (sizeof (uint32_t) == 4);
   if (std::distance (begin, end) < 4)
   {
      return std::make_pair (begin, false);
   }

   Iterator pos = begin;
   std::advance (pos, 4);

   uint32_t size = 
      util::conversion::from_byte_array <uint32_t> (std::string (begin, pos));   

   return std::make_pair (begin, 
                          std::distance (pos, end) >= size);
}

template <typename Callback>
inline /*! static */ void
protocol::parse_command (
   tcp_connection &                     connection,
   boost::shared_ptr <Callback>         callback)
{
   BOOST_STATIC_ASSERT (sizeof (uint32_t) == 4);
   char bytes_raw[4];

   PAXOS_ASSERT (connection.read_buffer ().sgetn (bytes_raw, 4) == 4);
   std::string bytes_string (bytes_raw, 4);

   uint32_t bytes = util::conversion::from_byte_array <uint32_t> (bytes_string);
   char byte_array[bytes];
   PAXOS_ASSERT (connection.read_buffer ().sgetn (byte_array, bytes) == bytes);

   pb::command command = pb::adapter::from_string (std::string (byte_array, 
                                                                bytes));
   (*callback) (command);
}



}; }; };
