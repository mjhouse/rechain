CPP=g++
CPPFLAGS = -std=c++11 -lcrypto++

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
debug: CPPFLAGS = -std=c++11 -lcrypto++ -Wall -Wextra -Wpedantic -g -ggdb -lcrypto++
debug: link-debug

test: CPPFLAGS = -std=c++11 -lcrypto++ -Wall -Wextra -Wpedantic -g -fprofile-arcs -ftest-coverage 
test: link-test
	./bin/rechain

release: CPPFLAGS = -std=c++11 -lcrypto++ -O3
release: link-release

# LINK
link-debug: $(OBJECTS)
	$(CPP) $(OBJECTS) -o $(TARGET) $(CPPFLAGS)

link-test: $(TOBJECTS)
	$(CPP) $(TOBJECTS) -o $(TARGET) $(CPPFLAGS)

link-release: $(OBJECTS)
	$(CPP) $(OBJECTS) -o $(TARGET) $(CPPFLAGS)

# BUILD
obj/%.o: $(TSTDIR)/%.cpp
	$(CPP) $(INC) -c $< -o $@ $(CPPFLAGS)

obj/%.o: $(SRCDIR)/%.cpp
	$(CPP) $(INC) -c $< -o $@ $(CPPFLAGS)

clean:
	rm $(BLDDIR)/*; rm $(OUTDIR)/*;
