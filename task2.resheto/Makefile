CC = g++
CFLAGS = -std=c++11

all: init destroy check set

init: init.cpp
	$(CC) $(CFLAGS) -o init init.cpp

destroy: destroy.cpp
	$(CC) $(CFLAGS) -o destroy destroy.cpp

check: check.cpp
	$(CC) $(CFLAGS) -o check check.cpp

set: set.cpp
	$(CC) $(CFLAGS) -o set set.cpp

clean:
	rm -f init destroy check set
