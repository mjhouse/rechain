CXX=g++-5
CC=gcc

# This is overridden from the command line
# to provide arguments to the tests
TAGGED =
VERSION= $(shell git describe --abbrev=0 --tags)

TARGET = bin/rechain
OUTDIR = bin
BLDDIR = obj
INCDIR = inc
SRCDIR = src
TSTDIR = test

INC = -I$(INCDIR) -I$(INCDIR)/dependencies

# find all source files in srcdir
TSOURCES := $(shell find $(SRCDIR) $(TSTDIR) -type f -name '*.cpp' -not -name "main.cpp")
SOURCES  := $(shell find $(SRCDIR)/ -type f -name '*.cpp')

# assembles each source file into a BLDIR/*.o filename
OBJECTS  := $(SOURCES:$(SRCDIR)/%.cpp=$(BLDDIR)/%.o)
TOBJECTS := $(patsubst %.cpp, $(BLDDIR)/%.o, $(notdir $(TSOURCES)))

# clear out tmp files from the test dir
$(shell rm -f test/data/files/tmp/*)

# ----------------------------------------------------------------------
# DON'T EDIT BELOW THIS LINE
# ----------------------------------------------------------------------
all: ./configure.sh

debug: CPPFLAGS = -std=c++11 -lcrypto++ -lpthread -Wall -Wextra -Wpedantic -g -ggdb -lboost_system -lboost_filesystem 
debug: link-debug

test: CPPFLAGS = -std=c++11 -lcrypto++ -lpthread -Wall -Wextra -Wpedantic -g -Wno-deprecated -fprofile-arcs -ftest-coverage -lboost_filesystem -lboost_system -DTEST_ROOT=\"test/data\" 
test: link-test
	./bin/rechain ${TAGGED}

release: CPPFLAGS = -DNDEBUG -std=c++11 -lpthread -lcrypto++ -O3 -lboost_system -lboost_filesystem
release: link-release

# LINK
link-debug: $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(CPPFLAGS)

link-test: $(TOBJECTS)
	$(CXX) $(TOBJECTS) -o $(TARGET) $(CPPFLAGS)

link-release: $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(CPPFLAGS)

# BUILD
obj/%.o: $(TSTDIR)/%.cpp
	$(CXX) -DRECHAIN_VERSION=\"$(VERSION)\" $(INC) -c $< -o $@ $(CPPFLAGS)

obj/%.o: $(SRCDIR)/%.cpp
	$(CXX) -DRECHAIN_VERSION=\"$(VERSION)\" $(INC) -c $< -o $@ $(CPPFLAGS)

clean:
	find $(BLDDIR)/ ! -name '.empty' -type f -exec rm -f {} +;
	find $(OUTDIR)/ ! -name '.empty' -type f -exec rm -f {} +;
