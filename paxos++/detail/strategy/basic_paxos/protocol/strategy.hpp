/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_CLIENT_PROTOCOL_REQUEST_HPP
#define LIBPAXOS_CPP_DETAIL_CLIENT_PROTOCOL_REQUEST_HPP

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
      boost::asio::ip::tcp::endpoint const &    leader_endpoint,
      boost::asio::ip::tcp::endpoint const &    follower_endpoint,
      tcp_connection_ptr                        follower_connection,
      detail::paxos_context &                   global_state,
      std::string const &                       byte_array,
      boost::shared_ptr <struct state>          state) const;


   /*!
     \brief Received by leader as a response to a 'prepare' command
    */
   virtual void
   receive_promise (
      tcp_connection_ptr                        client_connection,
      detail::command                           client_command,
      boost::asio::ip::tcp::endpoint const &    leader_endpoint,
      boost::asio::ip::tcp::endpoint const &    follower_endpoint,
      tcp_connection_ptr                        follower_connection,
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
      boost::asio::ip::tcp::endpoint const &    leader_endpoint,
      boost::asio::ip::tcp::endpoint const &    follower_endpoint,
      tcp_connection_ptr                        follower_connection,
      detail::paxos_context &                   global_state,
      std::string const &                       byte_array,
      boost::shared_ptr <struct state>          state) const;


   /*!
     \brief Received by leader as a response to a 'accept' command
    */
   virtual void
   receive_accepted (
      tcp_connection_ptr                        client_connection,
      detail::command                           client_command,
      boost::asio::ip::tcp::endpoint const &    follower_endpoint,
      detail::command const &                   command,
      boost::shared_ptr <struct state>          state) const;

private:   

};

}; }; }; }; };

#endif //! LIBPAXOS_CPP_DETAIL_CLIENT_PROTOCOL_REQUEST_HPP
