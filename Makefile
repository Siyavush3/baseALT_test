# Makefile

CXX = g++
CXXFLAGS = -fPIC -Wall -g -std=c++17 -I$(CURDIR)/src/lib 
LDFLAGS = -shared -L/usr/lib -L/usr/lib64 
LIBS = -lcurl -ljson-c -lrpm


PREFIX = /usr
LIBDIR = $(PREFIX)/lib
INCLUDEDIR = $(PREFIX)/include
BINDIR = $(PREFIX)/bin

LIB_SRC = src/lib/rdbcompare.cpp
LIB_HDR = src/lib/rdbcompare.hpp
CLI_SRC = src/cli/rdb_compare_cli.py 

LIB_OBJ_DIR = build/obj
LIB_BUILD_DIR = build/lib
LIB_NAME = librdbcompare.so
LIB_PATH = $(LIB_BUILD_DIR)/$(LIB_NAME)
LIB_OBJ = $(LIB_OBJ_DIR)/rdbcompare.o

.PHONY: all clean install

all: $(LIB_PATH)


$(LIB_OBJ): $(LIB_SRC) $(LIB_HDR) | $(LIB_OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@


$(LIB_OBJ_DIR):
	mkdir -p $(LIB_OBJ_DIR)

$(LIB_PATH): $(LIB_OBJ) | $(LIB_BUILD_DIR)
	$(CXX) $(LDFLAGS) -o $@ $< $(LIBS)


$(LIB_BUILD_DIR):
	mkdir -p $(LIB_BUILD_DIR)


install: $(LIB_PATH)
	install -d $(LIBDIR)
	install -m 644 $(LIB_PATH) $(LIBDIR)
	install -d $(INCLUDEDIR)
	install -m 644 $(LIB_HDR) $(INCLUDEDIR) 

install_cli:
	install -d $(BINDIR)
	install -m 755 $(CLI_SRC) $(BINDIR)/rdb_compare 


clean:
	rm -rf $(LIB_OBJ_DIR) $(LIB_BUILD_DIR)
	