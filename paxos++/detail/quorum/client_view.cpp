#include "../util/debug.hpp"

#include "client_view.hpp"

namespace paxos { namespace detail { namespace quorum {


client_view::client_view (
   boost::asio::io_service &    io_service)
   : view::view (io_service)
{
}

boost::optional <boost::asio::ip::tcp::endpoint>
client_view::select_leader ()
{
   std::vector <boost::asio::ip::tcp::endpoint> servers = this->live_servers ();

   if (servers.empty () == true)
   {
      return boost::none;
   }

   if (next_leader_.is_initialized () == false)
   {
      next_leader_ = *servers.begin ();
   }


   auto pos = std::lower_bound (servers.begin (),
                                servers.end (),
                                *next_leader_);

   if (*pos == *next_leader_)
   {
      /*!
        This means that the server we marked as the next candidate is actually live!
       */
      return *pos;
   }
   

   /*!
     The server was not found in the list of live servers, let's advance to the next
     server.
    */
   advance_leader (servers);

   /*!
     Note that this returns the actual optional object; this is exactly what we want,
     if there are no servers live this will return no leader.
    */
   return next_leader_;

}


void
client_view::advance_leader ()
{
   advance_leader (this->live_servers ());
}

void
client_view::advance_leader (
   std::vector <boost::asio::ip::tcp::endpoint> const & live_servers)
{
   if (live_servers.empty () == true)
   {
      next_leader_ = boost::none;
      return;
   }

   for (auto const & i : live_servers)
   {
      PAXOS_DEBUG (i);
   }

   /*!
     If the server was is found, this returns an iterator to the server, otherwise
     it returns the iterator *before* the last server.
    */
   std::vector <boost::asio::ip::tcp::endpoint>::const_iterator pos;

   if (next_leader_.is_initialized () == false)
   {
      pos = live_servers.begin ();
   }
   else
   {
      pos = std::find (live_servers.begin (),
                       live_servers.end (),
                       *next_leader_);

      if (pos == live_servers.end ())
      {
         /*!
           Server we determined to be the next leader was not in live_servers array anymore, 
           the best thing to do is to just return to the beginning of the array.
          */
         pos = live_servers.begin ();
      }      
   }

   PAXOS_ASSERT (pos != live_servers.end ());

   ++pos;
   if (pos == live_servers.end ())
   {
      /*!
        Uh-oh, we're at the end of the vector. Let's start at the beginning again.
       */
      pos = live_servers.begin ();
   }

   PAXOS_ASSERT (pos != live_servers.end ());

   /*!
     The next time we call this function, we want to immediately select the server
     we return here.
    */
   next_leader_ = *pos;
}




}; }; };
