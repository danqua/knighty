#ifndef GAME_H
#define GAME_H

#include "tilemap.h"
#include "player.h"
#include <stdint.h>

#define TILE_SIZE 32

typedef enum {
	key_left,
	key_right,
	key_up,
	key_down,
	key_jump,
	key_use,
	key_count
} key_t;

typedef struct {
	uint8_t draw_colliders;
} debug_info_t;

typedef enum {
	state_menu,
	state_game,
	state_quit
} state_t;

typedef struct {
	uint8_t keys[key_count];
	
	debug_info_t debug_info;

	tilemap_t* tilemap;
	player_t player;
	struct SDL_Texture* texture;

	state_t state;
	bool is_running;
	bool next_level;
	
} game_t;


void game_init(game_t* game);
void game_on_key_event(game_t* game, struct SDL_KeyboardEvent* ev);
void game_update(game_t* game, float dt);
void game_draw(struct SDL_Renderer* renderer, const game_t* game);

#endif