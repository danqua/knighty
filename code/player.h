#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

typedef enum {
	player_idle,
	player_run
} player_state_t;

typedef struct {
	float x;
	float y;
	float vx;
	float vy;
	int tile;
	int idle_tile;
	int walk_tile;
	int coins;
	bool tile_flipped;
	bool has_key;
	bool on_ground;
	bool is_using;
} player_t;

void update_player(struct game_t* game, float dt);

#endif