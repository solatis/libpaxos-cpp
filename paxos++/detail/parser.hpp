/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_PARSER_HPP
#define LIBPAXOS_CPP_DETAIL_PARSER_HPP

#include <boost/shared_array.hpp>

#include "connection/tcp_connection.hpp"
#include "command.hpp"

namespace paxos { namespace detail {

/*!
  \brief Interface for reading commands from tcp stream and dispatching to the correct handler
 */
class parser
{
public:

   typedef boost::function <void (command const &)>     callback_function;

public:

   /*!
     \brief Writes command to connection
     \param connection  Connection to write command to
     \param command     Command to serialize and write
    */
   static void
   write_command (
      connection::tcp_connection::pointer       connection,
      command const &                           command);


   /*!
     \brief Reads single command from input stream and dispatches to callback function 
     \param connection  Connection to read from
     \param callback    Callback function object
    */
   static void
   read_command (
      connection::tcp_connection::pointer       connection,
      callback_function                         callback);

private:



   static void
   read_command_parse_size (
      connection::tcp_connection::pointer       connection,
      boost::system::error_code const &         error,
      size_t                                    bytes_transferred,
      boost::shared_array <char>                buffer,
      callback_function                         callback);

   static void
   read_command_parse_command (
      connection::tcp_connection::pointer       connection,
      boost::system::error_code const &         error,
      size_t                                    bytes_transferred,
      boost::shared_array <char>                buffer,
      callback_function                         callback);


};

}; };

#endif //! LIBPAXOS_CPP_DETAIL_PARSER_HPP
