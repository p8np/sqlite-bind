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
#define SQLITE_ERR_BIND_STACK_NOT_TERMINATED  (-1)   // no end marker after all params are bound
#define SQLITE_ERR_BIND_STACK_MISSING_PARAMS  (-2)   // end marker before all params are bound
#define SQLITE_ERR_BIND_STACK_GUIDE_INVALID   (-3)   // guide marker was not recognized.
#define SQLITE_ERR_BIND_RESULT_COLUMNS_COUNT  (-4)   // the number of result columns does not match setup


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

#ifdef __cplusplus
}
#endif

#endif // _SQLITE3_BIND_H_

/* EOF */