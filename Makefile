T := main

all:
	g++ -g $(T).cpp planet.cpp player.cpp utils.cpp glad.c -o $(T) -Iinclude -Llib -lglfw3 -lgdi32 -lopengl32 -lassimp