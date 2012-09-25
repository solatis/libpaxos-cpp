/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_PROTOCOL_SESSION_HPP
#define LIBPAXOS_CPP_DETAIL_PROTOCOL_SESSION_HPP

#include <algorithm>
#include <boost/bind.hpp>

#include <boost/ptr_container/ptr_vector.hpp>

namespace paxos { namespace detail { namespace protocol {
class protocol;
}; }; };

namespace paxos { namespace detail { namespace protocol {

/*!
  \brief Maintains the state within a single protocol session

  Multiple similar activities can occur concurrently within the protocol. If this
  is the case, of course, we want those activities to have their own state.

  This session class maintains such a state.  
 */
template <class Protocol>
class session
{
public:

   session (
      detail::protocol::protocol &      protocol);

   /*!
     \brief Creates new session
    */
   Protocol &
   create ();

   /*!
     \brief Removes a session from the pool
     
     This function uses the address of the session to remove it. It asserts if the
     session can not be found.
    */
   void
   remove (
      Protocol const &  session);


   /*!
     \brief Returns the amount of sessions currently active.

     This can be useful when there is a limit of the amount of sessions a certain protocol
     can handle concurrently (for example, there can only be one leader election at the
     same time).
    */
   size_t
   size () const;

private:

   detail::protocol::protocol & protocol_;
   boost::ptr_vector <Protocol> sessions_;
};

}; }; };

#include "session.inl"

#endif //! LIBPAXOS_CPP_DETAIL_TCP_CONNECTION_SESSION_HPP
