#include <sstream>

#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/string_generator.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "util/debug.hpp"
#include "command.hpp"

namespace paxos { namespace detail { 


/*! static */ std::string
command::to_string (
   command const &      command)
{
   std::stringstream               value;
   boost::archive::text_oarchive oa (value);
   oa << command;

   return value.str ();
}


/*! static */ command
command::from_string (
   std::string const &  string)
{
   command ret;

   std::stringstream value (string);
   boost::archive::text_iarchive ia (value);
   ia >> ret;

   return ret;
}


void
command::set_host_id (
   boost::uuids::uuid const &        id)
{
   std::stringstream str;
   str << id;

   host_id_ = str.str ();
}

boost::uuids::uuid
command::host_id () const
{
   boost::uuids::string_generator gen;
   return gen (host_id_);
}

void
command::set_host_endpoint (
   boost::asio::ip::tcp::endpoint const &       endpoint)
{
   host_address_ = endpoint.address ().to_string ();
   host_port_    = endpoint.port ();
}


boost::asio::ip::tcp::endpoint
command::host_endpoint () const
{
   return boost::asio::ip::tcp::endpoint (boost::asio::ip::address::from_string (host_address_), 
                                          host_port_);
}

void
command::add_proposed_workload (
   int64_t              proposal_id,
   std::string const &  byte_array)
{
   PAXOS_ASSERT (proposed_workload_.find (proposal_id) == proposed_workload_.end ());

   proposed_workload_[proposal_id] = byte_array;
}




}; };
