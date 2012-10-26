/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_STRATEGY_BASIC_PAXOS_FACTORY_HPP
#define LIBPAXOS_CPP_DETAIL_STRATEGY_BASIC_PAXOS_FACTORY_HPP

#include "../factory.hpp"

namespace paxos { 
class configuration;
}; 

namespace paxos { namespace detail { namespace strategy { namespace basic_paxos {

/*!
  \brief Abstract base class for factory which creates paxos strategies
 */
class factory : public detail::strategy::factory
{
public:

   factory (
      paxos::configuration &    configuration);

   virtual strategy *
   create () const;

private:

   paxos::configuration &       configuration_;

};

} }; }; };


#endif  //! LIBPAXOS_CPP_DETAIL_STRATEGY_BASIC_PAXOS_FACTORY_HPP
