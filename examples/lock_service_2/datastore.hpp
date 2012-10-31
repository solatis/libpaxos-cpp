#ifndef DATASTORE_HPP
#define DATASTORE_HPP

#include <stdint.h>
#include <string>

#include <sqlite3.h>

class datastore
{
public:

   datastore ();

   ~datastore ();

   int64_t
   most_recent_proposal_id ();

   bool
   has_lock (
      std::string const &       lock_id);

   void
   acquire_lock (
      int64_t                   proposal_id,
      std::string const &       lock_id);

   void
   release_lock (
      int64_t                   proposal_id,
      std::string const &       lock_id);

private:

   void
   create_tables ();

   bool
   has_table ();

private:

   sqlite3 *    db_;
};

#endif
