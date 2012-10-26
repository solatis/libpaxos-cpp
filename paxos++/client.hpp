/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_CLIENT_HPP
#define LIBPAXOS_CPP_CLIENT_HPP

#include <future>
#include <initializer_list>

#include "detail/io_thread.hpp"
#include "detail/quorum/client_view.hpp"
#include "detail/request_queue/queue.hpp"
#include "detail/client/protocol/request.hpp"

#include "configuration.hpp"

namespace paxos {

/*!
  \brief Provides client-side interface to Paxos quorum

  The client class provides the core functionality for clients that want to
  send messages to all paxos::server instances in a paxos quorum. It maintains an 
  internal state of all remote servers inside the quorum, knows which server is the 
  leader, and automatically handles a leader failover in case that occurs.

  \par Thread Safety
  \e Distinct \e objects: Safe\n
  \e Shared \e objects: Unsafe\n

  \par Examples

  Let the client maintain its own thread of control, send a message to a quorum of
  servers and wait for the result to arrive:

  \code{.cpp}

  paxos::client client;
  client.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});

  std::future <std::string> future = client.send ("foo");
  std::string result = future.get ();

  \endcode


  Setup a client that uses a different paxos::configuration than the default:

  \code{.cpp}

  paxos::configuration configuration;
  configuration.set_timeout (45000);

  paxos::client client (configuration);
  client.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});

  std::future <std::string> future = client.send ("foo");
  std::string result = future.get ();

  \endcode


  Setup multiple clients that makes use of an external io_service object, and as such 
  can share the same thread:

  \code{.cpp}
  
  boost::asio::io_service io_service;

  // This prevents the io_service from running out of work
  boost::asio::io_service::work work (io_service);

  // Launch new thread in the background which calls io_service.run ()
  boost::thread io_thread (boost::bind (&boost::asio::io_service::run,
                                        &io_service));

  // Note that we share the same io_service here, and thus both clients share the same
  // worker thread.
  paxos::client client1 (io_service);
  paxos::client client2 (io_service);
  client1.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});
  client2.add ({{"127.0.0.1", 1337}, {"127.0.0.1", 1338}, {"127.0.0.1", 1339}});

  std::future <std::string> future1 = client1.send ("foo");
  std::future <std::string> future2 = client2.send ("foo");
  assert (future1.get () == future2.get ());
  
  \endcode  
  
  
 */
class client
{
public:

   /*!
     \brief Opens client

     This constructor launches its own background thread with i/o context
    */
   client ();

   /*!
     \brief Opens client
     \param io_service  Boost.Asio io_service object, which represents the link to the OS'es i/o services
   */
   client (
      boost::asio::io_service &         io_service);

   /*!
     \brief Destructor
     
     Gracefully closes the background io thread, if any.
    */
   ~client ();

   /*!
     \brief Add server to quorum registered within the client
     \param server      IPv4 address, IPv6 address or hostname of server to connect to
     \param port        Port of server to connect to
    */
   void
   add (
      std::string const &       server,
      uint16_t                  port);

   /*!
     \brief Add list of servers to quorum registered within the client
     \param servers List of pairs of server/ports to connect to
    */
   void
   add (
      std::initializer_list <std::pair <std::string, uint16_t> > const &        servers);

   /*!
     \brief Asynchronously send data to entire quorum and return result in a future
     \param byte_array  Data to sent. Binary-safe.
     \param retries     Amount of times to retry failed operations
     \returns Future to the result

     \note If \e retries is larger than 1, automatically retries the operation. This will
           result in future.get () taking longer to get a result, but it can greatly reduce
           the amount of exceptions exposed to the code making use of this client; situations
           like leadership failover can cause temporary errors to be thrown, which usually
           resolve themselves within a second.

     \note While this function by itself does not throw any exceptions due to its 
           asynchronous nature, errors can occur while processing the request. If this
           is the case, the exception will be stores inside the future, and any call
           to future.get () will throw this exception.

   */
   std::future <std::string>
   send (
      std::string const &       byte_array,
      uint16_t                  retries = 10) 
      throw ();

private:

   void
   do_request (
      boost::shared_ptr <std::promise <std::string> >   promise,
      std::string const &                               byte_array,
      uint16_t                                          retries);

private:


   detail::io_thread                                                    io_thread_;
   boost::asio::io_service &                                            io_service_;
   detail::quorum::client_view                                          quorum_;
   detail::request_queue::queue <detail::client::protocol::request>     request_queue_;

};

}

#endif  //! LIBPAXOS_CPP_CLIENT_HPP
