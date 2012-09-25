/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_PROTOCOL_ADAPTER_HPP
#define LIBPAXOS_CPP_DETAIL_PROTOCOL_ADAPTER_HPP

#include "command.pb.h"

namespace paxos { namespace detail { namespace protocol { namespace pb {

/*!
  \brief Interface for translating commands to strings and vice versa
 */
class adapter
{
public:

   static std::string
   to_string (
      command const &           command);

   static command
   from_string (
      std::string const &       string);

private:
};

}; }; }; };

#endif //! LIBPAXOS_CPP_DETAIL_PROTOCOL_ADAPTER_HPP
