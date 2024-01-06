all: clean game
game:
	g++ src/*.cpp -o main `pkg-config --cflags --libs freetype2` -lglfw -lGLU -lGL -lGLEW 
clean:
	rm main