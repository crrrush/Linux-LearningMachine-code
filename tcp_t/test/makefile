.PHONY:ALL
ALL:client server

client:client_tcp.cc
	g++  -o $@ $^ -std=c++11

server:server_tcp.cc
	g++  -o $@ $^ -std=c++11 -lpthread

.PHONY:clean
clean:
	rm -f client server