/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_STRATEGY_FACTORY_HPP
#define LIBPAXOS_CPP_DETAIL_STRATEGY_FACTORY_HPP

namespace paxos { namespace detail { namespace strategy {
class strategy;
}; }; };

namespace paxos { namespace detail { namespace strategy {

/*!
  \brief Abstract base class for factory which creates paxos strategies
 */
class factory
{
public:

   virtual ~factory ();

   virtual strategy *
   create () const = 0;

private:

};

} }; };

#include "factory.inl"

#endif  //! LIBPAXOS_CPP_DETAIL_STRATEGY_FACTORY_HPP
