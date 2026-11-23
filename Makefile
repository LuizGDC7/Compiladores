# [G-V2] O executável e os arquivos de entrada do Bison/Flex passam a ser g-v2.
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = g-v2

SRCS = main.c ast.c symbol_table.c semantic.c codegen.c g-v2.tab.c lex.yy.c
OBJS = $(SRCS:.c=.o)
HDRS = ast.h symbol_table.h semantic.h codegen.h g-v2.tab.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# [G-V2] O grupo "&:" indica que uma única execução do Bison gera os dois arquivos.
g-v2.tab.c g-v2.tab.h &: g-v2.y
	bison -d g-v2.y

lex.yy.c: g-v2.l g-v2.tab.h
	flex g-v2.l

# [G-V2] Objetos dependem dos headers: mudar a AST recompila todos os módulos.
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS) g-v2.tab.c g-v2.tab.h lex.yy.c

.PHONY: all clean
