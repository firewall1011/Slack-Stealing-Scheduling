MAIN_TARGETS := $(wildcard src/*/*.cpp src/*.cpp)
TEST_TARGETS := $(wildcard src/*/*.cpp test/*.cpp test/*/*.cpp)

INCLUDE_FILES = -Isrc
build:
	make compile_test
	make compile

compile:
	g++ -o main.out $(INCLUDE_FILES) $(MAIN_TARGETS)

run:
	./main.out

compile_test: 
	g++ -o test.out $(INCLUDE_FILES) $(TEST_TARGETS) 

check:
	./test.out

#g++ -o main.out src/utils/*.cpp src/tasks/*.cpp src/schedulers/*.cpp src/main.cpp

#sou burro