build:
	g++ App.cpp -std=c++17 -pthread -o App $(shell pkg-config --cflags --libs sdl2 SDL2_ttf)

run:
	./App
