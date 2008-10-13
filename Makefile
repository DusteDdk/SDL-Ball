CC=g++
CFLAGS=-c -Wall
#append -lwiiuse if compiling WITH_WIIUSE
#remove -lSDL_mixer if NOT compiling WITH_SOUND
LDFLAGS=-lGL -lGLU -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer -lwiiuse

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
