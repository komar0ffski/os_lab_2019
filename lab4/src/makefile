CC = gcc
CFLAGS = -I.
TARGETS = parallel_min_max process_memory
OBJS = find_min_max.o utils.o

all: $(TARGETS)

parallel_min_max: parallel_min_max.o $(OBJS)
	$(CC) -o $@ parallel_min_max.o $(OBJS) $(CFLAGS)

process_memory: process_memory.o
	$(CC) -o $@ process_memory.o $(CFLAGS)

utils.o: utils.c utils.h
	$(CC) -c utils.c $(CFLAGS)

find_min_max.o: find_min_max.c find_min_max.h utils.h
	$(CC) -c find_min_max.c $(CFLAGS)

parallel_min_max.o: parallel_min_max.c
	$(CC) -c parallel_min_max.c $(CFLAGS)

process_memory.o: process_memory.c
	$(CC) -c process_memory.c $(CFLAGS)

clean:
	rm -f $(OBJS) parallel_min_max.o process_memory.o $(TARGETS)