#include "../../util/debug.hpp"
#include "adapter.hpp"

namespace paxos { namespace detail { namespace protocol { namespace pb {


/*! static */ std::string
adapter::to_string (
   command const &      command)
{
   std::string output;
   command.SerializeToString (&output);
   return output;
}

/*! static */ command
adapter::from_string (
   std::string const &  string)
{
   command command;

   PAXOS_ASSERT (command.ParseFromString (string) == true);

   return command;
}


}; }; }; };
