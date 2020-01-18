CC = gcc
FLAGS = -std=c99 -w -O3 -Wextra -Werror -Wall -Wno-sign-compare -pedantic -lm

run-stat: mainStat.o libqueue.a
	$(CC) $(FLAGS) -o run-stat mainStat.o -L. -lqueue

mainStat.o: mainStat.c
	$(CC) $(FLAGS) -c mainStat.c

libqueue.a: queue.o
	ar cr libqueue.a queue.o

clean:
	rm -f *.o *.a run-stat