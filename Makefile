T := main

all:
	g++ $(T).cpp glad.c -o $(T) -Iinclude -Llib -lglfw3 -lgdi32 -lopengl32 -lassimp