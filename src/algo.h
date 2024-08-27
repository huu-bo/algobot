#ifndef ALGO_H_
#define ALGO_H_

#include <stdlib.h>

struct Algo_game__Tile {
	enum Algo_game__Tile__Shown {
		TILE_HIDDEN,
		TILE_SHOWN
	} shown;
	enum Algo_game__Tile__Colour {
		COLOUR_BLACK,
		COLOUR_WHITE
	} colour;
	unsigned int value;
};

struct Algo_game__Array {
	struct Algo_game__Tile *tiles;
	size_t length;
};

// Returns 1 on failure, 0 otherwise.
int algo_game__array__init(struct Algo_game__Array *);

void algo_game__array__shuffle(struct Algo_game__Array *);
// Undefined behaviour with empty array
struct Algo_game__Tile algo_game__array__pop_random(struct Algo_game__Array *);
void algo_game__array__insert_sort(struct Algo_game__Array *, struct Algo_game__Tile);
void algo_game__array__append(struct Algo_game__Array *, struct Algo_game__Tile);

struct Algo_game {
	// The Stack is not sorted so you should only pull tiles using algo_game__array__pop_random
	struct Algo_game__Array stack;
	struct Algo_game__Array players[2];
};

struct Algo_game *algo_game__new();
void algo_game__free(struct Algo_game *);

#endif // ALGO_H_
