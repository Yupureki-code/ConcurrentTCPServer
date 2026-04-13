.PHONY:all clean
all: client server

client:client.cpp
	@g++ client.cpp -o client

server:server.cpp
	@g++ server.cpp -o server -L../output/lib -L../../ConcurrentTCPServer/output/lib -lEchoServer -lCTS -lpthread -Wl,-rpath,'$$ORIGIN/../output/lib:$$ORIGAL/../../ConcurrentTCPServer/output/lib'

clean:
	@rm -f server client