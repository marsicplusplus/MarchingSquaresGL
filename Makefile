BIN = MarchingSquaresGL 
CC = g++
FLAGS = -Wall -g
INC = -I ext/GLAD/include $(shell pkg-config --cflags glfw3) -I ext/glm
SYS_LIB = -lGL $(shell pkg-config --static --libs glfw3)
SRC = ext/GLAD/src/glad.c src/main.cpp src/shader.cpp
OUT_DIR = build/



all: 
	@mkdir -p ${OUT_DIR}
	${CC} ${FLAGS} -o ${OUT_DIR}${BIN} ${SRC} ${INC} ${SYS_LIB}

clean:
	@rm ${OUT_DIR}${BIN}
