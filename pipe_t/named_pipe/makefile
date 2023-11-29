.PHONY: clean ALL

ALL: server client

server: server.cc
	g++ -o $@ $^ -std=c++11 -g

client: client.cc
	g++ -o $@ $^ -std=c++11 -g


clean:
	rm -f server client