/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_COMMAND_DISPATCHER_HPP
#define LIBPAXOS_CPP_DETAIL_COMMAND_DISPATCHER_HPP

#include <stdint.h>
#include <map>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>

#include "tcp_connection_fwd.hpp"

namespace paxos { namespace detail {
class command;
class paxos_context;
}; };

namespace paxos { namespace detail { namespace quorum {
class quorum;
}; }; };

namespace paxos { namespace detail {

/*!
  \brief Ensures that commands are dispatched to the proper handler
 */
class command_dispatcher : private boost::noncopyable
{
public:

   typedef boost::function <void (command const &)>     callback;

public:

   /*!
     \brief Constructor
    */
   command_dispatcher (
      tcp_connection &  connection);

   /*!
     \brief Dispatches stateless command to the appropriate handler
     \param connection  Connection that received this command
     \param command     The command that was received
     \param quorum      Reference to our server's current quorum
     \param state       Keeps track of state information for various paxos strategies
    */
   static void
   dispatch_stateless_command (
      tcp_connection_ptr        connection,
      detail::command const &   command,
      detail::quorum::quorum &  quorum,
      detail::paxos_context &   state);
   

   /*!
     \brief Dispatches request initiate to appropriate handler, or enqueues it
    */
   static void
   dispatch_request_initiate (
      tcp_connection_ptr           connection,
      detail::command const &      command,
      detail::quorum::quorum &     quorum,
      detail::paxos_context &      state);

   /*!
     \brief Writes a command to connection
     \param command     Command that is being sent

     Note that the command that is generated when sending this command is is stored inside 
     the command, which is why we need a writable reference.

     If a response is expected to this command, make sure to call read_command and provide
     this exact same command to
    */
   void
   write (
      detail::command &         command);


   /*!
     \brief Writes a command to connection that is a response to another command
     \param input_command       Command that this is a response to
     \param output_command      Command that is being sent

     Note that the command that is generated when sending this command is is stored inside 
     the command, which is why we need a writable reference.

     If a response is expected to this command, make sure to call read_command and provide
     this exact same command to
    */
   void
   write (
      detail::command const &   input_command,
      detail::command &         output_command);

   /*!
     \brief Reads the next command that is a response to an exact other command
     \param connection  Connection that this command should be read from
     \param command     Command that should be responded to
     \param callback    Callback function to call when response is received
    */
   void
   read (
      detail::command const &   command,
      callback                  callback);



   /*!
     \brief Endless loop that reads and dispatches input commands

     This is the variant of the loop that can be used by clients and servers, which never
     expect an unsollicited command.

     An assertion is thrown if stateless command is in fact called.
   */
   void
   read_loop ();

   /*!
     \brief Endless loop that reads and dispatches input commands
     \param stateless_command_callback  Callback to use for commands that 
                                        can be received without being a response to anything

     This is the variant of the loop that can be used by servers, which expect commands
     that aren't a response to anything.
    */
   void
   read_loop (
      callback  stateless_command_callback);

private:


   /*!
     \brief Dispatches command to the appropriate handler
     \param connection  Connection that received this command
     \param command     The command that was received
    */
   void
   dispatch (
      detail::command const &   command,
      callback                  stateless_command_callback);



   void
   write_locked (
      detail::command &         command);

   void
   write_locked (
      detail::command const &   input_command,
      detail::command &         output_command);

   void
   read_locked (
      detail::command const &   command,
      callback                  callback);

   callback
   lookup_callback (
      detail::command const &   command,
      callback                  stateless_command_callback);


   callback
   lookup_callback_locked (
      detail::command const &   command,
      callback                  stateless_command_callback);


private:

   /*!
     \brief Synchronizes access to private variables
    */
   boost::mutex                         mutex_;

   tcp_connection &                     connection_;

   uint64_t                             next_command_id_;
   std::map <uint64_t, callback>        state_;

};

}; };

#endif //! LIBPAXOS_CPP_DETAIL_COMMAND_DISPATCHER_HPP
