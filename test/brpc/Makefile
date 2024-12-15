all:server client
server: server.cc main.pb.cc
	g++ $^ -o $@ -std=c++17 -L/usr/local/lib -lbrpc -lleveldb -lgflags -lssh -lcrypto -lprotobuf 
client: client.cc main.pb.cc
	g++ $^ -o $@ -std=c++17 -L/usr/local/lib -lbrpc -lleveldb -lgflags -lssh -lcrypto -lprotobuf
.PHONY:clean
clean:
	rm server client