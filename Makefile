all: collision

collision: collision.c modenonce.c modedictionary.c libabikeccak.c libdictionary.c
	gcc collision.c libabikeccak.c modedictionary.c modenonce.c libdictionary.c -o collision -lkeccak -pthread
