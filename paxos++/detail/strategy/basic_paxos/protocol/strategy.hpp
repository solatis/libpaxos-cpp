/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_STRATEGY_BASIC_PAXOS_PROTOCOL_STRATEGY_HPP
#define LIBPAXOS_CPP_DETAIL_STRATEGY_BASIC_PAXOS_PROTOCOL_STRATEGY_HPP

#include <boost/asio/ip/tcp.hpp>

#include "../../../error.hpp"
#include "../../strategy.hpp"

namespace paxos { namespace durable {
class storage;
}; };

namespace paxos { namespace detail { namespace strategy { namespace basic_paxos { namespace protocol {

/*!
  \brief Defines protocol used when a leader must propagate a request throughout the quorum
 */
class strategy : public detail::strategy::strategy
{
private:
   enum response
   {
      response_none,
      response_ack,
      response_reject
   };

   struct state
   {
      std::map <boost::asio::ip::tcp::endpoint, enum response>                  accepted;
      std::map <boost::asio::ip::tcp::endpoint, std::string>                    responses;
      std::map <boost::asio::ip::tcp::endpoint, enum detail::error_code>        error_codes;
      std::map <boost::asio::ip::tcp::endpoint, detail::tcp_connection_ptr>     connections;
      queue_guard_type                                                          queue_guard;
   };
   
public:

   strategy (
      durable::storage &        storage);

   /*!
     \brief Received by leader from client that initiates a request
    */
   virtual void
   initiate (      
      tcp_connection_ptr                        client_connection,
      detail::command const &                   command,
      detail::quorum::server_view &             quorum,
      detail::paxos_context &                   global_state,
      queue_guard_type                          queue_guard);


   /*!
     \brief Received by follower when leader wants to prepare a request
    */
   virtual void
   prepare (      
      tcp_connection_ptr                        leader_connection,
      detail::command const &                   command,
      detail::quorum::server_view &             quorum,
      detail::paxos_context &                   global_state);


   /*!
     \brief Received by follower when leader wants to process a request

     This request is received by a follower when a leader has received an 'ACK' for a certain
     proposal id from all followers, and is now telling all followers to actually process a 
     command.
    */
   virtual void
   accept (      
      tcp_connection_ptr                        leader_connection,
      detail::command const &                   command,
      detail::quorum::server_view &             quorum,
      detail::paxos_context &                   global_state);

protected:

   /*!
     \brief Sends a 'prepare' to a specific server
    */
   virtual void
   send_prepare (
      tcp_connection_ptr                        client_connection,
      detail::command const &                   client_command,
      boost::asio::ip::tcp::endpoint const &    follower_endpoint,
      tcp_connection_ptr                        follower_connection,
      detail::quorum::server_view &             quorum,
      detail::paxos_context &                   global_state,
      std::string const &                       byte_array,
      boost::shared_ptr <struct state>          state);


   /*!
     \brief Received by leader as a response to a 'prepare' command
    */
   virtual void
   receive_promise (
      boost::optional <enum detail::error_code> error,
      tcp_connection_ptr                        client_connection,
      detail::command                           client_command,
      boost::asio::ip::tcp::endpoint const &    follower_endpoint,
      tcp_connection_ptr                        follower_connection,
      detail::quorum::server_view &             quorum,
      detail::paxos_context &                   global_state,
      std::string                               byte_array,
      detail::command const &                   command,
      boost::shared_ptr <struct state>          state);

   /*!
     \brief Sends a 'accept' from leader to a specific follower
    */
   virtual void
   send_accept (
      tcp_connection_ptr                        client_connection,
      detail::command const &                   client_command,
      boost::asio::ip::tcp::endpoint const &    follower_endpoint,
      tcp_connection_ptr                        follower_connection,
      detail::quorum::server_view &             quorum,
      detail::paxos_context &                   global_state,
      std::string const &                       byte_array,
      boost::shared_ptr <struct state>          state);


   /*!
     \brief Received by leader as a response to a 'accept' command
    */
   virtual void
   receive_accepted (
      boost::optional <enum detail::error_code> error,
      tcp_connection_ptr                        client_connection,
      detail::command                           client_command,
      boost::asio::ip::tcp::endpoint const &    follower_endpoint,
      detail::quorum::server_view &             quorum,
      detail::command const &                   command,
      boost::shared_ptr <struct state>          state);

   /*!
     \brief Sends error command back to client
    */
   virtual void
   handle_error (
      enum detail::error_code   error,
      quorum::server_view const &    quorum,
      tcp_connection_ptr        client_connection);


   /*!
     \brief Amends a command with information about our local host
    */
   virtual void
   add_local_host_information (
      quorum::server_view const &       quorum,
      detail::command &                 output);

   /*!
     \brief Processes most recent information from a remote host into quorum

     This essentially reads the information added with add_local_host_information () from
     a remote host and stores it inside our quorum.
    */
   virtual void
   process_remote_host_information (
      detail::command const &   command,
      quorum::server_view &     output);


   /*!
     \brief Looks up the state's current proposal id
    */
   virtual int64_t
   proposal_id ();


private:

   durable::storage &   storage_;

};

}; }; }; }; };

#endif //! LIBPAXOS_CPP_DETAIL_STRATEGY_BASIC_PAXOS_PROTOCOL_STRATEGY_HPP
