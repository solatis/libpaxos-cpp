/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_QUORUM_SERVER_HPP
#define LIBPAXOS_CPP_DETAIL_QUORUM_SERVER_HPP

#include <boost/uuid/uuid.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

#include <boost/optional.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "../tcp_connection_fwd.hpp"

namespace paxos { namespace detail { namespace quorum {

/*!
  \brief Represents a server within a quorum
 */
class server
{
public:

   server (
      boost::asio::io_service &                 io_service,
      boost::asio::ip::tcp::endpoint const &    endpoint);

   /*!
     \brief Destructor
    */
   ~server ();

   /*!
     \brief Returns endpoint this server listens at
    */
   boost::asio::ip::tcp::endpoint const &
   endpoint () const;

   /*!
     \brief Access to the unique identifier of the server
    */
   boost::uuids::uuid const &
   id () const;

   /*!
     \brief Adjusts unique identifier of the server
    */
   void
   set_id (
      boost::uuids::uuid const &        id);

   /*!
     \brief Returns true if we have a uuid
    */
   bool
   has_id () const;

   /*!
     \brief Resets uinique identifier of the server
    */
   void
   reset_id ();


   /*!
     \brief Adjusts the highest proposal id currently known by this server
    */
   void
   set_highest_proposal_id (
      int64_t                   proposal_id);

   /*!
     \brief The highest proposal id currently known by this server
    */
   int64_t
   highest_proposal_id () const;

   /*!
     \brief Establishes connection with remote host
    */
   void
   establish_connection ();

   
   /*!
     \brief Returns true if control_connection_ and broadcast_connection_ are set
    */
   bool
   has_connection () const;

   /*!
     \brief Resets the connection to a nullptr
    */
   void
   reset_connection ();

   /*!
     \brief Access to the underlying connection
     \pre has_connection () == true
    */
   detail::tcp_connection_ptr
   connection ();

private:

   boost::asio::io_service &                            io_service_;

   boost::asio::ip::tcp::endpoint                       endpoint_;
   boost::uuids::uuid                                   id_;

   int64_t                                              highest_proposal_id_;

   boost::posix_time::ptime                             most_recent_connection_attempt_;
   boost::optional <detail::tcp_connection_ptr>         connection_;

};

}; }; };

#endif //! LIBPAXOS_CPP_DETAIL_QUORUM_SERVER_HPP
