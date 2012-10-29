#include <iostream>
#include <assert.h>

#include <boost/lexical_cast.hpp>

#include "datastore.hpp"

datastore::datastore ()
{
   assert (sqlite3_open ("locks.sqlite", &db_) == SQLITE_OK);
   assert (db_ != NULL);

   if (this->has_table () == false)
   {
      this->create_tables ();
   }
}


datastore::~datastore ()
{
   assert (sqlite3_close (db_) == SQLITE_OK);
}

int64_t
datastore::most_recent_proposal_id ()
{
   std::string query = 
      "SELECT "
      "  id "
      "FROM "
      "  proposal_id ";

   sqlite3_stmt * prepared_statement = 0;
   assert (sqlite3_prepare (db_,
                            query.c_str (),
                            query.length (),
                            &prepared_statement,
                            NULL) == SQLITE_OK);


   int64_t proposal_id = -1;

   while (sqlite3_step (prepared_statement) == SQLITE_ROW)
   {
      assert (proposal_id == -1);
      assert (sqlite3_column_count (prepared_statement) == 1);

      proposal_id = sqlite3_column_int64 (prepared_statement, 0);
   }

   assert (sqlite3_finalize (prepared_statement) == SQLITE_OK);   

   return proposal_id;
}

bool
datastore::has_lock (
   std::string const &  lock_id)
{
   std::string query = 
      "SELECT "
      "  COUNT (*) "
      "FROM "
      "  locks "
      "WHERE "
      "  id = '" + lock_id + "'";

   sqlite3_stmt * prepared_statement = 0;
   assert (sqlite3_prepare (db_,
                            query.c_str (),
                            query.length (),
                            &prepared_statement,
                            NULL) == SQLITE_OK);

   int64_t result = 0;

   while (sqlite3_step (prepared_statement) == SQLITE_ROW)
   {
      assert (result == 0);
      assert (sqlite3_column_count (prepared_statement) == 1);

      result = sqlite3_column_int64 (prepared_statement, 0);
   }

   assert (0 <= result && result <= 1);
   assert (sqlite3_finalize (prepared_statement) == SQLITE_OK);

   std::cout << "lock_id = '" << lock_id << "', result = " << result << std::endl;

   return result == 1;   
}

void
datastore::acquire_lock (
   int64_t              proposal_id,
   std::string const &  lock_id)
{

   std::cout << "acquiring lock_id = '" << lock_id << "'" << std::endl;

   assert (
      sqlite3_exec (db_,
                    "BEGIN",
                    NULL,
                    NULL,
                    NULL) == SQLITE_OK);

   assert (
      sqlite3_exec (db_,
                    std::string ("INSERT INTO locks (id) VALUES ('" + lock_id + "')").c_str (),
                    NULL,
                    NULL,
                    NULL) == SQLITE_OK);

   assert (
      sqlite3_exec (db_,
                    std::string ("UPDATE "
                                 "  proposal_id "
                                 "SET "
                                 "  id = " + boost::lexical_cast <std::string> (proposal_id)).c_str (),
                    NULL,
                    NULL,
                    NULL) == SQLITE_OK);   

   assert (
      sqlite3_exec (db_,
                    "COMMIT",
                    NULL,
                    NULL,
                    NULL) == SQLITE_OK);
   
}


void
datastore::release_lock (
   int64_t              proposal_id,
   std::string const &  lock_id)
{
   std::cout << "releasing lock_id = '" << lock_id << "'" << std::endl;

   assert (
      sqlite3_exec (db_,
                    "BEGIN",
                    NULL,
                    NULL,
                    NULL) == SQLITE_OK);

   assert (
      sqlite3_exec (db_,
                    std::string ("DELETE FROM locks WHERE id = '" + lock_id + "'").c_str (),
                    NULL,
                    NULL,
                    NULL) == SQLITE_OK);

   assert (
      sqlite3_exec (db_,
                    std::string ("UPDATE "
                                 "  proposal_id "
                                 "SET "
                                 "  id = " + boost::lexical_cast <std::string> (proposal_id)).c_str (),
                    NULL,
                    NULL,
                    NULL) == SQLITE_OK);   

   assert (
      sqlite3_exec (db_,
                    "COMMIT",
                    NULL,
                    NULL,
                    NULL) == SQLITE_OK);
   
}


void
datastore::create_tables ()
{
   assert (
      sqlite3_exec (db_,
                    "CREATE TABLE locks ("
                    "  id TEXT PRIMARY KEY ASC) ",
                    NULL,
                    NULL,
                    NULL) == SQLITE_OK);

   assert (
      sqlite3_exec (db_,
                    "CREATE TABLE proposal_id ("
                    "  id INTEGER PRIMARY KEY ASC) ",
                    NULL,
                    NULL,
                    NULL) == SQLITE_OK);

   assert (
      sqlite3_exec (db_,
                    "INSERT INTO proposal_id VALUES (0)",
                    NULL,
                    NULL,
                    NULL) == SQLITE_OK);
}


bool
datastore::has_table ()
{
   std::string query = 
      "SELECT "
      "  COUNT (*) "
      "FROM "
      "  sqlite_master "
      "WHERE "
      "  type = 'table' "
      "  AND (name = 'locks' OR name = 'proposal_id')";

   sqlite3_stmt * prepared_statement = 0;
   assert (sqlite3_prepare (db_,
                            query.c_str (),
                            query.length (),
                            &prepared_statement,
                            NULL) == SQLITE_OK);

   int64_t result = 0;

   while (sqlite3_step (prepared_statement) == SQLITE_ROW)
   {
      assert (result == 0);
      assert (sqlite3_column_count (prepared_statement) == 1);

      result = sqlite3_column_int64 (prepared_statement, 0);
   }

   assert (sqlite3_finalize (prepared_statement) == SQLITE_OK);

   return result == 2;
}
