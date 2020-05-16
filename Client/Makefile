# this is an example makefile for the first homework
# assignment this semester

CXXFLAGS = g++ -DASIO_STANDALONE -Wall -O0 -g -std=c++11 -I./include -I./asio-1.12.2/include `pkg-config gtkmm-3.0 --cflags`
LDLIBS = `pkg-config gtkmm-3.0 --libs` -lX11 -lpthread
all: asio-1.12.2  chat_client

run:
	bin/chat_client

asio-1.12.2:
	tar xzf asio-1.12.2.tar.gz

chat_client: ./bin/chat_client.o ./bin/Connection.o ./bin/UI.o
	$(CXXFLAGS) ./bin/chat_client.o ./bin/Connection.o ./bin/UI.o -o ./bin/chat_client $(LDLIBS)

./bin/chat_client.o: ./src/chat_client.cpp
	$(CXXFLAGS) -c ./src/chat_client.cpp -o ./bin/chat_client.o

./bin/Connection.o: ./src/Connection.cpp
	$(CXXFLAGS) -c ./src/Connection.cpp -o ./bin/Connection.o

./bin/UI.o: ./src/UI.cpp
	$(CXXFLAGS) -c ./src/UI.cpp -o ./bin/UI.o

clean:
	-rm -rf asio-1.12.2
	-rm -f bin/*
