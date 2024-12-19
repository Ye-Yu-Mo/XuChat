all:test
test:test.cc
	g++ $^ -o $@ -std=c++23 -lcpr -lelasticlient -ldl -lssl -lcrypto -lz
.PHONY:clean
clean:
	rm test