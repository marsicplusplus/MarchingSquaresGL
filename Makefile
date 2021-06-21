BIN = MarchingSquaresGL 
CC = g++
FLAGS = -Wall -g
INC = -I ext/GLAD/include $(shell pkg-config --cflags glfw3) -I ext/glm -Iinc
SYS_LIB = -lGL $(shell pkg-config --static --libs glfw3)
SRC = ext/GLAD/src/glad.c src/main.cpp src/shader.cpp

all: 
	${CC} ${FLAGS} -o ${BIN} ${SRC} ${INC} ${SYS_LIB}
