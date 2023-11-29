.PHONY: all

all: server_udp client_udp

server_udp:udpServer.cc
	g++ -o $@ $^ -std=c++11

client_udp:udpClient.cc
	g++ -o $@ $^ -std=c++11

.PHONY: clean
clean:
	rm -f client_udp server_udp

