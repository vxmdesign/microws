SRCS:=WsConnection.cpp WsHeader.cpp WsMessage.cpp WsServer.cpp 
INSTALL_HEADERS:=WsConnection.h WsHeader.h WsMessage.h WsServer.h uWs.h
TEST_SRCS:= main.cpp
OBJS:=$(patsubst %.cpp, %.o, $(SRCS))
TEST_OBJS=$(patsubst %.cpp, %.o, $(TEST_SRCS))

CFLAGS= -g -O3 -Wall -Werror -fPIC
PREFIX ?= .

all: libmicrows.so microws

install: libmicrows.so
	mkdir -p $(PREFIX)/include
	mkdir -p $(PREFIX)/lib
	install -m 644 $(INSTALL_HEADERS) $(PREFIX)/include
	install -m 755 libmicrows.so $(PREFIX)/lib

libmicrows.so: $(OBJS)
	g++ $(CFLAGS) -shared -o $@ $^ -lcrypto

microws: $(OBJS) $(TEST_OBJS)
	g++ $(CFLAGS) -o $@ $^ -lcrypto

%.o: %.cpp
	g++ $(CFLAGS) -g -c $^

clean:
	rm -f *.o
	rm -f microws
	rm -f libmicrows.so

