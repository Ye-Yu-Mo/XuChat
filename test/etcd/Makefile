all:put get registry discovery
put:put.cc
	g++ -std=c++17 $^ -o $@ -letcd-cpp-api -lcpprest
get:get.cc
	g++ -std=c++17 $^ -o $@ -letcd-cpp-api -lcpprest
registry:registry.cc
	g++ -std=c++17 $^ -o $@ -letcd-cpp-api -lcpprest -lgflags
discovery:discovery.cc
	g++ -std=c++17 $^ -o $@ -letcd-cpp-api -lcpprest -lgflags

.PHONY:clean
clean:
	rm put get registry discovery