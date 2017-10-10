#
# Makefile for TrueNorth Simulator
# Note: requires a 64-bit x86-64 system
#
CC =gcc
CFLAGS = -g -Wall -Werror -m64
ALL = main.o chip.o tokencontroller.o neuronblock.o coresram.o router.o scheduler.o queue.o

all : truenorth datagen

# truenorth: main execution file (TrueNorth micro-architecture Simulator)
truenorth : ${ALL}
	${CC} ${CFLAGS} -o truenorth ${ALL}

main.o : main.c
	${CC} ${CFLAGS} -c -o main.o main.c

chip.o : chip.c
	${CC} ${CFLAGS} -c -o chip.o chip.c

tokencontroller.o : tokencontroller.c
	${CC} ${CFLAGS} -c -o tokencontroller.o tokencontroller.c

neuronblock.o : neuronblock.c
	${CC} ${CFLAGS} -c -o neuronblock.o neuronblock.c

coresram.o : coresram.c
	${CC} ${CFLAGS} -c -o coresram.o coresram.c

queue.o : queue.c
	${CC} ${CFLAGS} -c -o queue.o queue.c

router.o : router.c
	${CC} ${CFLAGS} -c -o router.o router.c

scheduler.o : scheduler.c
	${CC} ${CFLAGS} -c -o scheduler.o scheduler.c

# datagen: create a neuron_info file of TrueNorth
datagen : datagen.c
	${CC} ${CFLAGS} -o datagen datagen.c
	./datagen

clean :
	rm *.o
	rm truenorth
	rm datagen
	rm data/random_data