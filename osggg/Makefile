DATADIR?=./

CC=g++ -DDATADIR="\"$(DATADIR)\""

CFLAGS+=-Wall `sdl-config --cflags`

LIBS+=-lGL `sdl-config --libs` -lSDL_ttf -lSDL_image -lSDL_mixer

SOURCES=main.cpp text.cpp sound.cpp
OBJECTS=$(SOURCES:.c=.o)

EXECUTABLE=osgg

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o $(EXECUTABLE)
