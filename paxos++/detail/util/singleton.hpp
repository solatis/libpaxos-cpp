/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_UTIL_SINGLETON_HPP
#define LIBPAXOS_CPP_DETAIL_UTIL_SINGLETON_HPP

namespace paxos { namespace detail { namespace util {

/*!
  \brief Defines helper singleton class
 */
template <class T>
class singleton
{
public:

   static T &
   instance ();

private:
   
};

}; }; };

#include "singleton.inl"

#endif //! LIBPAXOS_CPP_DETAIL_UTIL_SINGLETON_HPP
