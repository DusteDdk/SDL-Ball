DATADIR?=themes/

#append -DWITH_WIIUSE to compile with WIIUSE support!
#append -DNOSOUND to compile WITHOUT sound support
CC=g++ -DDATADIR="\"$(DATADIR)\""

CFLAGS+=-c -Wall `sdl-config --cflags`

#append -lwiiuse to compile with WIIUSE support
#remove -lSDL_mixer if compiling with -DNOSOUND
LIBS+=-lGL -lGLU `sdl-config --libs` -lSDL_image -lSDL_ttf -lSDL_mixer

SOURCES=main.cpp 
OBJECTS=$(SOURCES:.cpp=.o)

EXECUTABLE=sdl-ball

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o sdl-ball

remove:
	rm -R ~/.config/sdl-ball
	
