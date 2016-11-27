#C/C++ Simple SQLite Binding Wrapper Functions

A simple wrapper to use SQLite parameter binding in a single exec call. 
Binding simplifies SQL and protects against SQL injection.

##sqlite3_bind_exec:

The `sqlite3_bind_exec` functions follow the `sqlite_exec` API pattern
but support variable arguments that will be bound to '?' markers
in the sql statements.

Variable arguments must use the `SQLITE_BIND_XXX` macros to provide type
information, to cast the argument which notifies the compiler of the type and
ensures the correct size conversion on the stack. They also facilitates
the passing of extra parameters with the argument (such as the size parameter
in the `SQLITE_BIND_BLOB` macro). 

Like `sqlite_exec`, the `sqlite3_bind_exec` functions will process multiple sql 
statements that are separated by **;**. The stack arguments are processed in the
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

The stack must be terminated with the `SQLITE_BIND_END` marker or an error
will be returned (after the statement(s) are executed successfully).

There are versions of the exec function that take a `va_list` to support libraries
that wrap SQLite, or more complicated program structures.

##sqlite3_bind_array:

The `sqlite_bind_array` functions are a convienence for inserting arrays
of data in a single call using the argument binding features of sqlite.
 
The bind_array functions do not return results right now. There does not
not seem to be a need to execute a set of **selects** with an array of data.
**inserts** and **updates** are better suited to this feature. The functions
will return an error if the statement has result columns.

In some RDMS array binding for inserts can be significantly more efficient.
That is not the case with SQLite. These functions are provided mainly as an
API option that might streamline coding of some loading processes.
 
Variable arguments must use the `SQLITE_BIND_ARRAY_XXX` macros to provide type
information, to cast the arguments which will help ensure the stack is
formatted correctly, and permits the compiler to better control the stack. 
They also facilitate the passing of extra information about certain types, 
e.g. the size of a blob.  
 
All the arrays passed to the insert functions must contain at least `rows` number
of elements. Native types (like `int` and `double`) are contiguous allocations e.g.
`double dbl_array[SIZE];` or `double *dbl_array = new double[SIZE];` 
Blobs and text are arrays of pointers where each pointer points to the storage
for that entry. This is my most common use-case, and these assumptions simplify
the interface. It would be easy enough to add the other cases, and to add macros
to specify.
 
###Special case:

The `SQLITE_BIND_BLOCK_TEXT` macro supports a very specific use case in which
a single allocation contains a set of variable length strings, but each string
starts at a specific offset. For example, to contain 100 strings, each less than
50 bytes long would be allocated with `array = new char[100 * 50]`, and accessing
the 12th string would be `array+(11*50)`. This occurs in bulk loading scenarios,
so this marco supports it.

The `sqlite_bind_array` functions execute a single sql statement, multiple statments
separated by **;** are NOT supported. The stack arguments are processed in the
order they occur in the statement (left to right).

##Pseudo code examples:

```C
void *bitmaps[3];   
int sizes[3];
char **captions[3];
sqlite_int64 page_ids[3];

for (i=0;i<3;i++) 
{ bitmaps[i] = <pointer to image bitmap data probably unsigned char*>;
  sizes[i] = <size of this image bitmap>;
  captions[i] = <null terminated caption for this image>
  page_ids[i] = <page id of source of the image>
}  

int ret = sqlite_bind_array(
  db,                                                            // sqlite3* 
  "insert into images (caption, bitmap, pageid) values (?,?,?)"  // sql with parameters 
  3,                                                             // rows to insert
  SQLITE_BIND_ARRAY_TEXT(captions),                              // array of captions
  SQLITE_BIND_ARRAY_BLOB(bitmaps, sizes),                        // array of bitmaps and sizes for each
  SQLITE_BIND_ARRAY_INT64(page_ids),                             // array of page_ids
  SQLITE_BIND_ARRAY_END);                                        // end of variable args marker 
```

The stack must be terminated with the `SQLITE_BIND_ARRAY_END` marker or an error
will be returned.


 
