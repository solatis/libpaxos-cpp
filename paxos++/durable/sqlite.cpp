
#include <boost/lexical_cast.hpp>

#include "../exception/exception.hpp"
#include "../detail/util/debug.hpp"

#include "sqlite.hpp"

namespace paxos { namespace durable {

sqlite::sqlite (
   std::string const &  filename)
   : filename_ (filename),
     db_ (NULL)
{
   PAXOS_ASSERT_EQ (sqlite3_open (filename.c_str (), &db_), SQLITE_OK);
   PAXOS_ASSERT (db_ != NULL);

   this->create_table ();
}

/*! virtual */ sqlite::~sqlite ()
{
   PAXOS_ASSERT_EQ (sqlite3_close (db_), SQLITE_OK);
}

/*! virtual */ void
sqlite::store (
   int64_t              proposal_id,
   std::string const &  byte_array)
{
   std::string query =
      "INSERT INTO "
      "  history ("
      "    id, "
      "    byte_array) "
      "VALUES (" 
      "  " + boost::lexical_cast <std::string> (proposal_id) + ", "
      "  '" + byte_array + "')";

   /*!
     This is a sanity check which could really be removed in production, but let's just
     keep it here for now. It ensures important things are sane, and we do not have any
     "gaps" in our history.
    */
   PAXOS_ASSERT_EQ (highest_proposal_id (), (proposal_id - 1));

   PAXOS_DEBUG ("executing query: " << query);

   char * error;

   if (sqlite3_exec (db_,
                     query.c_str (),
                     NULL,
                     NULL,
                     &error) != SQLITE_OK)
   {
      PAXOS_FATAL ("An error occured while storing data: " << sqlite3_errmsg (db_));
      PAXOS_THROW (paxos::exception::storage_error ());
   }
}

/*! virtual */ std::map <int64_t, std::string>
sqlite::retrieve (
   int64_t      proposal_id)
{
   std::map <int64_t, std::string> result;

   std::string query = 
      "SELECT "
      "  id, "
      "  byte_array "
      "FROM "
      "  history "
      "WHERE "
      "  id > " + boost::lexical_cast <std::string> (proposal_id);


   PAXOS_DEBUG ("executing query: " << query);

   sqlite3_stmt * prepared_statement = 0;
   PAXOS_ASSERT_EQ (sqlite3_prepare (db_,
                                     query.c_str (),
                                     query.length (),
                                     &prepared_statement,
                                     NULL), SQLITE_OK);

   while (sqlite3_step (prepared_statement) == SQLITE_ROW)
   {
      PAXOS_ASSERT_EQ (sqlite3_column_count (prepared_statement), 2);

      int64_t id              = sqlite3_column_int64 (prepared_statement, 0);
      void const * byte_array = sqlite3_column_blob  (prepared_statement, 1);

      result[id] = std::string (static_cast <char const *> (byte_array));
   }
   

   PAXOS_ASSERT_EQ (sqlite3_finalize (prepared_statement), SQLITE_OK);


   return result;
}

/*! virtual */ int64_t
sqlite::highest_proposal_id ()
{
   std::string query = 
      "SELECT "
      "  MAX (id) "
      "FROM "
      "  history";

   sqlite3_stmt * prepared_statement = 0;
   PAXOS_ASSERT_EQ (sqlite3_prepare (db_,
                                     query.c_str (),
                                     query.length (),
                                     &prepared_statement,
                                     NULL), SQLITE_OK);

   int64_t result = -1;

   while (sqlite3_step (prepared_statement) == SQLITE_ROW)
   {
      PAXOS_ASSERT_EQ (result, -1);
      PAXOS_ASSERT_EQ (sqlite3_column_count (prepared_statement), 1);

      result = sqlite3_column_int64 (prepared_statement, 0);
   }

   PAXOS_ASSERT_EQ (sqlite3_finalize (prepared_statement), SQLITE_OK);

   return result;
}

void
sqlite::create_table ()
{
   std::string query = 
      "CREATE TABLE history (id INTEGER PRIMARY KEY ASC, byte_array BLOB)";

   char * error;

   if (sqlite3_exec (db_,
                     query.c_str (),
                     NULL,
                     NULL,
                     &error) != SQLITE_OK)
   {
      PAXOS_WARN ("An error occured while creating table: " << error);
   }
}


}; };
