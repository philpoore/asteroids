
compile:
	g++ src/main.cpp $$(pkg-config --cflags --libs glfw3 gl glx) -o asteroids