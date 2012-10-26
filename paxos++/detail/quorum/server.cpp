#include <boost/uuid/nil_generator.hpp>

#include "../util/debug.hpp"

#include "../tcp_connection.hpp"
#include "server.hpp"

namespace paxos { namespace detail { namespace quorum {

server::server (
   boost::asio::io_service &                    io_service,
   boost::asio::ip::tcp::endpoint const &       endpoint)
   : io_service_ (io_service),
     endpoint_ (endpoint),
     highest_proposal_id_ (-1)
{
   this->reset_id ();
}

server::~server ()
{
}

boost::asio::ip::tcp::endpoint const &
server::endpoint () const
{
   return endpoint_;
}

boost::uuids::uuid const &
server::id () const
{
   return id_;
}

void
server::set_id (
   boost::uuids::uuid const &   id)
{
   id_ = id;
}

bool
server::has_id () const
{
   return id_.is_nil () == false;
}

void
server::reset_id ()
{
   id_ = boost::uuids::nil_generator ()();
}

void
server::set_highest_proposal_id (
   int64_t     proposal_id)
{
   highest_proposal_id_ = proposal_id;
}

int64_t
server::highest_proposal_id () const
{
   return highest_proposal_id_;
}


detail::tcp_connection_ptr
server::connection ()
{
   return *connection_;
}

bool
server::has_connection () const
{
   return 
      connection_.is_initialized () == true;
}

void
server::reset_connection ()
{
   connection_ = boost::none;
}

void
server::establish_connection ()
{
   PAXOS_ASSERT (has_connection () == false);

   if (most_recent_connection_attempt_.is_not_a_date_time () == false)
   {
      boost::posix_time::time_duration diff = 
         boost::posix_time::second_clock::local_time () - most_recent_connection_attempt_;

      if (diff <= boost::posix_time::seconds (3))
      {
         return;
      }
   }

   most_recent_connection_attempt_ = boost::posix_time::second_clock::local_time ();

   PAXOS_INFO ("attempting to establish connection with " << endpoint_);
   tcp_connection_ptr connection = tcp_connection::create (io_service_);
   connection->socket ().async_connect (
      endpoint_,
      [this, 
       connection] 
      (boost::system::error_code const & error)
      {
         if (error)
         {
            PAXOS_WARN ("Unable to connect to remote host: " << error.message ());
            PAXOS_ASSERT (this->has_connection () == false);
         }
         else
         {
            this->connection_ = connection;
         }
      });
}


}; }; };
