CROSS_COMPILE ?=

CC	:= $(CROSS_COMPILE)gcc
CFLAGS	?= -W -Wall -Iinclude
LDFLAGS	?=
LIBS	:= -lrt -lpthread

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

all: yavta

yavta: yavta.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

grab: grab.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

grab.so: grab.c
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -fPIC -o $@ $^ $(LIBS)

clean:
	-rm -f *.o
	-rm -f yavta
	-rm -f *.so
