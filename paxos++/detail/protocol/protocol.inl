namespace paxos { namespace detail { namespace protocol {


template <typename Callback>
inline /*! static */ void
protocol::read_command (
   tcp_connection &  input,
   Callback          output)
{
   typedef boost::asio::buffers_iterator<
      boost::asio::streambuf::const_buffers_type> iterator_type;

   boost::asio::streambuf buf;
   boost::asio::async_read_until (input.socket (), 
                                  buf, 
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
   /*! :TODO: IMPLEMENT */ 

   return std::make_pair (begin, true);
}

template <typename Callback>
inline /*! static */ void
protocol::parse_command (
   tcp_connection &                     connection,
   boost::system::error_code const &    error,
   size_t                               bytes_transferred,
   Callback                             callback)
{
   /*! :TODO: IMPLEMENT */ 

   pb::command command;

   callback (command);
}




}; }; };
