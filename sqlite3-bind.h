/* ---------------------------------------------------------------------------
** SQLite C API - Parameter binding helper for SQLite.
** ---------------------------------------------------------------------------
** Copyright (c) 2016 by Payton Bissell, payton.bissell@gmail.com
** ---------------------------------------------------------------------------
** Permission to use, copy, modify, and/or distribute this software for any
** purpose with or without fee is hereby granted. 
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
** OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
** DEALINGS IN THE SOFTWARE.
**
** Also offered with MIT License. 
**
** INTENTION: I, Payton Bissell, the sole author of this source code, dedicate
** any and all copyright interest in this code to the public domain. I make
** this dedication for the benefit of the public at large and to the detriment
** of my heirs and successors. I intend this dedication to be an overt act of
** relinquishment in perpetuity of all present and future rights to this code
** under copyright law.
**
** Copyleft is the antithesis of freedom. Please resist it.
** ---------------------------------------------------------------------------
** You can disable stack termination checking by defining
**   I_SQLITE_BIND_STACK_NOT_CHECKED prior to sqlite3-bind.h
**   e.g. gcc -DI_SQLITE_BIND_STACK_NOT_CHECKED
**
** ---------------------------------------------------------------------------
*/

#ifndef _SQLITE3_BIND_H_
#define _SQLITE3_BIND_H_

#include <stdarg.h>     /* Needed for the definition of va_list */
#include "sqlite3.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------------------
** Error codes: Since this tool simple wraps SQLite calls, most of the 
** return codes are those returned by SQLite. Using negative result/error
** codes until more research can be done. 
*/
#define SQLITE_ERR_BIND_STACK_NOT_TERMINATED    (-1)   // no end marker after all params are bound
#define SQLITE_ERR_BIND_STACK_MISSING_PARAMS    (-2)   // end marker before all params are bound
#define SQLITE_ERR_BIND_STACK_GUIDE_INVALID     (-3)   // guide marker was not recognized.
#define SQLITE_ERR_BIND_RESULT_COLUMNS_COUNT    (-4)   // the number of result columns does not match setup
#define SQLITE_ERR_BIND_ARRAY_CANT_HAVE_RESULTS (-5)   // for now the bind_array functions cannot return results
                                                       // a query that generates results will create this error

/* ---------------------------------------------------------------------------
** INTERNAL random guide bytes that provide some confidence that the stack 
** variables are the correct type
*/
#define I_SQLITE_BIND_TYPE_BLOB    ((unsigned int)0x156a82f4)
#define I_SQLITE_BIND_TYPE_DOUBLE  ((unsigned int)0x3a4947a1)
#define I_SQLITE_BIND_TYPE_INT     ((unsigned int)0xedf19d91)
#define I_SQLITE_BIND_TYPE_INT64   ((unsigned int)0xc3aea174)
#define I_SQLITE_BIND_TYPE_NULL    ((unsigned int)0x93640739)
#define I_SQLITE_BIND_TYPE_TEXT    ((unsigned int)0xa352651d)
#define I_SQLITE_BIND_TYPE_TEXT16  ((unsigned int)0xe55e1879)
#define I_SQLITE_BIND_TYPE_ZBLOB   ((unsigned int)0x7c435b51)  

/* ---------------------------------------------------------------------------
** The sqlite_bind_exec functions extends the sqlite_exec API pattern
** but supports variable arguments that will be bound to '?' markers
** in the sql statement.
** 
** Variable arguments must use the SQLITE_BIND_XXX macros to provide type
** information, to cast the argument which notifies the compiler of the type and
** ensures the correct size conversion to the stack, and facilitates the passing
** of extra parameters with the argument (such as the size parameter in the 
** SQLITE_BIND_BLOB macro) . 
** 
** Like sqlite_exec, the sqlite_bind_exec functions will process multiple sql 
** statements that are separated by a ';'. The stack arguments are processed in the
** order they occur in the statements (left to right).
**
** Pseudo code examples:
**
** sqlite_uint64 pageid = <pageid>;
** unsigned char *bitmap = <bitmap data>;
** char *caption = <caption>;
**
** int ret = sqlite_bind_exec(
**   db, 
**   "update images set caption=?, bitmap=? where pageid=?", 
**   NULL, NULL,                                                   // no callback or callback arg
**   SQLITE_BIND_TEXT(caption), 
**   SQLITE_BIND_BLOB(bitmap, size), 
**   SQLITE_BIND_INT64(page_id), 
**   SQLITE_BIND_END);
** 
** int ret = sqlite_bind_exec(
**   db, 
**   "select bitmap, size from images where pageid=? or caption=?", 
**   callback, cb_arg,                                              // callback to receive results
**   SQLITE_BIND_INT64(page_id), 
**   SQLITE_BIND_TEXT(caption), 
**   SQLITE_BIND_END);
** 
** The stack must be terminated with the SQLITE_BIND_END marker or an error
** will be returned (after the statement(s) are executed successfully).
** 
** The _va functions use a va_list instead of the stack to the immediate call, akin
** to the vsprintf functions in the standard C library. They are provided to support
** sqlite wrapper libraries so that the wrapper developers can expose their own binding
** functions however they choose, but they can still utilize these functions.
**
** ---------------------------------------------------------------------------
*/

/* ---------------------------------------------------------------------------
** Public function to use in-place of sqlite3_errmsg
*/
const char *sqlite3_bind_errmsg(sqlite3 *db);

/* ---------------------------------------------------------------------------
** User macros for pushing params on the stack for binding, provide guide marker and casting
*/
#define SQLITE_BIND_BLOB(b, s)    I_SQLITE_BIND_TYPE_BLOB, (int)(s), (const void*)(b)
#define SQLITE_BIND_DOUBLE(d)     I_SQLITE_BIND_TYPE_DOUBLE, (double)(d)
#define SQLITE_BIND_INT(i)        I_SQLITE_BIND_TYPE_INT, (int)(i)
#define SQLITE_BIND_INT64(i)      I_SQLITE_BIND_TYPE_INT64, (sqlite3_int64)(i)
#define SQLITE_BIND_NULL          I_SQLITE_BIND_TYPE_NULL
#define SQLITE_BIND_TEXT(t)       I_SQLITE_BIND_TYPE_TEXT, (const char*)(t)
#define SQLITE_BIND_TEXT16(t)     I_SQLITE_BIND_TYPE_TEXT16, (const void*)(t)
#define SQLITE_BIND_ZBLOB(s)      I_SQLITE_BIND_TYPE_ZBLOB, (int)(s)
#define SQLITE_BIND_END           ((unsigned int)0x87fa3dab)

/* ---------------------------------------------------------------------------
** User functions
*/
int sqlite3_bind_exec      (sqlite3 *db, const char *sql, int (*callback)(void*,int,char**,char**), void *arg, ...);
int sqlite3_bind_exec16    (sqlite3 *db, const void *sql, int (*callback)(void*,int,char**,char**), void *arg, ...);
int sqlite3_bind_exec_va   (sqlite3 *db, const char *sql, int (*callback)(void*,int,char**,char**), void *arg, va_list params);
int sqlite3_bind_exec_va16 (sqlite3 *db, const void *sql, int (*callback)(void*,int,char**,char**), void *arg, va_list params);

/* ---------------------------------------------------------------------------
** The sqlite_bind_array functions are a convienence for inserting arrays
** of data in a single call using the argument binding features of sqlite.
** 
** The bind_array functions do not return results right now. There does not
** not seem to be a need to execute a set of selects with an array of data.
** 'inserts' and 'updates' are better suited to this feature. The functions
** will return an error if the statement has result columns.
**
** Array binding for inserts can be significantly more efficient since the
** sql statement need not be parsed for each inserted row.
** 
** Variable arguments must use the SQLITE_BIND_ARRAY_XXX macros to provide type
** information, to cast the arguments which will help ensure the stack is
** formatted correctly, and that the compiler can manage the casting. They also
** facilitate the passing of extra information about certain types, e.g. the size
** of a blob.  
**
** All the arrays passed to the insert functions must contain at least "rows" number
** of elements. Native types (like int and double) are contiguous allocations e.g.
** double dbl_array[SIZE]; or double *d = new double[SIZE]; 
** Blobs and text are arrays of pointers where each pointer points to the storage
** for that entry. This is my most common use-case, and these assumptions simplify
** the interface. It would be easy enough to add the other cases, and to add macros
** to specify.
**
** The sqlite_bind_array functions execute a single sql statement, multiple statments
** separated by ';' are not supported. The stack arguments are processed in the
** order they occur in the statement (left to right).
**
** Psuedo code examples:
**
** void *bitmaps[3];   
** int sizes[3];
** char **captions[3];
** sqlite_int64 page_ids[3];
** 
*  for (i=0;i<3;i++) 
** { bitmaps[i] = <pointer to image bitmap data probably unsigned char*>;
**   sizes[i] = <size of this image bitmap>;
**   captions[i] = <null terminated caption for this image>
**   page_ids[i] = <page id of source of the image>
** }  
** 
** int ret = sqlite_bind_array(
**   db,                                                            // sqlite3* 
**   "insert into images (caption, bitmap, pageid) values (?,?,?)"  // sql with parameters 
**   3,                                                             // rows to insert
**   SQLITE_BIND_ARRAY_TEXT(captions),                              // array of captions
**   SQLITE_BIND_ARRAY_BLOB(bitmaps, sizes),                        // array of bitmaps and sizes for each
**   SQLITE_BIND_ARRAY_INT64(page_ids),                             // array of page_ids
**   SQLITE_BIND_ARRAY_END);                                        // end of variable args marker 
** 
** The BLOCK functions are useful when the allocation for strings is square (fixed size).
** Each strings starts at a multiple of the first dimension, but each string is terminated
** and must be less than the first dimension. Otherwise the array is an array of pointers
** which can be used in a block too, but the BLOCK functions eliminate the need for the
** array of pointers. 
**
** The stack must be terminated with the SQLITE_BIND_END marker or an error
** will be returned.
** 
** The _va functions use a va_list instead of the stack to the immediate call, akin
** to the vsprintf functions in the standard C library. They are provided to support
** sqlite wrapper libraries so that the wrapper writer can expose binding function
** however they choose, and can in turn call the bind_exec_va versions with the
** sql arguments.
**
** ---------------------------------------------------------------------------
*/

/* ---------------------------------------------------------------------------
** User macros for pushing params on the stack for binding arrays
*/
#define SQLITE_BIND_ARRAY_BLOB(ba,sa)    (I_SQLITE_BIND_TYPE_BLOB+1), (int*)(sa), (const void**)(ba)
#define SQLITE_BIND_ARRAY_DOUBLE(da)     (I_SQLITE_BIND_TYPE_DOUBLE+1), (double*)(da)
#define SQLITE_BIND_ARRAY_INT(ia)        (I_SQLITE_BIND_TYPE_INT+1), (int*)(ia)
#define SQLITE_BIND_ARRAY_INT64(ia)      (I_SQLITE_BIND_TYPE_INT64+1), (sqlite3_int64*)(da)
#define SQLITE_BIND_ARRAY_TEXT(ta)       (I_SQLITE_BIND_TYPE_TEXT+1), (const char**)(ta)
#define SQLITE_BIND_ARRAY_TEXT16(ta)     (I_SQLITE_BIND_TYPE_TEXT16+1), (const void**)(ta)
#define SQLITE_BIND_ARRAY_NULL           (I_SQLITE_BIND_TYPE_NULL+1)
#define SQLITE_BIND_ARRAY_ZBLOB(s)       (I_SQLITE_BIND_TYPE_ZBLOB+1), (int)(s)

#define SQLITE_BIND_BLOCK_TEXT(t,s)      (I_SQLITE_BIND_TYPE_TEXT+2), (int)(s), (const char*)(t)

int sqlite3_bind_array      (sqlite3 *db, const char *sql, int rows, ...);
int sqlite3_bind_array_va   (sqlite3 *db, const char *sql, int rows, va_list params);
int sqlite3_bind_array16    (sqlite3 *db, const void *sql, int rows, ...);
int sqlite3_bind_array_va16 (sqlite3 *db, const void *sql, int rows, va_list params);

#ifdef __cplusplus
}
#endif

#endif // _SQLITE3_BIND_H_

/* EOF */