CC = gcc 
INCLUDES = -I/home/mrjantz/cs360/include
CFLAGS = $(INCLUDES) -g -Wall -Wextra
LIBDIR = /home/mrjantz/cs360/lib
LIBS = $(LIBDIR)/libfdr.a 

EXECUTABLES = bin/famtree

all: $(EXECUTABLES)

bin/famtree: src/famtree.c
	$(CC) $(CFLAGS) -o bin/famtree src/famtree.c $(LIBS)

clean:
	rm -f core $(EXECUTABLES) *.o

# Run Valgrind to check for memory leaks
valgrind: bin/famtree
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./bin/famtree < data/fam1.txt
