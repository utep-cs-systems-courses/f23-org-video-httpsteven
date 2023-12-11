# Makefile configuration
CPU             	= msp430g2553
CFLAGS          	= -mmcu=${CPU} -Os -I../h
LDFLAGS		= -L../lib -L/opt/ti/msp430_gcc/include/

# Switch the compiler (for the internal make rules)
CC              = msp430-gcc
AS              = msp430-gcc -mmcu=${CPU} -c

# Common objects
COMMON_OBJECTS = lcdutils.o lcddraw.o

all: pong.elf

# Additional rules for files
pong.elf: ${COMMON_OBJECTS} pong.o
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ $^

install: pong.elf
	mspdebug rf2500 "prog $^"

clean:
	rm -f *.o *.elf
