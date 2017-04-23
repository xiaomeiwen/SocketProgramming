all: edge.cpp client.cpp  server_or.cpp server_and.cpp
	g++ edge.cpp -o ./edge
	g++ server_or.cpp -o ./server_or
	g++ server_and.cpp -o ./server_and
	g++ client.cpp -o ./client
.PHONY: edge
edge:
	./edge
.PHONY: server_and
server_and:
	./server_and
.PHONY: server_or
server_or:
	./server_or
.PHONY: client
client:
	./client
