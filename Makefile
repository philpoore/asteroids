
compile:
	g++ src/main.cpp \
		-DWITH_ALSA \
		deps/soloud20200207/src/core/*.cpp \
		deps/soloud20200207/src/backend/alsa/* \
		deps/soloud20200207/src/audiosource/wav/* \
		-Ideps/soloud20200207/include \
		$$(pkg-config --cflags --libs glfw3 gl alsa) -lpthread \
		-o asteroids