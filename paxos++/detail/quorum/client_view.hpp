/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_QUORUM_CLIENT_VIEW_HPP
#define LIBPAXOS_CPP_DETAIL_QUORUM_CLIENT_VIEW_HPP

#include "view.hpp"

namespace paxos { namespace detail { namespace quorum {

/*!
  \brief Represents the view clients have on the quorum

  Since the view servers have on the quorum is different from the view clients have, this
  class defines client-specific functionality.
 */
class client_view : public view
{
public:   

   /*!
     \brief Constructor for clients
     \param io_service  References the OS'es underlying I/O services

     This constructor assumes we are not part of the quorum. 
    */
   client_view (
      boost::asio::io_service &                 io_service);

   /*!
     \brief Selects an endpoint who we try to select as leader
    */
   boost::optional <boost::asio::ip::tcp::endpoint>
   select_leader ();

   /*!
     \brief Advances to the next leader

     Calling this function provides this view with the feedback that the last server
     returned by select_leader () is *not* the leader. This will advance the internal 
     pointer to the next server.
    */
   void
   advance_leader ();

private:

   void
   advance_leader (
      std::vector <boost::asio::ip::tcp::endpoint> const &       live_servers);


private:

   boost::optional <boost::asio::ip::tcp::endpoint>     next_leader_;


};

}; }; };

#endif //! LIBPAXOS_CPP_DETAIL_QUORUM_CLIENT_VIEW_HPP
