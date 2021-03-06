CXX             := g++
DEBUG_LEVEL     := -g
EXTRA_CCFLAGS   := -Wall -Werror=return-type -Wextra -pedantic
OPT             := -O0
CXXFLAGS        := $(DEBUG_LEVEL) $(EXTRA_CCFLAGS) $(OPT)
LDLIBS          := -lm -lstdc++ -pthread

INC             := include
SRC             := src
PROC            := proc
TEST            := tests
SHEL            := state_machine.o token.o tokenizer.o parser.o shell.o
ALL             := MyCompress MyDecompress ForkCompress PipeCompress ParFork\
                   MinShell MoreShell DupShell ParThread CompareFiles

# $@ targt name
# $< first prerequisite
# $^ all prerequisites

all: $(ALL)
#	rm *.o

MyCompress: MyCompress.o
	$(CXX) -o $@ $^

MyCompress.o: $(PROC)/MyCompress.cpp\
	$(INC)/compression.h\
	$(INC)/timer.h
	$(CXX) $(CXXFLAGS) -c $<

MyDecompress: MyDecompress.o
	$(CXX) -o $@ $^

MyDecompress.o: $(PROC)/MyDecompress.cpp\
	$(INC)/compression.h
	$(CXX) $(CXXFLAGS) -c $<

ForkCompress: ForkCompress.o
	$(CXX) -o $@ $^

ForkCompress.o: $(PROC)/ForkCompress.cpp
	$(CXX) $(CXXFLAGS) -c $<

PipeCompress: PipeCompress.o
	$(CXX) -o $@ $^

PipeCompress.o: $(PROC)/PipeCompress.cpp\
	$(INC)/compression.h
	$(CXX) $(CXXFLAGS) -c $<

ParFork: ParFork.o
	$(CXX) -o $@ $^

ParFork.o: $(PROC)/ParFork.cpp\
	$(INC)/compression.h\
	$(INC)/timer.h
	$(CXX) $(CXXFLAGS) -c $<

MinShell: MinShell.o
	$(CXX) -o $@ $^

MinShell.o: $(PROC)/MinShell.cpp
	$(CXX) $(CXXFLAGS) -c $<

MoreShell: MoreShell.o
	$(CXX) -o $@ $^

MoreShell.o: $(PROC)/MoreShell.cpp
	$(CXX) $(CXXFLAGS) -c $<

DupShell: DupShell.o $(SHEL)
	$(CXX) -o $@ $^

DupShell.o: $(PROC)/DupShell.cpp
	$(CXX) $(CXXFLAGS) -c $<

ParThread: ParThread.o
	$(CXX) -o $@ $^ $(LDLIBS)

ParThread.o: $(PROC)/ParThread.cpp\
	$(INC)/compression.h\
	$(INC)/timer.h
	$(CXX) $(CXXFLAGS) -c $<

CompareFiles: CompareFiles.o
	$(CXX) -o $@ $^

CompareFiles.o: $(PROC)/CompareFiles.cpp
	$(CXX) $(CXXFLAGS) -c $<

test: test.o $(SHEL)
	$(CXX) -o $@ $^

test.o: $(TEST)/test.cpp
	$(CXX) $(CXXFLAGS) -c $<

# SHELL
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

shell.o: ${SRC}/shell.cpp\
	${INC}/shell.h
	$(CXX) $(CXXFLAGS) -c $<

.PHONY: clean

clean:
	rm -f *.o *.out $(ALL)
