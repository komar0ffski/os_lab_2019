CC = gcc
CFLAGS = -I.
TARGETS = sequential_min_max parallel_min_max another_sequential
OBJS = find_min_max.o utils.o

all: $(TARGETS)

sequential_min_max: sequential_min_max.o $(OBJS)
	$(CC) -o sequential_min_max find_min_max.o utils.o sequential_min_max.c $(CFLAGS)

parallel_min_max: parallel_min_max.o $(OBJS)
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

another_sequential: another_sequential.o
	$(CC) -o another_sequential another_sequential.o $(CFLAGS)

another_sequential.o: another_sequential.c
	$(CC) -c -o another_sequential.o another_sequential.c $(CFLAGS)

clean:
	rm utils.o find_min_max.o sequential_min_max parallel_min_max another_sequential