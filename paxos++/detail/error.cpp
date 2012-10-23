#include "util/debug.hpp"
#include "error.hpp"

namespace paxos { namespace detail {

std::string
to_string (
   enum error_code error_code)
{
   switch (error_code)
   {
         case no_error:
            return "No error";
            break;

         case error_no_leader:
            return "No leader";
            break;

         case error_incorrect_proposal:
            return "Incorrect proposal id";
            break;

         case error_inconsistent_response:
            return "Inconsistent response";
            break;

         case error_connection_close:
            return "Connection closed";
            break;

         case error_no_majority:
            return "No majority";
            break;
   };

   PAXOS_UNREACHABLE ();

}

}; };
