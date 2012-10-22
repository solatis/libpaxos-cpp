/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_IO_THREAD_HPP
#define LIBPAXOS_CPP_DETAIL_IO_THREAD_HPP

#include <boost/thread.hpp>

#include <boost/asio/io_service.hpp>

namespace paxos { namespace detail {

/*!
  \brief Helper class which provides its own I/O context and worker thread
 */
class io_thread
{
public:

   /*!
     \brief Default constructor
    */
   io_thread ();

   boost::asio::io_service &
   io_service ();

   /*!
     \brief Launches background thread which continues
    */
   void
   launch ();

   /*!
     \brief Thread control function
    */
   void
   run ();

   /*!
     \brief Blocks until thread_ has exited
    */
   void
   join ();

   /*!
     \brief Stops thread, if any
    */
   void
   stop ();

private:

   boost::thread                        thread_;
   boost::asio::io_service              io_service_;
   boost::asio::io_service::work        work_;
};

} };

#endif  //! LIBPAXOS_CPP_DETAIL_IO_THREAD_HPP
