all:server client
server: server.cc main.pb.cc
	g++ $^ -o $@ -std=c++17 -lbrpc -lleveldb -lgflags -lssh -lcrypto -lprotobuf -lpthread -ldl -lssl -lcrypto -lz
client: client.cc main.pb.cc
	g++ $^ -o $@ -std=c++17 -lbrpc -lleveldb -lgflags -lssh -lcrypto -lprotobuf -lpthread -ldl -lssl -lcrypto -lz
.PHONY:clean
clean:
	rm server client