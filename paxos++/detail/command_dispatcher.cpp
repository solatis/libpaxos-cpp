#include <functional>

#include "util/debug.hpp"

#include "../exception/exception.hpp"

#include "quorum/quorum.hpp"
#include "quorum/protocol/handshake.hpp"
#include "quorum/protocol/announce_leadership.hpp"
#include "strategy/strategy.hpp"

#include "tcp_connection.hpp"
#include "parser.hpp"
#include "paxos_context.hpp"
#include "command_dispatcher.hpp"

namespace paxos { namespace detail {

command_dispatcher::command_dispatcher (
   boost::asio::io_service &                    io_service,
   tcp_connection &                             connection,
   boost::asio::ip::tcp::endpoint const &       endpoint,
   detail::quorum::quorum &                     quorum)
   : io_service_ (io_service),
     connection_ (connection),
     endpoint_ (endpoint),
     quorum_ (quorum),
     next_command_id_ (0)
{
}

/*! static */ void
command_dispatcher::dispatch_stateless_command (
   boost::optional <enum paxos::error_code>     error,
   tcp_connection_ptr                           connection,
   detail::command const &                      command,
   detail::quorum::quorum &                     quorum,
   detail::paxos_context &                      state)
{
   if (error)
   {
      PAXOS_WARN ("An error occured while reading stateless commands: " << paxos::to_string (*error));
      PAXOS_ASSERT (connection->is_open () == false);
      return;
   }

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
                                                          quorum);
            break;


         case command::type_request_initiate:
            state.request_queue ().push (
               {
                  connection,
                  command,
                  quorum,
                  state
               });
            break;

         case command::type_request_prepare:
            state.strategy ().prepare (connection,
                                       command,
                                       quorum,
                                       state);
            break;      

         case command::type_request_accept:
            state.strategy ().accept (connection,
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
   PAXOS_ASSERT_GT (input_command.id (), 0);

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

   boost::shared_ptr <state> state (
      new command_dispatcher::state (io_service_, callback));

   state->timer_.expires_from_now (
      boost::posix_time::milliseconds (3000));
   state->timer_.async_wait (
      std::bind (&command_dispatcher::handle_timeout, 
                 this,
                 std::placeholders::_1,
                 command.id ()));
   

   state_.insert (
      std::make_pair (command.id (),
                      state));
}

void
command_dispatcher::read_loop ()
{
   read_loop ([](boost::optional <enum paxos::error_code> error, command const & command)
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
                                  stateless_command_callback] (
                                     boost::optional <enum paxos::error_code>   error,
                                     command const &                            command)
                                 {
                                    if (error)
                                    {
                                       this->handle_error (*error,
                                                           stateless_command_callback);
                                    }
                                    else
                                    {
                                       //! Ensure the command is called with the proper callback handler
                                       this->dispatch (command,
                                                       stateless_command_callback);

                                       //! Enters recursion
                                       this->read_loop (stateless_command_callback);
                                    }
                                 });
}

void
command_dispatcher::dispatch (
   detail::command const &      command,
   callback                     stateless_command_callback)
{
   boost::optional <callback> c = lookup_callback (command,
                                                   stateless_command_callback);

   if (c)
   {
      (*c) (boost::none, command);
   }
}

boost::optional <command_dispatcher::callback>
command_dispatcher::lookup_callback (
   detail::command const &      command,
   callback                     stateless_command_callback)
{
   boost::mutex::scoped_lock lock (mutex_);
   return lookup_callback_locked (command,
                                  stateless_command_callback);
}

boost::optional <command_dispatcher::callback>
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
      if (pos == state_.end ())
      {
         PAXOS_WARN ("unable to find callback for command " << command.response_id () << ", ignoring response");
         return boost::none;
      }
      else
      {
         pos->second->timer_.cancel ();
         callback c = pos->second->callback_;

         state_.erase (pos);

         return c;
      }
   }
}

void
command_dispatcher::handle_timeout (
   boost::system::error_code const & error,
   uint64_t                          command_id)
{
   if (error)
   {
      /*!
        Timer was cancelled, which is considered normal operation.
       */
      return;
   }


   boost::mutex::scoped_lock lock (mutex_);
   handle_timeout_locked (command_id);
}

void
command_dispatcher::handle_timeout_locked (
   uint64_t                          command_id)
{
   auto pos = state_.find (command_id);
   if (pos == state_.end ())
   {
      PAXOS_WARN ("timeout occured for command not found: " << command_id);
      return;
   }

   callback c = pos->second->callback_;
   state_.erase (pos);

   c (paxos::error_connection_timeout, command ());
}

void
command_dispatcher::handle_error (
   enum paxos::error_code       error,
   callback                     stateless_command_callback)
{
   PAXOS_DEBUG ("handling error: " << paxos::to_string (error));

   boost::mutex::scoped_lock lock (mutex_);
   handle_error_locked (error,
                        stateless_command_callback);   
}

void
command_dispatcher::handle_error_locked (
   enum paxos::error_code       error,
   callback                     stateless_command_callback)
{
   /*!
     What we're going to do when handling errors is extremely crude: we're basically going to
     perform a callback on *all* registered states, and ensure they get a nice & clean "error"
     callback.

     When that's done, we're going to close the connection and let everything die from there.
    */
   for (auto & i : state_)
   {
      i.second->timer_.cancel ();
      i.second->callback_ (error,
                           command ());
   }

   state_.clear ();
   connection_.close ();

   /*!
     We might want to be able to do something within the stateless command callback too when an
     error occurs, so let's call that one here too.
    */
   stateless_command_callback (error,
                               command ());

   /*!
     And last but not least, if this command dispatcher is actually of a follower inside the quorum,
     after an error occured it makes sense to mark this host as dead.
    */
   quorum_.mark_dead (endpoint_);
}

}; };
