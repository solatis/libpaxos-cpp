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

#include "quorum/server.hpp"

#include "error.hpp"

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
      enum detail::error_code           error_code);

   enum detail::error_code
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
   set_next_proposal_id (
      int64_t                   proposal_id);

   int64_t
   next_proposal_id () const;

   /*!
     \brief Sets the highest proposal id currently known by a host
    */
   void
   set_highest_proposal_id (
      int64_t                   proposal_id);

   /*!
     \brief The highest proposal id currently known by a host
    */
   int64_t
   highest_proposal_id () const;


   /*!
     \brief Sets the highest proposal id that has been processed by all hosts
    */
   void
   set_lowest_proposal_id (
      int64_t                   proposal_id);

   /*!
     \brief The highest proposal id processed by all hosts
    */
   int64_t
   lowest_proposal_id () const;


   /*!
     \brief Sets a single workload entry

     This is sent from client to leader.
    */
   void
   set_workload (
      std::string const &       byte_array);

   /*!
     \brief Access to single workload entry.
   */
   std::string const &
   workload () const;

   /*!
     \brief Sets multiple workload entries, associated by proposal id
     
     This is sent from leader to follower.
    */
   void
   set_proposed_workload (
      std::map <int64_t, std::string> const &   proposed_workload);

   /*!
     \brief Adds a single proposed workload entry, associated by proposal id
    */
   void
   add_proposed_workload (
      int64_t                   proposal_id,
      std::string const &       byte_array);

   /*!
     \brief Access to multiple workload entries, associated by proposal id
    */
   std::map <int64_t, std::string> const &
   proposed_workload () const;

private:

   template <class Archive>
   void serialize (
      Archive &                 ar,
      unsigned int const        version);
   
private:

   enum type                                            type_;
   enum detail::error_code                              error_code_;

   std::string                                          host_id_;
   std::string                                          host_address_;
   uint16_t                                             host_port_;

   int64_t                                              next_proposal_id_;
   int64_t                                              highest_proposal_id_;
   int64_t                                              lowest_proposal_id_;

   std::string                                          workload_;
   std::map <int64_t, std::string>                      proposed_workload_;
};

}; };

#include "command.inl"

#endif //! LIBPAXOS_CPP_DETAIL_TCP_CONNECTION_COMMAND_HPP
