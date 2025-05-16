CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -g

all: lab4
	@clear
#	@./tests
	@./lab4

ui: lab4
	@clear
	@./lab4

#tests: tests.o
#	$(CXX) $(CXXFLAGS) tests.o -o tests

lab4: main.o ui.o
	$(CXX) $(CXXFLAGS) main.o ui.o -o lab4

main.o: main.cpp ui.h N-aryTree.hpp errors.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp

ui.o: ui.cpp ui.h N-aryTree.hpp errors.hpp
	$(CXX) $(CXXFLAGS) -c ui.cpp

#tests.o: tests.cpp nary_tree.h ui.h errors.h
#	$(CXX) $(CXXFLAGS) -c tests.cpp


clean:
	rm -f *.o lab4 
