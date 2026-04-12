test:test.cpp
	@g++ test.cpp -o test -std=c++17
.PHONY:clean
clean:
	@rm test