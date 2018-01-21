CXX=g++-5
CC=gcc

# This is overridden from the command line
# to provide arguments to the tests
TAGGED =

TARGET = bin/rechain
OUTDIR = bin
BLDDIR = obj
INCDIR = inc
SRCDIR = src
TSTDIR = test

INC = -I$(INCDIR) -I$(INCDIR)/dependencies

# find all source files in srcdir
TSOURCES := $(shell find $(SRCDIR) $(TSTDIR)/ -type f -name '*.cpp' -not -name "main.cpp")
SOURCES  := $(shell find $(SRCDIR)/ -type f -name '*.cpp')

# assembles each source file into a BLDIR/*.o filename
OBJECTS  := $(SOURCES:$(SRCDIR)/%.cpp=$(BLDDIR)/%.o)
TOBJECTS := $(patsubst %.cpp, $(BLDDIR)/%.o, $(notdir $(TSOURCES)))

# ----------------------------------------------------------------------
# DON'T EDIT BELOW THIS LINE
# ----------------------------------------------------------------------
all: ./configure.sh

debug: CPPFLAGS = -std=c++11 -lcrypto++ -lpthread -Wall -Wextra -Wpedantic -g -ggdb -lcrypto++
debug: link-debug

test: CPPFLAGS = -std=c++11 -lcrypto++ -lpthread -Wall -Wextra -Wpedantic -g -fprofile-arcs -ftest-coverage 
test: link-test
	./bin/rechain ${TAGGED}

release: CPPFLAGS = -std=c++11 -lpthread -lcrypto++ -O3
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
	$(CXX) $(INC) -c $< -o $@ $(CPPFLAGS)

obj/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(INC) -c $< -o $@ $(CPPFLAGS)

clean:
	rm $(BLDDIR)/*; rm $(OUTDIR)/*;
