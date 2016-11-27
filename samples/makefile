#---------------------------------------------------------------------------
# SV TOOLS
#---------------------------------------------------------------------------

CC = gcc -ggdb -DSVLINUX -I.

#----------------------------------------------------------- DRIVERS -------

xjsf: xjs.h xjs.c xjsf.c
	$(CC) -o xjsf -DXJS_CFLAG_ENABLE_BLOCKS xjs.c xjsf.c 

xxxxjsf: xjs.h xjs.c xjsf.c
	$(CC) -DXJS_CFLAG_DISABLE_DESCRIPTIONS -DXJS_CFLAG_ENABLE_BLOCKS -o xjsf xjs.c xjsf.c 



sqlite3.o: $(SQLITE_SRC)
	$(CC)  -o xjsf xjs.c mfsd.cpp mfsstr.cpp $(SVSRC) sqlite3-bind.o sqlite3.o -luuid -ldl -lpthread -lreadline
	gcc -c $(SQLITE_SRC)

sqlite3-bind.o: $(SQLITE_BIND_SRC)
	gcc -c $(SQLITE_BIND_SRC)

mfssql.cpp: ../../schema/mfs.sql csql
	./csql in=../../schema/mfs.sql var=g_sys_sql static out=mfssql.cpp

csql: $(SVINC)/csql.cpp
	$(CC) -o csql $(SVINC)/csql.cpp $(SVSRC) -luuid

test:
	$(CC) -o test test.cpp -lreadline


load:
	$(CC) -o ex_load ex_load.c sqlite3-bind.c sqlite3.c -ldl -lpthread


#----------------------------------------------------------- CLEAN -------

clean:
	rm *.o

cleanall:
	rm xjfs


