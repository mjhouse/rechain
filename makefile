CPP=g++
CPPFLAGS=-std=c++11 -pthread -ldl -Wall -Wextra -Wpedantic -g -lcrypto++
TSTFLAGS=-fprofile-arcs -ftest-coverage 

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
all: link

# link executable
link: $(OBJECTS)
	$(CPP) $(OBJECTS) -o $(TARGET) $(CPPFLAGS)

# build all .o files from .cpp source
obj/%.o: $(SRCDIR)/%.cpp
	$(CPP) $(INC) -c $< -o $@ $(CPPFLAGS)

# ----------------------------------------------------------------------
# Test Build
# ----------------------------------------------------------------------
# link executable for testing

test: test-link
	./bin/rechain

test-link: $(TOBJECTS)
	$(CPP) $(TOBJECTS) -o $(TARGET) $(CPPFLAGS) $(TSTFLAGS)

# build all .o files from .cpp source
obj/%.o: $(TSTDIR)/%.cpp
	$(CPP) $(INC) -c $< -o $@ $(CPPFLAGS) $(TSTFLAGS)

# build all .o files from .cpp source
obj/%.o: $(SRCDIR)/%.cpp
	$(CPP) $(INC) -c $< -o $@ $(CPPFLAGS) $(TSTFLAGS)

clean:
	rm $(BLDDIR)/*; rm $(OUTDIR)/*;
