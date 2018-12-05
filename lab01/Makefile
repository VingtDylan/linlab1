LAB = shell

CC = gcc
OBJS = shell.o parse.o internal.o execute.o
FLAGS = -Wall -O1 -ggdb 
TARGET = njush

.PHONY :process

process:
	$(CC) $(FLAGS) -c shell.c 
	$(CC) $(FLAGS) -c parse.c
	$(CC) $(FLAGS) -c internal.c	
	$(CC) $(FLAGS) -c execute.c
	$(CC) -o $(TARGET) $(OBJS)
	rm -r $(OBJS)
	./$(TARGET)

