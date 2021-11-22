CC=gcc
DFLAGS=I.
DEPS = NMEA_parser.h
OBJ = main.o NMEA_parser.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
