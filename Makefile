build:
    make compile_test
    make compile

compile: src/*[!.test].cpp src/*[!doctest].hpp
    g++ -o main.out src/*[!.test].cpp

run:
    ./main.out

compile_test: src/*[!main].cpp src/*.hpp
    g++ -o test.out src/*[!main].cpp

check:
    ./test.out

g++ -o main.out src/utils/*.cpp src/tasks/*.cpp src/schedulers/*.cpp src/main.cpp

sou burro