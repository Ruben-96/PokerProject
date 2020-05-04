all: asio-1.12.2 src/dealer_server src/test_deck src/test_hand src/test_score src/test_win

asio-1.12.2:
	tar xzf asio-1.12.2.tar.gz

CXXFLAGS+= -DASIO_STANDALONE -Wall -O0 -g -std=c++11
CPPFLAGS+= -I./include -I./asio-1.12.2/include
LDLIBS+= -lpthread

test:
	./src/test_deck
	./src/test_hand
	./src/test_score
	./src/test_win
clean:
	-rm -rf asio-1.12.2
	-rm -f src/dealer_server
	-rm -f src/test_deck
	-rm -f src/test_hand
	-rm -f src/test_score
	-rm -f src/test_win
clean_test:
	-rm -f src/test_deck
	-rm -f src/test_hand
	-rm -f src/test_score
	-rm -f src/test_win
clean_main:
	-rm -f src/dealer_server
