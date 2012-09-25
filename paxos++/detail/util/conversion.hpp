/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_UTIL_CONVERSION_HPP
#define LIBPAXOS_CPP_DETAIL_UTIL_CONVERSION_HPP

#include <string>

#include "debug.hpp"

namespace paxos { namespace detail { namespace util {

/*!
  \brief Defines helper functions that convert by arrays to ints and vice versa.
 */
class conversion
{
public:

   /*!
     \brief Converts POD type to byte array (as string)
    */
   template <typename T> static std::string  
   to_byte_array (
      T input);

   /*!
     \brief Converts bytearray (as string) to POD type
     \pre input.size () == sizeof (T)
    */
   template <typename T> static T
   from_byte_array (
      std::string const &  input);   

private:
   
};

}; }; };

#include "conversion.inl"

#endif //! LIBPAXOS_CPP_DETAIL_UTIL_CONVERSION_HPP
