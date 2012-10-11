/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_PROTOCOL_COMMAND_HPP
#define LIBPAXOS_CPP_DETAIL_PROTOCOL_COMMAND_HPP

#include <boost/function.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp> //! For std::pair
#include <boost/serialization/vector.hpp>

#include "quorum/server.hpp"

#include "../error.hpp"

namespace paxos { namespace detail { 

/*!
  \brief Describes command that is exchanged between servers
 */
class command
{
public:

   friend class boost::serialization::access;

   enum type
   {
      type_invalid = -1,

      //! Sent by a host to initiate a handshake
      type_handshake_start = 1,
      
      //! Sent as a response to a handshake request
      type_handshake_response = 2,

      //! Sent when a node in the quorum claims leadership
      type_leader_claim = 3,

      //! Sent when a node acknowledges a new leader
      type_leader_claim_ack = 4,

      //! Sent when a node rejects a new leader
      type_leader_claim_reject = 5,


      //! Sent by a client to the leader when it wants to initiate a new request
      type_request_initiate = 6,

      //! Sent by a leader to all the followers when they must all prepare a request
      type_request_prepare = 7,

      //! Sent by all followers as a response to the prepare command
      type_request_promise = 8,

      //! Sent by followers to leader if an inconsistency in the proposal ids has been detected
      type_request_fail = 9,

      //! Sent by leader to all followers after they have sent a promise
      type_request_accept = 10,

      //! Sent by followers to leader after they have accepted and processed a request
      type_request_accepted = 11,






      //! Sent back to client when an error has occured. This will mean that error_code is also set
      type_request_error = 12
   };


public:

   /*!
     \brief Default constructor
    */
   command ();

   static command
   from_string (
      std::string const &               string);

   static std::string
   to_string (
      command const &                   command);


   void
   set_id (
      uint64_t  id);

   uint64_t
   id () const;

   void
   set_response_id (
      uint64_t  id);

   uint64_t
   response_id () const;

   void
   set_type (
      enum type                         type);

   enum type
   type () const;

   void
   set_error_code (
      enum paxos::error_code            error_code);

   enum paxos::error_code
   error_code () const;

   void
   set_host_id (
      boost::uuids::uuid const &        id);

   boost::uuids::uuid
   host_id () const;

   void
   set_host_endpoint (
      boost::asio::ip::tcp::endpoint const &    endpoint);

   boost::asio::ip::tcp::endpoint
   host_endpoint () const;

   void
   set_host_state (
      enum quorum::server::state state);

   enum quorum::server::state
   host_state () const;


   void
   set_live_servers (
      std::vector <boost::asio::ip::tcp::endpoint> const &      endpoints);

   std::vector <boost::asio::ip::tcp::endpoint>
   live_servers () const;

   void
   set_proposal_id (
      uint64_t                  proposal_id);

   uint64_t
   proposal_id () const;

   void
   set_workload (
      std::string const &       byte_array);

   std::string const &
   workload () const;

private:

   template <class Archive>
   void serialize (
      Archive &                 ar,
      unsigned int const        version);
   
private:

   uint64_t                                             id_;
   uint64_t                                             response_id_;

   enum type                                            type_;
   enum paxos::error_code                               error_code_;

   std::string                                          host_id_;
   std::string                                          host_address_;
   uint16_t                                             host_port_;
   enum quorum::server::state                           host_state_;

   std::vector <std::pair <std::string, uint16_t> >     live_servers_;

   uint64_t                                             proposal_id_;
   std::string                                          workload_;
};

}; };

#include "command.inl"

#endif //! LIBPAXOS_CPP_DETAIL_TCP_CONNECTION_COMMAND_HPP
