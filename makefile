CXX=g++-5
CC=gcc

# This is overridden from the command line
# to provide arguments to the tests
TAGGED  =
VERSION = $(shell git describe --abbrev=0 --tags)
COMMON  = -std=c++11 -lpthread -lcrypto++ -lboost_serialization -lboost_filesystem -lboost_system -lboost_thread -lboost_regex -ltorrent-rasterbar 


TARGET = bin/rechain
OUTDIR = bin
BLDDIR = obj
INCDIR = inc
SRCDIR = src
TSTSRC = test/src
TSTINC = test/inc

# create directories
$(shell mkdir -p obj bin test/data/home )

# clear out tmp files from the test dir
$(shell rm -rf test/data/home/*)

# find all source files in srcdir
TSOURCES := $(shell find $(SRCDIR) $(TSTSRC) -type f -name '*.cpp' -not -name "main.cpp")
SOURCES  := $(shell find $(SRCDIR)/ -type f -name '*.cpp')

# assembles each source file into a BLDIR/*.o filename
OBJECTS  := $(SOURCES:$(SRCDIR)/%.cpp=$(BLDDIR)/%.o)
TOBJECTS := $(patsubst %.cpp, $(BLDDIR)/%.o, $(notdir $(TSOURCES)))

INC = -I$(INCDIR) -I$(INCDIR)/dependencies 

# ----------------------------------------------------------------------
# DON'T EDIT BELOW THIS LINE
# ----------------------------------------------------------------------

init: $(shell git config core.hooksPath .hooks)

debug: CPPFLAGS = ${COMMON} -Wall -Wextra -Wpedantic -g -ggdb
debug: link-debug

test: CPPFLAGS = ${COMMON} -Wall -Wextra -Wpedantic -g -fprofile-arcs -ftest-coverage -DTEST_ROOT=\"test/data\" 
test: INC += -I$(TSTINC)
test: link-test

release: CPPFLAGS = ${COMMON} -DNDEBUG -O3
release: link-release

# LINK
link-debug: $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(CPPFLAGS)

link-test: $(TOBJECTS)
	$(CXX) $(TOBJECTS) -o $(TARGET) $(CPPFLAGS)

link-release: $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(CPPFLAGS)

# BUILD
obj/%.o: $(TSTSRC)/%.cpp
	$(CXX) -DRECHAIN_VERSION=\"$(VERSION)\" $(INC) -c $< -o $@ $(CPPFLAGS)

obj/%.o: $(SRCDIR)/%.cpp
	$(CXX) -DRECHAIN_VERSION=\"$(VERSION)\" $(INC) -c $< -o $@ $(CPPFLAGS)

clean:
	find $(BLDDIR)/ -type f -exec rm -f {} +;
	find $(OUTDIR)/ -type f -exec rm -f {} +;
