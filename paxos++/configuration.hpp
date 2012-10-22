/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_CONFIGURATION_HPP
#define LIBPAXOS_CPP_CONFIGURATION_HPP

#include <stdint.h>

#include <boost/shared_ptr.hpp>

namespace paxos { namespace detail { namespace strategy {
class factory;
}; }; };

namespace paxos {

/*!
  \brief Provides runtime configurable parameters to paxos::client and paxos::server
 */
class configuration
{

public:

   /*!
     \brief Default constructor
    */
   configuration ();

   /*!
     \brief Adjusts timeout (in milliseconds) before marking a host as dead

     Default is 3000 (3 seconds)
    */
   void
   set_timeout (
      uint32_t  timeout);

   /*!
     \brief Access to timeout
    */
   uint32_t
   timeout () const;

   /*!
     \brief Adjusts the strategy used for internal paxos protocol
     \note Takes over ownership of \c factory
    */
   void
   set_strategy_factory (
      detail::strategy::factory *       factory);

   /*!
     \brief Access to strategy used for internal paxos protocol
    */
   detail::strategy::factory const &
   strategy_factory () const;

private:

   uint32_t                                             timeout_;

   boost::shared_ptr <detail::strategy::factory>        strategy_factory_;
};

}

#endif  //! LIBPAXOS_CPP_SERVER_HPP
