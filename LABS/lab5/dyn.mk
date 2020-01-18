CC = gcc
FLAGS = -std=c99 -w -O3 -Wextra -Werror -Wall -Wno-sign-compare -pedantic -lm

run-dyn: mainDyn.o libqueue.so
	$(CC) $(FLAGS) -o run-dyn mainDyn.o -L. -lqueue -Wl,-rpath,.

mainDyn.o: mainDyn.c
	$(CC) -c $(FLAGS) mainDyn.c

libqueue.so: queue.o
	gcc -shared -o libqueue.so queue.o

queue.o: queue.c
	$(CC) -c -fPIC $(FLAGS) queue.c

clean:
	rm -f *.o *.so run-dyn