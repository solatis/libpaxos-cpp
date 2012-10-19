/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_QUORUM_QUORUM_HPP
#define LIBPAXOS_CPP_DETAIL_QUORUM_QUORUM_HPP

#include <map>
#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/deadline_timer.hpp>

#include "../../configuration.hpp"
#include "server.hpp"

namespace boost { namespace uuids {
struct uuid;
}; };

namespace paxos { namespace detail { namespace quorum {

/*!
  \brief Represents the quorum and provides interface to utility funcitons
 */
class quorum : private boost::noncopyable
{
public:   

   /*!
     \brief Constructor for clients
     \param io_service  References the OS'es underlying I/O services

     This constructor assumes we are not part of the quorum. 
    */
   quorum (
      boost::asio::io_service &                 io_service,
      paxos::configuration const &              configuration);

   /*
     \brief Constructor for servers
     \param io_service  References the OS'es underlying I/O services
     \param endpoint    Our endpoint so we can identify ourselves within the quorum

     This constructor assumes we are part of the quorum. This will automatically make
     sure our endpoint is added to the quorum.
    */
   quorum (
      boost::asio::io_service &                 io_service,
      boost::asio::ip::tcp::endpoint const &    endpoint,
      paxos::configuration const &              configuration);

   /*!
     \brief Returns our endpoint
    */
   boost::asio::ip::tcp::endpoint const &
   our_endpoint () const;

   /*!
     \brief Adds new server to quorum
    */
   void
   add (
      boost::asio::ip::tcp::endpoint const &    endpoint);

   /*!
     \brief Retrieves server from quorum
     \returns Returns reference to server
    */
   detail::quorum::server &
   lookup_server (
      boost::asio::ip::tcp::endpoint const &    endpoint);


   /*!
     \brief Retrieves server from quorum
     \returns Returns reference to server
    */
   detail::quorum::server const &
   lookup_server (
      boost::asio::ip::tcp::endpoint const &    endpoint) const;


   /*!
     \brief Returns endpoint of server that should be leader
     \param allow_unknown       Allow hosts that have an unknown uuid to be selected as leader
    */
   boost::optional <boost::asio::ip::tcp::endpoint>
   who_is_our_leader (
      bool allow_unknown        = false);

   /*!
     \brief Returns vector of endpoints of servers that are currently alive
     
     Note that this function also tries to re-establish a connection with any dead
     servers if the most recent attempt occured long enough ago.
    */
   std::vector <boost::asio::ip::tcp::endpoint>
   live_servers ();

   /*!
     \brief Called when connection trouble has occured with a specific host

     This resets the connection of a host.
    */
   void
   connection_died (
      boost::asio::ip::tcp::endpoint const &    endpoint);

private:

private:

   boost::asio::io_service &                                            io_service_;
   paxos::configuration                                                 configuration_;

   boost::optional <boost::asio::ip::tcp::endpoint>                     our_endpoint_;

   std::map <boost::asio::ip::tcp::endpoint, detail::quorum::server>    servers_;

};

}; }; };

#endif //! LIBPAXOS_CPP_DETAIL_QUORUM_QUORUM_HPP
