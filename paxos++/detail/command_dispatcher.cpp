#include "util/debug.hpp"

#include "../exception/exception.hpp"

#include "quorum/protocol/handshake.hpp"
#include "quorum/protocol/announce_leadership.hpp"
#include "strategies/basic_paxos/protocol/request.hpp"

#include "tcp_connection.hpp"
#include "parser.hpp"
#include "command_dispatcher.hpp"

namespace paxos { namespace detail {

command_dispatcher::command_dispatcher (
   tcp_connection &     connection)
   : connection_ (connection),
     next_command_id_ (0)
{
}



/*! static */ void
command_dispatcher::dispatch_stateless_command (
   tcp_connection_ptr           connection,
   detail::command const &      command,
   detail::quorum::quorum &     quorum,
   detail::paxos_state &        state)
{

   switch (command.type ())
   {
         case command::type_handshake_start:
            quorum::protocol::handshake::step2 (connection,
                                                command,
                                                quorum);
            break;

         case command::type_leader_claim:
            quorum::protocol::announce_leadership::step2 (connection,
                                                          command,
                                                          quorum,
                                                          state);
            break;


         case command::type_request_initiate:
            strategies::basic_paxos::protocol::request::step1 (connection,
                                                               command,
                                                               quorum,
                                                               state);
            break;

         case command::type_request_prepare:
            strategies::basic_paxos::protocol::request::step3 (connection,
                                                               command,
                                                               quorum,
                                                               state);
            break;

         case command::type_request_accept:
            strategies::basic_paxos::protocol::request::step6 (connection,
                                                               command,
                                                               quorum,
                                                               state);
            break;

         default:
            /*!
              This means an unexpected command was received!
             */
            PAXOS_THROW (paxos::exception::protocol_error ());
   };
};


void
command_dispatcher::write (
   detail::command &    command)
{
   boost::mutex::scoped_lock lock (mutex_);
   write_locked (command);
}

void
command_dispatcher::write_locked (
   detail::command &    command)
{
   ++next_command_id_;

   command.set_id (next_command_id_);

   parser::write_command (connection_,
                          command);
}

void
command_dispatcher::write (
   detail::command const &      input_command,
   detail::command &            output_command)
{
   boost::mutex::scoped_lock lock (mutex_);
   write_locked (input_command,
                 output_command);
}

void
command_dispatcher::write_locked (
   detail::command const &      input_command,
   detail::command &            output_command)
{
   PAXOS_ASSERT (input_command.id () > 0);

   ++next_command_id_;

   output_command.set_id (next_command_id_);
   output_command.set_response_id (input_command.id ());

   parser::write_command (connection_,
                          output_command);

}   

void
command_dispatcher::read (
   detail::command const &      command,
   callback                     callback)
{
   boost::mutex::scoped_lock lock (mutex_);
   read_locked (command,
                callback);
}

void
command_dispatcher::read_locked (
   detail::command const &      command,
   callback                     callback)
{
   PAXOS_ASSERT (state_.find (command.id ()) == state_.end ());

   state_.insert (
      std::make_pair (command.id (),
                      callback));
}

void
command_dispatcher::read_loop ()
{
   read_loop ([](command const & command)
   {
      PAXOS_UNREACHABLE ();
   });
}


void
command_dispatcher::read_loop (
   callback     stateless_command_callback)
{

   detail::parser::read_command (connection_,
                                 [this,
                                  stateless_command_callback] (command const &     command)
                                 {
                                    //! Ensure the command is called with the proper callback handler
                                    this->dispatch (command,
                                                    stateless_command_callback);

                                    //! Enters recursion
                                    this->read_loop (stateless_command_callback);
                                 });
}

void
command_dispatcher::dispatch (
   detail::command const &      command,
   callback                     stateless_command_callback)
{
   callback c = lookup_callback (command,
                                 stateless_command_callback);

   c (command);
}

command_dispatcher::callback
command_dispatcher::lookup_callback (
   detail::command const &      command,
   callback                     stateless_command_callback)
{
   boost::mutex::scoped_lock lock (mutex_);
   return lookup_callback_locked (command,
                                  stateless_command_callback);
}

command_dispatcher::callback
command_dispatcher::lookup_callback_locked (
   detail::command const &      command,
   callback                     stateless_command_callback)
{
   if (command.response_id () == 0)
   {
      return stateless_command_callback;
   }
   else
   {
      auto pos = state_.find (command.response_id ());      
      PAXOS_ASSERT (pos != state_.end ());
   
      callback c = pos->second;
      state_.erase (pos);

      return c;
   }
}

}; };
