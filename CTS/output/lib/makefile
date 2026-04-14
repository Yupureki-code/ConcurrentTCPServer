test:test.cpp
	@g++ -g -O0 -fno-inline test.cpp -o test -lCTS -lpthread -lstdc++ -I../include -L. -Wl,-rpath,.

.PHONY:clean
clean:
	@rm test