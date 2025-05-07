CC = gcc
CFLAGS = -Ofast -march=native -funroll-loops -ffast-math -flto -fomit-frame-pointer -Wall -Wextra -lm
LDFLAGS = -lm
OBJ = ox.o io.o mem.o buffer.o array.o hash.o misc.o pmodels.o alphabet.o context.o
DEPS = defs.h io.h rc.h msg.h mem.h buffer.h array.h hash.h misc.h pmodel.h alphabet.h context.h

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

ox: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LDFLAGS)

.PHONY: clean

clean:
	rm -f *.o ox
