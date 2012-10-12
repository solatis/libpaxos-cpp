#include <boost/bind.hpp>

#include "io_thread.hpp"


namespace paxos { namespace detail {


io_thread::io_thread ()
   : work_ (io_service_)
{
}

void
io_thread::launch ()
{
   /*!
     \todo Move to std::bind
    */
   thread_ = std::move (
      boost::thread (boost::bind (&boost::asio::io_service::run,
                                  &io_service_)));
}


void
io_thread::stop ()
{
   io_service_.stop ();
}

boost::asio::io_service &
io_thread::io_service ()
{
   return io_service_;
}


}; };
