/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_QUORUM_HPP
#define LIBPAXOS_CPP_QUORUM_HPP

#include "detail/quorum.hpp"

namespace paxos {
class server;
};

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
private:
   friend class paxos::server;

public:

   /*!
     \brief Adds a new server to the quorum with a default 'non_participant' state
    */
   void
   add (
      boost::asio::ip::address const &  address,
      uint16_t                          port);

private:

   detail::quorum       quorum_;
};
};

#endif //! LIBPAXOS_CPP_QUORUM_HPP
