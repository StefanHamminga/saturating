CXX=g++
CXXSTANDARD=c++17
CXXFLAGS=-std=$(CXXSTANDARD) \
		 -Wall -Wextra -Werror -pedantic

all: test

test: test.cpp saturating_types.hpp
	$(CXX) $(CXXFLAGS) -o $@ $<

check: test
	./test
