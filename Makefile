# Makefile

LIB_MAJOR_VERSION = 1
LIB_MINOR_VERSION = 0
LIB_PATCH_VERSION = 0
LIB_FULL_VERSION = $(LIB_MAJOR_VERSION).$(LIB_MINOR_VERSION).$(LIB_PATCH_VERSION)


PREFIX = /usr
LIBDIR = $(PREFIX)/lib         
INCLUDEDIR = $(PREFIX)/include  
BINDIR = $(PREFIX)/bin         


LIB_SRC = src/lib/rdbcompare.cpp
LIB_HDR = src/lib/rdbcompare.hpp
CLI_SRC = src/cli/rdb_compare_cli.py 


LIB_OBJ_DIR = build/obj          
LIB_BUILD_DIR = build/lib        


LIB_NAME_BASE = librdbcompare.so                  
LIB_NAME_SONAME = $(LIB_NAME_BASE).$(LIB_MAJOR_VERSION) 
LIB_NAME_FULL = $(LIB_NAME_BASE).$(LIB_FULL_VERSION)    


LIB_PATH = $(LIB_BUILD_DIR)/$(LIB_NAME_FULL) 
LIB_OBJ = $(LIB_OBJ_DIR)/rdbcompare.o       


CXXFLAGS = -fPIC -Wall -g -std=c++17 -I$(CURDIR)/src/lib 
LDFLAGS = -shared -L/usr/lib -L/usr/lib64              
LIBS = -lcurl -ljson-c -lrpm                          


.PHONY: all clean install install_cli


all: $(LIB_PATH)


$(LIB_OBJ): $(LIB_SRC) $(LIB_HDR) | $(LIB_OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(LIB_OBJ_DIR):
	mkdir -p $(LIB_OBJ_DIR)

$(LIB_PATH): $(LIB_OBJ) | $(LIB_BUILD_DIR)
	$(CXX) $(LDFLAGS) -Wl,-soname,$(LIB_NAME_SONAME) -o $@ $< $(LIBS)

$(LIB_BUILD_DIR):
	mkdir -p $(LIB_BUILD_DIR)

install: $(LIB_PATH)
	install -d $(LIBDIR)                           
	install -m 644 $(LIB_PATH) $(LIBDIR)          
	ln -sf $(LIB_NAME_FULL) $(LIBDIR)/$(LIB_NAME_SONAME) 
	ln -sf $(LIB_NAME_SONAME) $(LIBDIR)/$(LIB_NAME_BASE) 
	install -d $(INCLUDEDIR)                       
	install -m 644 $(LIB_HDR) $(INCLUDEDIR)       
	/sbin/ldconfig                                 

install_cli:
	install -d $(BINDIR)                           
	install -m 755 $(CLI_SRC) $(BINDIR)/rdb_compare 

clean:
	rm -rf $(LIB_OBJ_DIR) $(LIB_BUILD_DIR)        
	rm -f $(LIBDIR)/$(LIB_NAME_BASE) $(LIBDIR)/$(LIB_NAME_SONAME)