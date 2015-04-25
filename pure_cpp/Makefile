CROSS_COMPILE ?=

CC	:= g++
CFLAGS	?= -O2 -W -Wall -Iinclude -std=c++11
LDFLAGS	?=
LIBS	:= -lrt

%.o : %.cpp
	$(CC) $(CFLAGS) -c -o $@ $< $(shell pkg-config --cflags opencv)

all: yavta

yavta: yavta.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS) $(shell pkg-config --libs opencv)

clean:
	-rm -f *.o
	-rm -f yavta

