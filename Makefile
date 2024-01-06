all: clean game
game:
	g++ src/*.cpp -g -o game `pkg-config --cflags --libs freetype2` -lglfw -lGLU -lGL -lGLEW 
clean:
	rm game