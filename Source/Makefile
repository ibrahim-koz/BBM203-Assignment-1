OBJS	= matrixman.o
SOURCE	= matrixman.c
HEADER	= 
OUT	= matrixman
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

matrixman.o: matrixman.c
	$(CC) $(FLAGS) matrixman.c 


clean:
	rm -f $(OBJS) $(OUT)
