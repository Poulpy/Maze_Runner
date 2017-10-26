#changer le nom des deux lignes suivantes si nécessaire
SOURCE = main.c
OUTPUT = resultat
LIB = -lm -lSDL -lSDL_ttf
OPT = -Wall -g


SDL_CFLAGS = $(shell sdl-config --cflags)
SDL_LDFLAGS = $(shell sdl-config --libs)


all: $(OUTPUT)

clean: $(OUTPUT)
	rm $(OUTPUT) *.o; rm ../../lib/*.o

$(OUTPUT): ../../lib/libgraphique.o source.o charge_labyrinthe.o
	@gcc ../../lib/libgraphique.o charge_labyrinthe.o source.o -o $(OUTPUT)  $(SDL_CFLAGS) $(SDL_LDFLAGS) $(LIB)

charge_labyrinthe.o: charge_labyrinthe.c
	@gcc -c charge_labyrinthe.c -o charge_labyrinthe.o 


../../lib/libgraphique.o: ../../lib/libgraphique.c
	@gcc -c ../../lib/libgraphique.c -o ../../lib/libgraphique.o 

source.o: $(SOURCE) charge_labyrinthe.h
	@gcc $(OPT) -c $(SOURCE) -o source.o






