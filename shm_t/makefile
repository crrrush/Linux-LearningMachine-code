.PHONY: ALL clean

ALL: server client

server:shm_server.cc
	g++ -o $@ $^ -std=c++11 -g

client:shm_client.cc
	g++ -o $@ $^ -std=c++11 -g

clean:
	rm -f server client
