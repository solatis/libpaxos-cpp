/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_PROTOCOL_ELECT_LEADER_HPP
#define LIBPAXOS_CPP_DETAIL_PROTOCOL_ELECT_LEADER_HPP

namespace paxos { 
class quorum;
}; 

namespace paxos { namespace detail { 
class connection_pool;
}; };

namespace paxos { namespace detail { namespace protocol {
class protocol;
}; }; };

namespace paxos { namespace detail { namespace protocol { namespace pb {
class command;
}; }; }; };

namespace paxos { namespace detail { namespace protocol {

/*!
  \brief Attempts to elect a new leader within the quorum
 */
class elect_leader
{
public:

   /*!
     \brief Constructor
     \param protocol    Reference to the protocol we live in
    */
    elect_leader (
       detail::protocol::protocol &     protocol);

   /*!
     \brief Starts new leader election attempt
    */
   void
   start ();

private:

   /*!
     \brief Attempts to connect with all nodes inside the quorum
    */
   void
   step1 ();

   /*!
     \brief Called when a new connection has been established
    */
   void
   step2 (
      tcp_connection &                          connection,
      boost::system::error_code const &         error);

   void
   step3 (
      tcp_connection &                          connection,
      pb::command const &                       command);

private:

   quorum &                     quorum_;
   detail::connection_pool &    connection_pool_;

};

}; }; };

#endif //! LIBPAXOS_CPP_DETAIL_PROTOCOL_ELECT_LEADER_HPP
