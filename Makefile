CC=g++

#append -DWITH_WIIUSE to compile with WIIUSE support!
CFLAGS=-c -Wall

#append -lwiiuse to compile with WIIUSE support
#remove -lSDL_mixer if NOT compiling WITH_SOUND
LDFLAGS=-lGL -lGLU -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer

SOURCES=main.cpp 
OBJECTS=$(SOURCES:.cpp=.o)

EXECUTABLE=sdl-ball

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o

remove:
	rm -R ~/.config/sdl-ball
