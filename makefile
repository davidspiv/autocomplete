BIN=main
SRCDIR=src
OBJDIR=build
INCDIR=./include/

CXX=g++
OPT=-O0
DEPFLAGS=-MP -MD
CFLAGS=-g -Wall -std=c++17 -fpermissive -I$(INCDIR) $(OPT) $(DEPFLAGS)
CPPFILES=$(wildcard $(SRCDIR)/*.cpp)
OBJECTS=$(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(CPPFILES))
DEPFILES=$(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.d,$(CPPFILES))

all: $(OBJDIR)/$(BIN)

$(OBJDIR)/$(BIN): $(OBJECTS)
	$(CXX) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) -c -o $@ $<

run: all
	@./$(OBJDIR)/$(BIN)

clean:
	rm -rf $(OBJDIR)

-include $(DEPFILES)

.PHONY: all run clean
