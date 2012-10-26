/*!
  Copyright (c) 2012, Leon Mergen, all rights reserved.
 */

#ifndef LIBPAXOS_CPP_DURABLE_SQLITE_HPP
#define LIBPAXOS_CPP_DURABLE_SQLITE_HPP

#include <sqlite3.h>

#include "storage.hpp"

namespace paxos { namespace durable {

/*!
  \brief Provides durable paxos::server backend based on sqlite 
 */
class sqlite : public storage
{
public:

   /*!
     \brief Constructor
     \param filename    Filename to use to look for database file
    */
   sqlite (
      std::string const &       filename);

   /*!
     \brief Destructor
    */
   virtual ~sqlite ();

public:

   virtual void
   store (
      int64_t                   proposal_id,
      std::string const &       byte_array);

   virtual std::map <int64_t, std::string>
   retrieve (
      int64_t                                                   proposal_id);

   virtual int64_t
   highest_proposal_id ();

private:

   bool
   has_table ();

   void
   create_table ();


private:

   std::string  filename_;
   sqlite3 *    db_;
};

} }

#endif  //! LIBPAXOS_CPP_DURABLE_SQLITE_HPP
