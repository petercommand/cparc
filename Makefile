CFLAGS  = -Wall -g


SRCS = main.c list.c input.c parser.c tests/test_parser.c
OBJS = $(SRCS:%.c=%.o)
BIN = main
%.o: %.c
	$(CC) $(CFLAGS) -c $<

all: $(BIN)


#main: $(OBJS)
#	$(CC) $(LDFLAGS) -o $@ $^
test: $(OBJS)
	$(CC) list.o input.o parser.o test_parser.o -o test_parser
	./test_parser
clean:
	rm -f list.o input.o parser.o test_parser.o test_parser

