all:
	g++ src/main.cpp src/bitboard.cpp src/search.cpp -o Connect4 -lSDL2 -lSDL2_image
clean:
	rm Connect4