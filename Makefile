SRCS:=WsConnection.cpp WsHeader.cpp WsMessage.cpp WsServer.cpp main.cpp
OBJS:=$(patsubst %.cpp, %.o, $(SRCS))

all: microws

microws: $(OBJS)
	g++ -Werror -Wall -g -o $@ $^ -lcrypto

%.o: %.cpp
	g++ -Werror -Wall -g -c $^

clean:
	rm -f *.o
	rm -f microws
