CFLAGS = -std=c++17 -I. `pkg-config --cflags-only-I sdl2 sdl2_image`

LDFLAGS = `pkg-config --libs sdl2 sdl2_image`

SOURCES = $(wildcard *.cpp) $(wildcard */*.cpp)
OBJS = $(wildcard *.o) $(wildcard */*.o)

game: main.cpp
	g++ $(CFLAGS) -o out.a -g $(SOURCES) $(LDFLAGS) -Wall

clean:
	rm -f out.a $(OBJS)
