CXX = g++
CXXFLAGS = -std=c++11 -pthread -O2 -Wall
LDFLAGS = -pthread

# Object files
OBJS = main.o Mapper.o Reducer.o Sortable.o TeraSortItems.o Thread.o ThreadManager.o sortingalgorithms.o globalcutpoints.o

# Executable
TARGET = terasort

# Benchmark executable
BENCHMARK = benchmark

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

main.o: main.cpp TeraSort.h Thread.h ThreadManager.h globalcutpoints.h sortingalgorithms.h
	$(CXX) $(CXXFLAGS) -c main.cpp

Mapper.o: Mapper.cpp TeraSort.h Thread.h ThreadManager.h globalcutpoints.h sortingalgorithms.h
	$(CXX) $(CXXFLAGS) -c Mapper.cpp

Reducer.o: Reducer.cpp TeraSort.h Thread.h ThreadManager.h
	$(CXX) $(CXXFLAGS) -c Reducer.cpp

Sortable.o: Sortable.cpp TeraSort.h
	$(CXX) $(CXXFLAGS) -c Sortable.cpp

TeraSortItems.o: TeraSortItems.cpp TeraSort.h Thread.h
	$(CXX) $(CXXFLAGS) -c TeraSortItems.cpp

Thread.o: Thread.cpp Thread.h
	$(CXX) $(CXXFLAGS) -c Thread.cpp

ThreadManager.o: ThreadManager.cpp ThreadManager.h Thread.h
	$(CXX) $(CXXFLAGS) -c ThreadManager.cpp

sortingalgorithms.o: sortingalgorithms.cpp sortingalgorithms.h TeraSort.h
	$(CXX) $(CXXFLAGS) -c sortingalgorithms.cpp

globalcutpoints.o: globalcutpoints.cpp globalcutpoints.h
	$(CXX) $(CXXFLAGS) -c globalcutpoints.cpp

benchmark: benchmark.cpp
	$(CXX) $(CXXFLAGS) -o $(BENCHMARK) benchmark.cpp $(LDFLAGS)

clean:
	rm -f $(OBJS) $(TARGET) $(BENCHMARK) *.o

.PHONY: all clean benchmark
