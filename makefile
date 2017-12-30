CPP=g++
C=gcc
CPPFLAGS=-std=c++11 -ldl -Wall -Wextra -Wpedantic -g -lcrypto++

TARGET = bin/rechain
OUTDIR = bin
BLDDIR = obj
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
obj/%.o: $(SRCDIR)/%.cpp
	$(CPP) $(INC) -c $< -o $@ $(CPPFLAGS)

# build all .o files from .c source
obj/%.o: $(SRCDIR)/%.c
	$(C) $(INC) -c $< -o $@

clean:
	rm $(BLDDIR)/*.o; rm $(OUTDIR)/*;
