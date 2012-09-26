#include "../../util/debug.hpp"
#include "adapter.hpp"

namespace paxos { namespace detail { namespace protocol { namespace pb {


/*! static */ std::string
adapter::to_string (
   command const &      command)
{
   std::string output;
   command.SerializeToString (&output);

   PAXOS_DEBUG ("got command = " << output.size () << " bytes");

   return output;
}

/*! static */ command
adapter::from_string (
   std::string const &  string)
{
   command command;

   PAXOS_DEBUG ("got command = " << string.size () << " bytes");

   PAXOS_ASSERT (command.ParseFromString (string) == true);

   PAXOS_DEBUG ("parsed command!");

   return command;
}


}; }; }; };
