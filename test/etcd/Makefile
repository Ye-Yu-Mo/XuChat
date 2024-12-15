all:put get registry discovery
put:put.cc
	g++ -std=c++17 $^ -o $@ -letcd-cpp-api -lcpprest
get:get.cc
	g++ -std=c++17 $^ -o $@ -letcd-cpp-api -lcpprest
registry:registry.cc main.pb.cc
	g++ -std=c++20 $^ -o $@ -letcd-cpp-api -lcpprest -lgflags -lbrpc -lgflags -lssl -lcrypto -lprotobuf -lleveldb -ldl
discovery:discovery.cc main.pb.cc
	g++ -std=c++20 $^ -o $@ -letcd-cpp-api -lcpprest -lgflags -lbrpc -lgflags -lssl -lcrypto -lprotobuf -lleveldb -ldl

.PHONY:clean
clean:
	rm put get registry discovery