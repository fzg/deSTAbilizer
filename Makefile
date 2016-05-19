SRC= main.c desta.c x.c
PRG= deSTAbilizer
CC = gcc-5  --std=c99
CFLAGS = -gdwarf-4 -g2
LDFLAGS = -lcrypto -lssl

SRCDIR = src
OBJDIR = obj


OBJ=$(SRC:%.c=$(OBJDIR)/%.o)

$(OBJDIR)/%.o:$(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

$(PRG) : $(OBJ)
	$(CC) $(CFLAGS) -o $(PRG) $(OBJ) $(LDFLAGS)

clean:
	-rm $(OBJ) $(PRG)
