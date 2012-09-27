/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_PROTOCOL_COMMAND_HPP
#define LIBPAXOS_CPP_DETAIL_PROTOCOL_COMMAND_HPP

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_serialize.hpp>
#include <boost/serialization/access.hpp>

namespace paxos { namespace detail { namespace protocol {

/*!
  \brief Describes command that is exchanged between servers
 */
class command
{
public:

   friend class boost::serialization::access;

   enum type
   {
      invalid,
      start_election,
      won_election
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
   set_host_id (
      boost::uuids::uuid const &        id);

   boost::uuids::uuid const &
   host_id () const;


private:

   template <class Archive>
   void serialize (
      Archive &                 ar,
      unsigned int const        version);

   
private:

   enum type            type_;
   boost::uuids::uuid   host_id_;

};

}; }; };

#include "command.inl"

#endif //! LIBPAXOS_CPP_DETAIL_TCP_CONNECTION_COMMAND_HPP
