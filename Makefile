CXX             := g++
DEBUG_LEVEL     := -g
EXTRA_CCFLAGS   := -Wall -Werror=return-type -Wextra -pedantic
CXXFLAGS        := $(DEBUG_LEVEL) $(EXTRA_CCFLAGS)

INC             := include
SRC				:= src
PROC            := proc
TEST            := tests
PARSER          := state_machine.o token.o tokenizer.o parser.o
ALL             := MyCompress MyDecompress ForkCompress MinShell MoreShell\
                   ParFork

# $@ targt name
# $< first prerequisite
# $^ all prerequisites

all: $(ALL)
#	rm *.o

MyCompress: MyCompress.o
	$(CXX) -o $@ $^

MyCompress.o: $(PROC)/MyCompress.cpp
	$(CXX) $(CXXFLAGS) -c $<

MyDecompress: MyDecompress.o
	$(CXX) -o $@ $^

MyDecompress.o: $(PROC)/MyDecompress.cpp
	$(CXX) $(CXXFLAGS) -c $<

ForkCompress: ForkCompress.o
	$(CXX) -o $@ $^

ForkCompress.o: $(PROC)/ForkCompress.cpp
	$(CXX) $(CXXFLAGS) -c $<

MinShell: MinShell.o
	$(CXX) -o $@ $^

MinShell.o: $(PROC)/MinShell.cpp
	$(CXX) $(CXXFLAGS) -c $<

MoreShell: MoreShell.o
	$(CXX) -o $@ $^

MoreShell.o: $(PROC)/MoreShell.cpp
	$(CXX) $(CXXFLAGS) -c $<

ParFork: ParFork.o
	$(CXX) -o $@ $^

ParFork.o: $(PROC)/ParFork.cpp
	$(CXX) $(CXXFLAGS) -c $<

test: test.o $(PARSER)
	$(CXX) -o $@ $^

test.o: $(TEST)/test.cpp
	$(CXX) $(CXXFLAGS) -c $<

# PARSER
state_machine.o: ${SRC}/state_machine.cpp\
	${INC}/state_machine.h
	$(CXX) $(CXXFLAGS) -c $<

token.o: ${SRC}/token.cpp\
	${INC}/token.h
	$(CXX) $(CXXFLAGS) -c $<

tokenizer.o: ${SRC}/tokenizer.cpp\
	${INC}/tokenizer.h
	$(CXX) $(CXXFLAGS) -c $<

parser.o: ${SRC}/parser.cpp\
	${INC}/parser.h
	$(CXX) $(CXXFLAGS) -c $<

.PHONY: clean

clean:
	rm -f *.o *.out $(ALL)
