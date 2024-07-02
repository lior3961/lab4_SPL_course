# Target to build the 'hexeditplus' executable
all: hexeditplus

# Rule to link the 'hexeditplus' executable
hexeditplus: hexeditplus.o
	gcc -m32 -g -Wall -o hexeditplus hexeditplus.o

# Rule to compile 'hexeditplus.c' into 'hexeditplus.o'
hexeditplus.o: hexeditplus.c
	gcc -m32 -g -Wall -c -o hexeditplus.o hexeditplus.c

# Phony target to clean up object files and the executable
.PHONY: clean
clean:
	rm -f *.o hexeditplus
