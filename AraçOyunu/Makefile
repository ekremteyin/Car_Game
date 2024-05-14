# all: main

# main: main.c
# 	gcc -Isrc/include -Lsrc/lib -o main main.c -lmingw32 -lSDL2 

# clean:
# 	rm -f main
# gcc -Isrc/include -Lsrc/lib -o main main.c background.c -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image


# all: main

# main: main.c
# 	gcc main.c background.c -o main -Isrc/include -Lsrc/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf


# clean:
# 	rm -f main

all: main

main: main.c
	gcc main.c -o main -Isrc/include -Lsrc/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -mwindows

clean:
	rm -f main
