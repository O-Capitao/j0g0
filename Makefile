CFLAGS = -std=c++17 -I. `pkg-config --cflags-only-I sdl2 sdl2_image yaml-cpp`

LDFLAGS = `pkg-config --libs sdl2 sdl2_image yaml-cpp`

SOURCES = $(wildcard *.cpp) $(wildcard */*.cpp)
OBJS = $(wildcard *.o) $(wildcard */*.o)

game: main.cpp
	g++ $(CFLAGS) -o out.a -g $(SOURCES) $(LDFLAGS) -Wall

clean:
	rm -f out.a $(OBJS)
