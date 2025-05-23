CXX = g++
CXXFLAGS = -fPIC -Wall -g -std=c++17
LDFLAGS = -shared
LIBS = -lcurl -ljson-c
PREFIX = /usr
LIBDIR = $(PREFIX)/lib
BINDIR = $(PREFIX)/bin

all: librdbcompare.so

librdbcompare.so: rdbcompare.o
	$(CXX) $(LDFLAGS) -o $@ $< $(LIBS)

rdbcompare.o: rdbcompare.cpp rdbcompare.hpp
	$(CXX) $(CXXFLAGS) -c $<

install: librdbcompare.so
	install -d $(LIBDIR)
	install -m 644 librdbcompare.so $(LIBDIR)

clean:
	rm -f rdbcompare.o librdbcompare.so