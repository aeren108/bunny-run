all: clean game
game:
	g++ src/*.cpp -o game -lglfw -lpthread -lX11 -ldl -lXrandr -lGLEW -lGL -DGL_SILENCE_DEPRECATION -DGLM_ENABLE_EXPERIMENTAL -I.
clean:
	rm game