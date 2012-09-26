#include <iterator>

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

   PAXOS_DEBUG ("got data on match_command, distance = " << std::distance (begin, end));

   return std::pair <Iterator, bool> (begin,
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
   char bytes_raw[4];
   PAXOS_ASSERT (connection.read_buffer ().sgetn (bytes_raw, 4) == 4);
   std::string bytes_string (bytes_raw, 4);
   PAXOS_ASSERT (bytes_string.size () == 4);

   uint32_t bytes = util::conversion::from_byte_array <uint32_t> (bytes_string);
   char byte_array[bytes];
   PAXOS_ASSERT (connection.read_buffer ().sgetn (byte_array, bytes) == bytes);

   pb::command command = pb::adapter::from_string (std::string (byte_array, 
                                                                bytes));

   PAXOS_DEBUG ("has command!");
   callback (command);
   PAXOS_DEBUG ("called back!");
}



template <typename Iterator>
inline /*! static */ bool
protocol::read_command_byte_array (
   Iterator             begin,
   Iterator             end,
   std::string &        output)
{
   PAXOS_DEBUG ("reading command as byte array, bytes = " << (end - begin));
   PAXOS_DEBUG ("reading command as byte array, bytes = " << (end - begin));

   if (std::distance (begin, end) < 4)
   {
      PAXOS_DEBUG ("distance < 4");
      return false;
   }

   Iterator pos = begin;
   std::advance (pos, 4);

   uint32_t size = 
      util::conversion::from_byte_array <uint32_t> (std::string (begin, pos));
   

   PAXOS_DEBUG ("command size = " << size);

   if (std::distance (pos, end) < size)
   {
      return false;
   }

   output.assign (pos, end);
   PAXOS_ASSERT (output.size () == size);
   PAXOS_DEBUG ("output.size () = " << output.size ());

   return true;
}




}; }; };
