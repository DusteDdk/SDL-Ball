DATADIR?=data/

CC=g++ -DDATADIR="\"$(DATADIR)\""

#append -DWITH_WIIUSE to compile with WIIUSE support!
CFLAGS+=-g -c -Wall

#append -lwiiuse to compile with WIIUSE support
#remove -lSDL_mixer if NOT compiling WITH_SOUND
LDFLAGS+=-lGL -lGLU -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer

SOURCES=main.cpp 
OBJECTS=$(SOURCES:.cpp=.o)

EXECUTABLE=sdl-ball

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o sdl-ball

remove:
	rm -R ~/.config/sdl-ball
