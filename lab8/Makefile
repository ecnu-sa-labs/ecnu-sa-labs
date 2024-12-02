# Compiler and linker
CXX = clang++
CXXFLAGS = -std=c++14 -Wall -I./include -I/usr/include/llvm-14 -I/usr/include/c++/7 -I /usr/include/x86_64-linux-gnu/c++/7 -I/usr/include/llvm-c-14
LDFLAGS = -nostdlib++ -L/usr/lib/llvm-14/lib -lLLVM -lpthread /usr/lib/x86_64-linux-gnu/libstdc++.so.6

# File names
SRCS = src/main.cpp \
	src/Executor.cpp \
	src/ExecutionState.cpp \
	src/Expr.cpp \
	src/Time.cpp \
	src/CoreSolver.cpp \
	src/Solver.cpp \
	src/SolverImpl.cpp \
	src/DummySolver.cpp \
	src/TinySolver.cpp

OBJS = $(SRCS:.cpp=.o)
EXEC = miniklee

# Targets and rules
all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $(EXEC) $(LDFLAGS)
	# rm -f $(OBJS)

# Compile .cpp files to .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

SRC ?= ./test/example.c
OUT ?= ./test/example.ll
RES = result*

run:
	clang -emit-llvm -g -O0 -S $(SRC) -o $(OUT) -I./include
	./$(EXEC) $(OUT)

# Clean up
clean:
	rm -rf $(OBJS) $(EXEC) $(OUT) $(RES)

line:
	find . -type f \( -name "*.cpp" -o -name "*.h" \) -exec wc -l {} +

.PHONY: all clean run line
