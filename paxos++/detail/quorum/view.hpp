/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_QUORUM_VIEW_HPP
#define LIBPAXOS_CPP_DETAIL_QUORUM_VIEW_HPP

#include <map>
#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "../../configuration.hpp"
#include "server.hpp"

namespace paxos { namespace detail { namespace quorum {

/*!
  \brief Base class which represents a view on the quorum
 */
class view : private boost::noncopyable
{
public:   

   /*!
     \brief Constructor
     \param io_service  References the OS'es underlying I/O services
    */
   view (
      boost::asio::io_service &                 io_service);

   /*!
     \brief Adds new server to view
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
     \brief Returns sorted vector of endpoints of servers that are currently alive
     
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


   /*!
     \brief Returns the highest proposal id of the server that is lagging behind the most
    */
   int64_t
   lowest_proposal_id () const;

protected:

   std::map <boost::asio::ip::tcp::endpoint, detail::quorum::server>    servers_;

private:

   boost::asio::io_service &                                            io_service_;


};

}; }; };

#endif //! LIBPAXOS_CPP_DETAIL_QUORUM_VIEW_HPP
