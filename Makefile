all:
	make compile_test 
	make run_test
	make compile
	make run

compile: src/main.cpp src/slack-stealing.cpp src/slack-stealing.h
	g++ -o main.out src/main.cpp src/slack-stealing.cpp

run:
	./main.out

compile_test: src/slack-stealing.cpp src/slack-stealing.h src/doctest.h
	g++ -o test.out src/test_suite.cpp src/slack-stealing.cpp
	
run_test:
	./test.out
