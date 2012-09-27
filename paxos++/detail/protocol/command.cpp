#include <sstream>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include "command.hpp"

namespace paxos { namespace detail { namespace protocol {


/*! static */ std::string
command::to_string (
   command const &      command)
{
   std::stringstream               value;
   boost::archive::binary_oarchive oa (value);
   oa << command;
   return value.str ();
}


/*! static */ command
command::from_string (
   std::string const &  string)
{
   command ret;

   std::stringstream value (string);
   boost::archive::binary_iarchive ia (value);
   ia >> ret;

   return ret;
}

}; }; };
