/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_STRATEGY_BASIC_PAXOS_PROTOCOL_STRATEGY_HPP
#define LIBPAXOS_CPP_DETAIL_STRATEGY_BASIC_PAXOS_PROTOCOL_STRATEGY_HPP

#include <boost/asio/ip/tcp.hpp>

#include "../../strategy.hpp"

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
      std::map <boost::asio::ip::tcp::endpoint, detail::tcp_connection_ptr>     connections;
      queue_guard_type                                                          queue_guard;
   };
   
public:

   /*!
     \brief Received by leader from client that initiates a request
    */
   virtual void
   initiate (      
      tcp_connection_ptr                        client_connection,
      detail::command const &                   command,
      detail::quorum::quorum &                  quorum,
      detail::paxos_context &                   global_state,
      queue_guard_type                          queue_guard) const;


   /*!
     \brief Received by follower when leader wants to prepare a request
    */
   virtual void
   prepare (      
      tcp_connection_ptr                        leader_connection,
      detail::command const &                   command,
      detail::quorum::quorum &                  quorum,
      detail::paxos_context &                   global_state) const;


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
      detail::quorum::quorum &                  quorum,
      detail::paxos_context &                   global_state) const;

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
      detail::quorum::quorum &                  quorum,
      detail::paxos_context &                   global_state,
      std::string const &                       byte_array,
      boost::shared_ptr <struct state>          state) const;


   /*!
     \brief Received by leader as a response to a 'prepare' command
    */
   virtual void
   receive_promise (
      boost::optional <enum paxos::error_code>  error,
      tcp_connection_ptr                        client_connection,
      detail::command                           client_command,
      boost::asio::ip::tcp::endpoint const &    follower_endpoint,
      tcp_connection_ptr                        follower_connection,
      detail::quorum::quorum &                  quorum,
      detail::paxos_context &                   global_state,
      std::string                               byte_array,
      detail::command const &                   command,
      boost::shared_ptr <struct state>          state) const;

   /*!
     \brief Sends a 'accept' from leader to a specific follower
    */
   virtual void
   send_accept (
      tcp_connection_ptr                        client_connection,
      detail::command const &                   client_command,
      boost::asio::ip::tcp::endpoint const &    follower_endpoint,
      tcp_connection_ptr                        follower_connection,
      detail::quorum::quorum &                  quorum,
      detail::paxos_context &                   global_state,
      std::string const &                       byte_array,
      boost::shared_ptr <struct state>          state) const;


   /*!
     \brief Received by leader as a response to a 'accept' command
    */
   virtual void
   receive_accepted (
      boost::optional <enum paxos::error_code>  error,
      tcp_connection_ptr                        client_connection,
      detail::command                           client_command,
      boost::asio::ip::tcp::endpoint const &    follower_endpoint,
      detail::quorum::quorum &                  quorum,
      detail::command const &                   command,
      boost::shared_ptr <struct state>          state) const;

   /*!
     \brief Sends error command back to client
    */
   virtual void
   handle_error (
      enum paxos::error_code    error,
      quorum::quorum const &    quorum,
      tcp_connection_ptr        client_connection) const;


   /*!
     \brief Amends a command with information about our local host
    */
   virtual void
   add_local_host_information (
      quorum::quorum const &    quorum,
      detail::command &         output) const;

   /*!
     \brief Processes most recent information from a remote host into quorum

     This essentially reads the information added with add_local_host_information () from
     a remote host and stores it inside our quorum.
    */
   virtual void
   process_remote_host_information (
      detail::command const &   command,
      quorum::quorum &          output) const;


private:

};

}; }; }; }; };

#endif //! LIBPAXOS_CPP_DETAIL_STRATEGY_BASIC_PAXOS_PROTOCOL_STRATEGY_HPP
