CPP=g++
C=gcc
CPPFLAGS=-std=c++11 -ldl -Wall -Wextra -Wpedantic -g -lcrypto++

TARGET = bin/rechain
OUTDIR = bin
BLDDIR = obj
<<<<<<< HEAD
INCDIR = inc
SRCDIR = src

INC = -I$(INCDIR) -I$(INCDIR)/dependencies

ifdef test
$(shell find src/* -not -name "main.cpp" -exec cp '{}' test/ \;)
SRCDIR = test
endif

# find all source files in srcdir
SOURCES  := $(shell find $(SRCDIR)/ -type f -name '*.cpp')
CSOURCES := $(shell find $(SRCDIR)/ -type f -name '*.c')
=======
SRCDIR = src
INCDIR = inc

INC = -I$(INCDIR) -I$(INCDIR)/dependencies

# find all source files in src
SOURCES  := $(shell find src/ -type f -name '*.cpp')
CSOURCES := $(shell find src/ -type f -name '*.c')
>>>>>>> 87a9496026c2891bd964ab35e1298d3e1fc938ff

# assembles each source file into a BLDIR/*.o filename
OBJECTS  := $(SOURCES:$(SRCDIR)/%.cpp=$(BLDDIR)/%.o)
COBJECTS := $(CSOURCES:$(SRCDIR)/%.c=$(BLDDIR)/%.o)

# ----------------------------------------------------------------------
# DON'T EDIT BELOW THIS LINE
# ----------------------------------------------------------------------
all: link

# link executable
link: $(OBJECTS) $(COBJECTS)
	$(CPP) $(OBJECTS) $(COBJECTS) -o $(TARGET) $(CPPFLAGS)

# build all .o files from .cpp source
<<<<<<< HEAD
obj/%.o: $(SRCDIR)/%.cpp
	$(CPP) $(INC) -c $< -o $@ $(CPPFLAGS)

# build all .o files from .c source
obj/%.o: $(SRCDIR)/%.c
=======
obj/%.o: src/%.cpp
	$(CPP) $(INC) -c $< -o $@ $(CPPFLAGS)

# build all .o files from .c source
obj/%.o: src/%.c
>>>>>>> 87a9496026c2891bd964ab35e1298d3e1fc938ff
	$(C) $(INC) -c $< -o $@

clean:
	rm $(BLDDIR)/*.o; rm $(OUTDIR)/*;
