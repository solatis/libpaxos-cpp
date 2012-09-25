#include <sstream>

namespace paxos { namespace detail { namespace protocol {

template <typename Callback>
inline /*! static */ void
protocol::read_command (
   tcp_connection &  input,
   Callback          output)
{
   typedef boost::asio::buffers_iterator <boost::asio::streambuf::const_buffers_type> iterator_type;

   boost::asio::async_read_until (input.socket (), 
                                  input.read_buffer (), 
                                  match_command <iterator_type>,
                                  boost::bind (&protocol::parse_command <Callback>,
                                               boost::ref (input),
                                               boost::asio::placeholders::error,
                                               boost::asio::placeholders::bytes_transferred,
                                               boost::ref (output)));
}

template <typename Iterator>
inline /*! static */ std::pair <Iterator, bool>
protocol::match_command (
   Iterator     begin,
   Iterator     end)
{
   std::string tmp;

   return std::make_pair (begin,
                          read_command_byte_array (begin, 
                                                   end,
                                                   tmp));
}

template <typename Callback>
inline /*! static */ void
protocol::parse_command (
   tcp_connection &                     connection,
   boost::system::error_code const &    error,
   size_t                               bytes_transferred,
   Callback                             callback)
{
   char const * data = 
      boost::asio::buffer_cast <char const *> (connection.read_buffer ().data ());

   std::string byte_array;
   read_command_byte_array (data, 
                            data + bytes_transferred,
                            byte_array);
   
   callback (pb::adapter::from_string (byte_array));
}



template <typename Iterator>
inline /*! static */ bool
protocol::read_command_byte_array (
   Iterator             begin,
   Iterator             end,
   std::string &        output)
{
   if (std::distance (begin, end) < 4)
   {
      return false;
   }

   Iterator pos = begin;
   pos += 4;

   uint32_t size = 
      util::conversion::from_byte_array <uint32_t> (std::string (begin, pos));
   
   if (std::distance (pos, end) < size)
   {
      return false;
   }

   output = std::string (pos, end);
   return true;
}




}; }; };
