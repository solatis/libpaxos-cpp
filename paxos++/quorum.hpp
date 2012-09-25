/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_QUORUM_HPP
#define LIBPAXOS_CPP_DETAIL_QUORUM_HPP

#include <set>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/optional.hpp>
#include <boost/uuid/uuid.hpp>

#include "detail/remote_server.hpp"

namespace paxos { 

/*!
  \brief Quorum of servers in the pool

  The quorum maintains a state of all the servers inside the pool. The assumption is
  that the quorum is fixed and is equal for all servers in the pool: that is, it is up
  to the application to ensure that this configuration is the same on all hosts.

  Please be aware that the quorum does not necessarily describe /all/ connections to all
  other servers. It only describes the long-lived connections required by the Paxos protocol, 
  and not any short-lived connections required by leader election, for example.

  Note that there are several options here:
   * we are a leader, in which case we (should) have connections to all other followers;
   * we are a follower, in which case we only have a connection to our leader;
   * we are a client, in which we can have several connections to anyone.

  If these conditions are not met, an assertion is thrown.
 */
class quorum
{
public:

   /*!
     \brief Denotes which server we are
     \param address     Our local IP address
     \param port        Port we're listening at
     \param uuid        Our unique identification number
     \pre The server is part of the quorum
    */
   void
   we_are (
      boost::asio::ip::address const &  address,
      uint16_t                          port,
      boost::uuids::uuid const &        uuid);

   /*!
     \brief Access to the endpoint we represent
    */
   boost::asio::ip::tcp::endpoint const &
   self () const;

   /*!
     \brief Adds a new server to the quorum with a default 'dead' state
    */
   void
   add (
      boost::asio::ip::address const &  address,
      uint16_t                          port);

   
   template <class OutputIterator> OutputIterator
   get_endpoints (
      OutputIterator    result) const;

private:
   
   typedef std::map <boost::asio::ip::tcp::endpoint, 
                     detail::remote_server> map_type;

   boost::asio::ip::tcp::endpoint       self_;
   boost::uuids::uuid                   uuid_;

   map_type                             servers_;

};

};

#include "quorum.inl"

#endif //! LIBPAXOS_CPP_DETAIL_QUORUM_HPP
