#C/C++ Simple SQLite Binding Wrapper Functions

A simple wrapper to use SQLite parameter binding in a single exec call. 

##sqlite3_bind_exec:

The *sqlite3_bind_exec* functions follow the *sqlite_exec* API pattern
but support variable arguments that will be bound to '?' markers
in the sql statements.

Variable arguments must use the *SQLITE_BIND_XXX* macros to provide type
information, to cast the argument which notifies the compiler of the type and
ensures the correct size conversion on the stack. They also facilitates
the passing of extra parameters with the argument (such as the size parameter
in the *SQLITE_BIND_BLOB* macro). 

Like *sqlite_exec*, the *sqlite3_bind_exec* functions will process multiple sql 
statements that are separated by ';'. The stack arguments are processed in the
order they occur in the statements (left to right). 

##Pseudo code examples:

```C
sqlite_uint64 pageid = <pageid>;
unsigned char *bitmap = <bitmap>;
char *caption = <caption>;

int ret = sqlite_bind_exec(
  db, 
  "update images set caption=?, bitmap=? where pageid=?", 
  NULL, NULL,  // no callback or callback arg because no results
  SQLITE_BIND_TEXT(caption), 
  SQLITE_BIND_BLOB(bitmap, size), 
  SQLITE_BIND_INT64(page_id), 
  SQLITE_BIND_END);

int ret = sqlite_bind_exec(
  db, 
  "select bitmap, size from images where pageid=? or caption=?", 
  callback, cb_arg, // callback to receive results
  SQLITE_BIND_INT64(page_id), 
  SQLITE_BIND_TEXT(caption), 
  SQLITE_BIND_END);
```

The stack must be terminated with the SQLITE_BIND_END marker or an error
will be returned (after the statement(s) are executed successfully).

There are versions of the exec function that take a va_list to support libraries
that wrap SQLite, or more complicated program structures.
