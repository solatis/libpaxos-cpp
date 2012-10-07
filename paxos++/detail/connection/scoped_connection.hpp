/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DETAIL_CONNECTION_SCOPED_CONNECTION_HPP
#define LIBPAXOS_CPP_DETAIL_CONNECTION_SCOPED_CONNECTION_HPP

#include <boost/shared_ptr.hpp>

#include "tcp_connection.hpp"

namespace paxos { namespace detail { namespace connection {
class pool;
}; }; };

namespace paxos { namespace detail { namespace connection {

/*!
  \brief Automatically acquires and releases connection from pool
 */
class scoped_connection
{
public:

   typedef boost::shared_ptr <scoped_connection>   pointer;   

public:

   /*!
     \brief Acquires connection from pool
    */
   scoped_connection (
      pool &    pool);

   
   /*!
     \brief Releases connection to pool
   */
   ~scoped_connection ();

   /*!
     \brief Creates new scoped connection
    */
   static pointer
   create (
      pool &    pool);

   /*!
     \brief Marks connection ready to be released to the pool
    */
   void
   done ();


   tcp_connection::pointer
   connection ();

private:

   pool &                       pool_;
   tcp_connection::pointer      connection_;
   bool                         done_;

};

}; }; };

#endif //! LIBPAXOS_CPP_DETAIL_CONNECTION_SCOPED_CONNECTION_HPP
