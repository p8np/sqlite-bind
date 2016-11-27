/* ---------------------------------------------------------------------------
** sqlite3-bind: SQLite C API - Parameter binding helper for SQLite.
** ---------------------------------------------------------------------------
** Copyright (c) 2016 by Payton Bissell, payton.bissell@gmail.com
** ---------------------------------------------------------------------------
** This example demonstrats using the sqlite3-bind function to bind parameters
** for a select. 
**
** The purpose of this example is to illustrate how the sqlite3-bind function
** can be used as simply as possible. 
**
** 1. It does not imply or even demonstrate good programming practices.
** 2. It is NOT designed for high performance.
** 3. It is not defect free, so use with caution.
** 
** ---------------------------------------------------------------------------
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3-bind.h>

/* ---------------------------------------------------------------------------
** Structure of test.db for example...
**
**  create table re_trans
**  ( re_trans_id integer primary key,
**    street      text not null default '',
**    city        text not null default '',
**    zip         text not null default '',
**    state       text not null default '',
**    beds        integer not null default 0,
**    baths       integer not null default 0,
**    sqft        integer not null default 0,
**    type        text not null default '',
**    price       integer not null default 0,
**    latitude    double not null default '0.0',
**    longitude   double not null default '0.0'
**  );
** ---------------------------------------------------------------------------
*/

/* ---------------------------------------------------------------------------
** Print each result
*/
static int print_result(void *arg, int argc, char **argv, char **cols) 
{ if (argc!=4) return 0; 
  printf("Result - CITY:%s\tPRICE:%s\tSQFT:%s\tBEDS:%s\n", argv[0], argv[1], argv[2], argv[3]);
  return 0;
}

/* ---------------------------------------------------------------------------
** Assumes that the test.db file is present and contains realestate transaction
** data
*/
int main()
{ const char *type="Condo";
  int price=80000;

  // open and setup the database. 
  sqlite3 *db=NULL;
  if (sqlite3_open("test.db", &db)!=SQLITE_OK) return 0;

  // Add each row to the database using the sqlite3-bind function
  int r=sqlite3_bind_exec(db, "select city, price, sqft, beds from re_trans where type=? and price<?",  
      print_result, NULL, SQLITE_BIND_TEXT(type), SQLITE_BIND_INT(price), SQLITE_BIND_END); 
        
  // check for errors from the API    
  if (r!=SQLITE_OK) printf("Query Error: code=%d message=%s\n", r, sqlite3_bind_errmsg(db));
  else printf("Query Completed.\n");
  
  if (db) sqlite3_close(db); 
  return 0;
} 

/* EOF */
