/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_PARSER_HPP
#define LIBPAXOS_CPP_DETAIL_PARSER_HPP

#include <boost/shared_array.hpp>

#include "error.hpp"
#include "tcp_connection_fwd.hpp"
#include "command.hpp"

namespace paxos { namespace detail {

/*!
  \brief Interface for reading commands from tcp stream and dispatching to the correct handler
 */
class parser
{
public:

   typedef boost::function <void (boost::optional <enum error_code>, command const &)>  callback_function;

   /*!
     \brief Writes command to connection
     \param connection  Connection to write command to
     \param command     Command to serialize and write
    */
   static void
   write_command (
      tcp_connection_ptr        connection,
      command const &           command);


   /*!
     \brief Reads single command from input stream and dispatches to callback function 
     \param connection  Connection to read from
     \param callback    Callback function object
    */
   static void
   read_command (
      tcp_connection_ptr        connection,
      callback_function         callback);


private:

   static void
   read_command_parse_size (
      tcp_connection_ptr                connection,
      boost::system::error_code const & error,
      size_t                            bytes_transferred,
      boost::shared_array <char>        buffer,
      callback_function                 callback);

   static void
   read_command_parse_command (
      tcp_connection_ptr                connection,
      boost::system::error_code const & error,
      size_t                            bytes_transferred,
      boost::shared_array <char>        buffer,
      callback_function                 callback);


};

}; };

#endif //! LIBPAXOS_CPP_DETAIL_PARSER_HPP
