.PHONY:ALL
ALL:client server

client:client_tcp.cc
	g++  -o $@ $^ -std=c++11 -ljsoncpp

server:server_tcp.cc
	g++  -o $@ $^ -std=c++11 -lpthread -ljsoncpp

.PHONY:clean
clean:
	rm -f client server
