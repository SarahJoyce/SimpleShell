# choose your compiler
CC=gcc
#CC=gcc -Wall

mysh: sh.o get_path.o main.c 
	$(CC) -g main.c sh.o get_path.o -o mysh
#	$(CC) -g main.c sh.o get_path.o bash_getcwd.o -o mysh

sh.o: sh.c sh.h
	$(CC) -g -c sh.c

get_path.o: get_path.c get_path.h
	$(CC) -g -c get_path.c

clean:
	rm -rf *.o mysh

debug: sh_debug
	valgrind --leak-check=full -v ./sh_debug
sh_debug: sh_debug.o get_path.o main.c
	gcc -g -D debug sh_debug.o get_path.o main.c -o sh_debug
sh_debug.o: sh.c sh.h
	gcc -c -D debug -g sh.c -o sh_debug.o
