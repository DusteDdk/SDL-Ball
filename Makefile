DATADIR?=themes/

#append -DWITH_WIIUSE to compile with WIIUSE support!
#append -DNOSOUND to compile WITHOUT sound support
CXX?=g++

CXXFLAGS+=-Wall `sdl-config --cflags` -DDATADIR="\"$(DATADIR)\""

#append -lwiiuse to compile with WIIUSE support
#remove -lSDL_mixer if compiling with -DNOSOUND
LIBS+=-lGL -lGLU `sdl-config --libs` -lSDL_image -lSDL_ttf -lSDL_mixer

SOURCES=main.cpp 
OBJECTS=$(SOURCES:.cpp=.o)

EXECUTABLE=sdl-ball

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	rm -f *.o sdl-ball

remove:
	rm -R ~/.config/sdl-ball
	
