#include <sstream>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include "command.hpp"

namespace paxos { namespace detail { 


/*! static */ std::string
command::to_string (
   command const &      command)
{
   std::stringstream               value;
   boost::archive::binary_oarchive oa (value);
   oa << command;
   return value.str ();
}


/*! static */ command
command::from_string (
   std::string const &  string)
{
   command ret;

   std::stringstream value (string);
   boost::archive::binary_iarchive ia (value);
   ia >> ret;

   return ret;
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



}; };
