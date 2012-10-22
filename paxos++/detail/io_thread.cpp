#include <functional>

#include "io_thread.hpp"


namespace paxos { namespace detail {


io_thread::io_thread ()
   : work_ (io_service_)
{
}

void
io_thread::launch ()
{
   thread_ = std::move (
      boost::thread (std::bind (&io_thread::run,
                                this)));
}

void
io_thread::run ()
{
   io_service_.run ();
}


void
io_thread::join ()
{
   thread_.join ();
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
