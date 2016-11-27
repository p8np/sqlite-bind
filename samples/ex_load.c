/* ---------------------------------------------------------------------------
** sqlite3-bind: SQLite C API - Parameter binding helper for SQLite.
** ---------------------------------------------------------------------------
** Copyright (c) 2016 by Payton Bissell, payton.bissell@gmail.com
** ---------------------------------------------------------------------------
** This example demonstrats using the sqlite3-bind function to bind parameters
** for an insert. It creates/fills a table with records from
** "realestate.csv" included with the sqlite3-bind source code.
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

static const char *tbl = "drop table if exists re_trans;"
"create table re_trans"
"( re_trans_id integer primary key,"
"  street      text not null default '',"
"  city        text not null default '',"
"  zip         text not null default '',"
"  state       text not null default '',"
"  beds        integer not null default 0,"
"  baths       integer not null default 0,"
"  sqft        integer not null default 0,"
"  type        text not null default '',"
"  price       integer not null default 0,"
"  latitude    double not null default '0.0',"
"  longitude   double not null default '0.0'"
");";

/* ---------------------------------------------------------------------------
** Close the open DB.
*/
static int i_close(const char *msg, sqlite3 *db) 
{ if (msg) printf("Load Error: %s\n", msg); 
  if (db) sqlite3_close(db); 
  return 0; 
}

/* ---------------------------------------------------------------------------
** Create DB and fill from realestate.csv
*/
int main()
{ char row[1024], *street, *city, *zip, *state, *beds, *baths, *sqft, *type, *price, *lat, *lng;

  // there is no need to convert the parsed data into native types (int/double) since SQLite will
  // handle text input correctly, I do it here only to expand the example.
  int rowcnt=0, ibeds, ibaths, isqft, iprice;
  double dlat, dlng;
  
  // open the input file for reading
  FILE *f = fopen("realestate.csv", "rt");
  if (f==NULL) return 0;
  if (fgets(row, 1024, f)==NULL) return 0; // read header and discard.

  // open and setup the database. 
  sqlite3 *db=NULL;
  if (sqlite3_open("test.db", &db)!=SQLITE_OK) return 0;
  char *errmsg=NULL; 
  if (sqlite3_exec(db, tbl, NULL, NULL, &errmsg)!=SQLITE_OK) return 0; // build table for data
  
  // read and parse all the rows in the file
  while (fgets(row, 1024, f)!=NULL)
  { char *end=row; 
    street=end;   while ((*end)&&(*end!=',')) end++; if (*end==0) return i_close("p1", db); *end=0;
    city  =++end; while ((*end)&&(*end!=',')) end++; if (*end==0) return i_close("p2", db); *end=0;
    zip   =++end; while ((*end)&&(*end!=',')) end++; if (*end==0) return i_close("p3", db); *end=0;
    state =++end; while ((*end)&&(*end!=',')) end++; if (*end==0) return i_close("p4", db); *end=0;
    beds  =++end; while ((*end)&&(*end!=',')) end++; if (*end==0) return i_close("p5", db); *end=0; ibeds=atoi(beds);
    baths =++end; while ((*end)&&(*end!=',')) end++; if (*end==0) return i_close("p6", db); *end=0; ibaths=atoi(baths);
    sqft  =++end; while ((*end)&&(*end!=',')) end++; if (*end==0) return i_close("p7", db); *end=0; isqft=atoi(sqft);
    type  =++end; while ((*end)&&(*end!=',')) end++; if (*end==0) return i_close("p8", db); *end=0; 
    price =++end; while ((*end)&&(*end!=',')) end++; if (*end==0) return i_close("p9", db); *end=0; iprice=atoi(price);
    lat   =++end; while ((*end)&&(*end!=',')) end++; if (*end==0) return i_close("pA", db); *end=0; dlat=atof(lat);
    lng   =++end; while ((*end)&&(*end!=',')&&(*end!='\n')&&(*end!='\r')) end++; *end=0;            dlng=atof(lng); 

    // declaring the static SQL for binding here for clarity...
    const char *ins_sql = "insert into re_trans (street,city,zip,state,beds,baths,sqft,type,price,latitude,longitude)"
                          "values (?,?,?,?,?,?,?,?,?,?,?)";

    // Add each row to the database using the sqlite3-bind function
    int r=sqlite3_bind_exec(db, ins_sql, 
        NULL, NULL, // no callback/arg because no results.
        SQLITE_BIND_TEXT(street), 
        SQLITE_BIND_TEXT(city), 
        SQLITE_BIND_TEXT(zip), 
        SQLITE_BIND_TEXT(state), 
        SQLITE_BIND_INT(ibeds), 
        SQLITE_BIND_INT(ibaths), 
        SQLITE_BIND_INT(isqft), 
        SQLITE_BIND_TEXT(type), 
        SQLITE_BIND_INT(iprice), 
        SQLITE_BIND_DOUBLE(dlat), 
        SQLITE_BIND_DOUBLE(dlng), 
        SQLITE_BIND_END); // END OF STACK marker
        
    // check for errors from the API    
    if (r!=SQLITE_OK) 
    { printf("Insert Error: code=%d message=%s\n", r, sqlite3_bind_errmsg(db));
      return i_close(NULL, db);
    } 
    
    rowcnt++; // if we made it here, it was all good... 
  } /* ROWS IN FILE */
  
  // Close up things and exit
  fclose(f); 
  printf("Done loading %d rows.\n", rowcnt);  
  return i_close(NULL, db);
} 

/* EOF */
