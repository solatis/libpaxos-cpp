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
#include <boost/serialization/map.hpp>
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
      type_invalid,


      //! Sent by a client to the leader when it wants to initiate a new request
      type_request_initiate,

      //! Sent by a leader to all the followers when they must all prepare a request
      type_request_prepare,

      //! Sent by all followers as a response to the prepare command
      type_request_promise,

      //! Sent by followers to leader if an inconsistency in the proposal ids has been detected
      type_request_fail,

      //! Sent by leader to all followers after they have sent a promise
      type_request_accept,

      //! Sent by followers to leader after they have accepted and processed a request
      type_request_accepted,


      //! Sent back to client when an error has occured. This will mean that error_code is also set
      type_request_error
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

   enum type                                            type_;
   enum paxos::error_code                               error_code_;

   std::string                                          host_id_;
   std::string                                          host_address_;
   uint16_t                                             host_port_;

   uint64_t                                             proposal_id_;
   std::string                                          workload_;
};

}; };

#include "command.inl"

#endif //! LIBPAXOS_CPP_DETAIL_TCP_CONNECTION_COMMAND_HPP
