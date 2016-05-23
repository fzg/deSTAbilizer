SRC= main.c desta.c util.c fmt.c arg_parser.c opts.c
PRG= deSTAbilizer
CC = gcc
CFLAGS =  --std=c99 -gdwarf-4 -g2 -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE
LDFLAGS = -lcrypto -lssl

SRCDIR = src
OBJDIR = obj


OBJ=$(SRC:%.c=$(OBJDIR)/%.o)

$(OBJDIR)/%.o:$(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

$(PRG) : $(OBJ)
	$(CC) $(CFLAGS) -o $(PRG) $(OBJ) $(LDFLAGS)

clean:
	-@rm $(OBJ) $(PRG)

re   : clean $(PRG)
