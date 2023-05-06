all:
	g++ main.cpp glad.c -o main -Iinclude -Llib -lglfw3 -lgdi32 -lopengl32 -lassimp