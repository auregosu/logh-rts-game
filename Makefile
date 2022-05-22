CC:=g++
FLAGS:=-w -g --std=c++17
LIB:=-lsfml-system -lsfml-window -lsfml-graphics
SRC:=$(wildcard src/*.cpp)
OBJ:=$(SRC:src/%.cpp=obj/%.o)

bin/main: $(OBJ)
	$(CC) $(FLAGS) $^ $(LIB) -o $@

obj/%.o: src/%.cpp
	$(CC) $(FLAGS) -c -I include $^ $(LIB) -o $@

clear:
	rm obj/*.o

#src/%.cpp: include/%.h

#main: $(wildcard obj/*.o)
#	g++ obj/*.o $(FLAGS) -o main
