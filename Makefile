CXX             := g++
DEBUG_LEVEL     := -g
EXTRA_CCFLAGS   := -Wall -Werror=return-type -Wextra -pedantic
CXXFLAGS        := $(DEBUG_LEVEL) $(EXTRA_CCFLAGS)

INC             := include
SRC             := src
ALL				:= MyCompress MyDecompress ForkCompress MinShell MoreShell

# $@ targt name
# $< first prerequisite
# $^ all prerequisites

all: $(ALL)
#	rm *.o

MyCompress: MyCompress.o
	$(CXX) -o $@ $^

MyCompress.o: $(SRC)/MyCompress.cpp
	$(CXX) $(CXXFLAGS) -c $<

MyDecompress: MyDecompress.o
	$(CXX) -o $@ $^

MyDecompress.o: $(SRC)/MyDecompress.cpp
	$(CXX) $(CXXFLAGS) -c $<

ForkCompress: ForkCompress.o
	$(CXX) -o $@ $^

ForkCompress.o: $(SRC)/ForkCompress.cpp
	$(CXX) $(CXXFLAGS) -c $<

MinShell: MinShell.o
	$(CXX) -o $@ $^

MinShell.o: $(SRC)/MinShell.cpp
	$(CXX) $(CXXFLAGS) -c $<

MoreShell: MoreShell.o
	$(CXX) -o $@ $^

MoreShell.o: $(SRC)/MoreShell.cpp
	$(CXX) $(CXXFLAGS) -c $<

.PHONY: clean

clean:
	rm -f *.o *.out $(ALL)
