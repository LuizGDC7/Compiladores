CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = g-v1

SRCS = main.c ast.c symbol_table.c semantic.c codegen.c g-v1.tab.c lex.yy.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): g-v1.tab.c lex.yy.c $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

g-v1.tab.c g-v1.tab.h: g-v1.y
	bison -d g-v1.y

lex.yy.c: g-v1.l g-v1.tab.h
	flex g-v1.l

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) *.o g-v1.tab.c g-v1.tab.h lex.yy.c

.PHONY: all clean
