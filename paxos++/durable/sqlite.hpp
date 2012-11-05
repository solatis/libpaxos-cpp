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
  \note Requires the sqlite development headers to be installed, and libpaxos-cpp to be configured 
        with --enable-sqlite support

   This class provides a durable storage backend based on sqlite. It opens a specific database
   file, handles input/output to that file and maintains a durable state. When a paxos::server 
   is restarted using a previously used database file, it continues from the previous state.

   \par Thread Safety
   \e Distinct \e objects: Safe, as long as different database files are used\n
   \e Shared \e objects: Unsafe\n

   \par Examples

   Set up a paxos::server that uses a sqlite database file called "db.sqlite". 

   \code{.cpp}

   paxos::configuration configuration;
   configuration.set_durable_storage (new paxos::durable::sqlite ("db.sqlite"));
   paxos::server server ("127.0.0.1", 1337,
                         [] (int64_t proposal_id, std::string const & input) -> std::string
                         {
                             return input;
                         },
                         configuration);

   \endcode
 */
class sqlite : public storage
{
public:

   /*!
     \brief Constructor
     \param filename    Location where sqlite database is stored
    */
   sqlite (
      std::string const &       filename);

   /*!
     \brief Destructor
    */
   virtual ~sqlite ();

public:

   virtual std::map <int64_t, std::string>
   retrieve (
      int64_t                                                   proposal_id);

   virtual int64_t
   highest_proposal_id ();

   virtual int64_t
   lowest_proposal_id ();

protected:

   virtual void
   store (
      int64_t                   proposal_id,
      std::string const &       byte_array);

   virtual void
   remove (
      int64_t                   proposal_id);


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
