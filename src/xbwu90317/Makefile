CXX=g++
CXXFLAGS=-std=c++14 -Wall -pedantic -pthread -lboost_system
CXX_INCLUDE_DIRS=/usr/local/include
CXX_INCLUDE_PARAMS=$(addprefix -I , $(CXX_INCLUDE_DIRS))
CXX_LIB_DIRS=/usr/local/lib
CXX_LIB_PARAMS=$(addprefix -L , $(CXX_LIB_DIRS))

part1: http_server console.cgi
	
part2: cgi_server.exe

http_server: http_server.cpp
	$(CXX) $< -o $@ $(CXX_INCLUDE_PARAMS) $(CXX_LIB_PARAMS) $(CXXFLAGS)

console.cgi: console.cpp
	$(CXX) $< -o $@ $(CXX_INCLUDE_PARAMS) $(CXX_LIB_PARAMS) $(CXXFLAGS)

cgi_server.exe: cgi_server.cpp console_run.hpp
	g++ $< -o $@ -lws2_32 -lwsock32 -std=c++14 -ID:\MinGW\include -g

clean:
	rm -f http_server

format:
	clang-format -i *.cpp *.hpp --style=file