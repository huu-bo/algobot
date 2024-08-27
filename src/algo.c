#include <stdlib.h>

#include "algo.h"

struct Algo_game *algo_game__new() {
	struct Algo_game *game = malloc(sizeof(struct Algo_game));
	if (game == NULL) {
		return NULL;
	}

	if (algo_game__array__init(&game->stack) != 0) {
		free(game);
		return NULL;
	}
	for (unsigned int i = 0; i < 2; i++) {
		for (unsigned int j = 0; j <= 11; j++) {
			algo_game__array__append(&game->stack, (struct Algo_game__Tile){TILE_HIDDEN, i, j});
		}
	}

	for (unsigned int i = 0; i < 4; i++) {
		for (unsigned int player_idx = 0; player_idx < 2; player_idx++) {
			struct Algo_game__Tile tile = algo_game__array__pop_random(&game->stack);
			algo_game__array__insert_sort(&game->players[player_idx], tile);
		}
	}

	return game;
}

