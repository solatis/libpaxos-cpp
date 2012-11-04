/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_QUORUM_SERVER_VIEW_HPP
#define LIBPAXOS_CPP_DETAIL_QUORUM_SERVER_VIEW_HPP

#include "../../configuration.hpp"

#include "view.hpp"

namespace paxos { namespace detail { namespace quorum {

/*!
  \brief Represents the view servers have on the quorum

  Since the view servers have on the quorum is different from the view clients have, this
  class defines the server-specific functionality.
 */
class server_view : public view
{
public:   

   /*
     \brief Constructor
     \param io_service    References the OS'es underlying I/O services
     \param endpoint      Our endpoint so we can identify ourselves within the quorum
     \param configuration Our runtime configuration parameters

     This constructor assumes we are part of the quorum. This will automatically make
     sure our endpoint is added to the quorum.
    */
   server_view (
      boost::asio::io_service &                 io_service,
      boost::asio::ip::tcp::endpoint const &    endpoint,
      paxos::configuration const &              configuration);

   /*!
     \brief Returns our endpoint
    */
   boost::asio::ip::tcp::endpoint const &
   our_endpoint () const;

   /*!
     \brief Determines whether a majority of servers is currently alive
     \returns Returns true if the majority of servers is currently alive
    */
   bool
   has_majority ();

   /*!
     \brief Returns endpoint of server that should be leader
    */
   boost::optional <boost::asio::ip::tcp::endpoint>
   who_is_our_leader ();

private:

private:

   double                               majority_factor_;
   boost::asio::ip::tcp::endpoint       our_endpoint_;

};

}; }; };

#endif //! LIBPAXOS_CPP_DETAIL_QUORUM_SERVER_VIEW_HPP
