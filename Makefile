CXX   ?= g++
EXE = dc
CXX_WARNS_FLAGS=-Wall -Wcast-align -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wdisabled-optimization -Wextra -Wfloat-equal -Wformat=2 -Winit-self -Wlogical-op -Wmissing-include-dirs -Wnoexcept -Wno-unused -Woverloaded-virtual -Wpointer-arith -Wredundant-decls -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wsuggest-override -Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code -Wunsafe-loop-optimizations -Wunused -Wzero-as-null-pointer-constant
MILD_CXXFLAGS=-Wno-switch-default -Wno-conversion -Wno-zero-as-null-pointer-constant
CXXSTD = -std=c++11
CXXFLAGS = -O0 $(CXX_WARNS_FLAGS) $(CXXSTD)

SOURCE = src
BIN = bin
INCLUDE = include
SOBJ =  parser lexer

FILES = $(wildcard $(SOURCE)/*.cpp)

OBJS  = $(patsubst $(SOURCE)/%.cpp,$(BIN)/%.o, $(FILES))

CLEANLIST =  DCParser.output DCParser.tab.cc $(INCLUDE)/bin.tab.hh $(INCLUDE)/DCParser.tab.hh $(INCLUDE)/location.hh $(INCLUDE)/position.hh $(INCLUDE)/stack.hh dc

.PHONY: all
all: parser lexer $(OBJS) dc

dc: $(FILES)
	$(CXX) $(CXXFLAGS) -o $(EXE) -I$(INCLUDE) $(OBJS) $(BIN)/parser.o $(BIN)/lexer.o $(LIBS)

$(BIN)/%.o: $(SOURCE)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c $< -o $@


parser: $(SOURCE)/DCParser.yy
	mkdir -p $(BIN)
	bison -d -v $<
	mv *.hh $(INCLUDE)
	$(CXX) $(CXXFLAGS) $(MILD_CXXFLAGS) -c -o $(BIN)/parser.o -I$(INCLUDE) DCParser.tab.cc

lexer: $(SOURCE)/DCLexer.l
	mkdir -p $(BIN)
	flex --outfile=$(BIN)/DCLexer.yy.cc  $<
	$(CXX)  $(CXXFLAGS) $(MILD_CXXFLAGS) -c $(BIN)/DCLexer.yy.cc -I$(INCLUDE) -o $(BIN)/lexer.o

# .PHONY: test
# test:
# 	test/test0.pl

.PHONY: clean
clean:
	rm -rf $(CLEANLIST)
	rm -rf $(BIN)
