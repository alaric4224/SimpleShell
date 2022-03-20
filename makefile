override CFLAGS := -Wall -Werror -std=gnu99 -O0 -g $(CFLAGS) -I.

# Build the parser .o file
myshell: script.c myshell_parser.c myshell_parser.h
	${CC} ${CFLAGS} script.c myshell_parser.c myshell_parser.h -o myshell

.PHONY: clean

clean:
	rm -f *.o myshell
