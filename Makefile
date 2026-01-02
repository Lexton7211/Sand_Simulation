build:
	g++ App.cpp -std=c++17 -o App $(shell pkg-config --cflags --libs sdl2)

run:
	./App
