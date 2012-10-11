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
command::set_live_servers (
   std::vector <boost::asio::ip::tcp::endpoint> const & endpoints)
{
   for (boost::asio::ip::tcp::endpoint const & endpoint : endpoints)
   {
      live_servers_.push_back (
         std::pair <std::string, uint16_t> (endpoint.address ().to_string (),
                                            endpoint.port ()));
   }
}

std::vector <boost::asio::ip::tcp::endpoint>
command::live_servers () const
{
   std::vector <boost::asio::ip::tcp::endpoint> live_servers;

   for (std::pair <std::string, uint16_t> const & server : live_servers_)
   {
      live_servers.push_back (
         boost::asio::ip::tcp::endpoint (boost::asio::ip::address::from_string (server.first), 
                                         server.second));
   }

   return live_servers;
}




}; };
