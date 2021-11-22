OBJS = main.o station.o satellite.o sensor.o check_sentinel.o
CC = mpicc
CFLAGS = -Wall -c
LFLAGS = -Wall
LM = -lm
np = 10
row = 3
col = 3
threshold = 6000
iter = 10

main: $(OBJS)
		$(CC) $(LFLAGS) $(OBJS) -o main $(LM)

clean:
		-rm $(OBJS) main log.txt

run:
		mpirun -oversubscribe -np $(np) main $(row) $(col) $(threshold) $(iter)

main.o: main.c station.h sensor.h
		$(CC) $(CFLAGS) main.c

station.o: station.c station.h satellite.h check_sentinel.h
		$(CC) $(CFLAGS) station.c

sensor.o: sensor.c sensor.h
		$(CC) $(CFLAGS) sensor.c
			
satellite.o: satellite.c satellite.h
		$(CC) $(CFLAGS) satellite.c
		
check_sentinel.o: check_sentinel.c check_sentinel.h
		$(CC) $(CFLAGS) check_sentinel.c
