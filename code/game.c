#include "game.h"
#include "video.h"
#include "menu.h"
#include <SDL.h>
#include <string.h>

int current_level = 0;
const int level_count = 3;

const char* level_files[] = {
	"level_1.json",
	"level_2.json",
	"level_3.json"
};

void reset_player(game_t* game)
{
	game->player.x = 2.0f;
	game->player.y = 15.0f;
	game->player.vx = 0.0f;
	game->player.vy = 0.0f;
	game->player.idle_tile = 96;
	game->player.walk_tile = 98;
	game->player.tile = game->player.idle_tile;
	game->player.tile_flipped = false;

	tilemap_layer_t* layer = find_tilemap_layer(game->tilemap, layer_type_foreground);
	
	for (int y = 0; y < game->tilemap->height; y++) {
		for (int x = 0; x < game->tilemap->width; x++) {
			int tileid = layer->tiles[x + y * game->tilemap->width];
			if (tileid != -1) {
				tile_t* tile = &game->tilemap->tileset.tiles[tileid];
				if (tile->type == tile_type_spawn) {
					game->player.x = (float)x;
					game->player.y = (float)y;
					layer->tiles[x + y * game->tilemap->width] = -1;
					return;
				}
			}
		}
	}
}

void game_next_level(game_t* game)
{
	if (game->tilemap) {
		destroy_tilemap(game->tilemap);
		free(game->tilemap);
	}
	if (game->texture) {
		SDL_DestroyTexture(game->texture);
	}

	game->tilemap = create_tilemap_from_file(level_files[current_level]);
	game->texture = load_texture_from_file(game->tilemap->tileset.image_path);
	game->next_level = 0;
	reset_player(game);

	current_level = (current_level + 1) % level_count;
}

void reload_level(game_t* game)
{
	int level = current_level - 1;
	destroy_tilemap(game->tilemap);
	free(game->tilemap);
	game->tilemap = create_tilemap_from_file(level_files[level]);
}

void game_init(game_t* game)
{
	memset(game, 0, sizeof(game_t));
	game->state = state_menu;
	game->debug_info.draw_colliders = 0;
	game->next_level = 0;
	game->is_running = true;

	game_next_level(game);
	reset_player(game);
}

void game_on_key_event(game_t* game, struct SDL_KeyboardEvent* ev)
{
	switch (ev->keysym.scancode) {
		case SDL_SCANCODE_LEFT:		game->keys[key_left]	= ev->type == SDL_KEYDOWN ? 1 : 0; break;
		case SDL_SCANCODE_RIGHT:	game->keys[key_right]	= ev->type == SDL_KEYDOWN ? 1 : 0; break;
		case SDL_SCANCODE_UP:		game->keys[key_up]		= ev->type == SDL_KEYDOWN ? 1 : 0; break;
		case SDL_SCANCODE_DOWN:		game->keys[key_down]	= ev->type == SDL_KEYDOWN ? 1 : 0; break;
		case SDL_SCANCODE_X:		game->keys[key_jump]	= ev->type == SDL_KEYDOWN ? 1 : 0; break;
		case SDL_SCANCODE_Z:		//game->keys[key_use]		= ev->type == SDL_KEYDOWN ? 1 : 0; break;
		{
			if (ev->type == SDL_KEYDOWN) {
				game->player.is_using = true;
			}
			break;
		}
		case SDL_SCANCODE_F1:
			if (ev->type == SDL_KEYDOWN) {
				game->debug_info.draw_colliders = !game->debug_info.draw_colliders;
			}
			break;
		case SDL_SCANCODE_F5:
			reload_level(game);
			break;
	}
	menu_on_key_event(game, ev);
}

void game_update(game_t* game, float dt)
{
	if (game->state == state_game) {
		if (game->next_level) {
			game_next_level(game);
		}

		float speed = 10.0f;
		float jump_force = 30.0f;
		if (game->keys[key_right])
			game->player.vx = speed;
		if (game->keys[key_left])
			game->player.vx = -speed;
		if (game->keys[key_jump] && game->player.on_ground) {
			game->player.vy -= jump_force;
			game->keys[key_jump] = 0;
		}

		update_tilemap(game->tilemap, dt);
		update_player(game, dt);
		game->player.is_using = false;
	}
	else if (game->state == state_menu) {
		menu_update(dt);
	}
	else if (game->state == state_quit) {
		game->is_running = false;
	}
}

void draw_tile(int x, int y, int tileid, tileset_t* tileset, SDL_Texture* texture, bool flipped)
{
	if (tileset->tiles[tileid].frame_count > 0) {
		tileid = tileset->tiles[tileid].frames[tileset->tiles[tileid].current_frame].tile;
	}

	tile_t* tile = &tileset->tiles[tileid];

	SDL_Rect src_rect = {
					(int)(tileid % tileset->columns) * tileset->tile_width,
					(int)(tileid / tileset->columns) * tileset->tile_width,
					tileset->tile_width,
					tileset->tile_height
	};

	SDL_Rect dst_rect = { x, y, TILE_SIZE, TILE_SIZE };

	SDL_RenderCopyEx(renderer, texture, &src_rect, &dst_rect, 0, 0, flipped ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

void game_draw(struct SDL_Renderer* renderer, const game_t* game)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	if (game->state == state_game) {
		tilemap_t* tilemap = game->tilemap;
		tileset_t* tileset = &tilemap->tileset;

		// draw tiles
		for (int y = 0; y < tilemap->height; y++) {
			for (int x = 0; x < tilemap->width; x++) {
				for (int i = 0; i < tilemap->layer_count; i++) {
					int tileid = tilemap->layers[i].tiles[x + y * tilemap->width];
					if (tileid != -1) {

						if (tileset->tiles[tileid].frame_count > 0) {
							tileid = tileset->tiles[tileid].frames[tileset->tiles[tileid].current_frame].tile;
						}

						tile_t* tile = &tileset->tiles[tileid];

						SDL_Rect src_rect = {
							(int)(tileid % tileset->columns) * tileset->tile_width,
							(int)(tileid / tileset->columns) * tileset->tile_width,
							tileset->tile_width,
							tileset->tile_height
						};
						SDL_Rect dst_rect = {
							(int)(x * TILE_SIZE),
							(int)(y * TILE_SIZE),
							TILE_SIZE,
							TILE_SIZE
						};

						SDL_RenderCopy(renderer, game->texture, &src_rect, &dst_rect);
					}
				}
			}
		}

		// draw colliders
		if (game->debug_info.draw_colliders) {
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
			for (int y = 0; y < tilemap->height; y++) {
				for (int x = 0; x < tilemap->width; x++) {
					for (int i = 0; i < tilemap->layer_count; i++) {
						int tileid = tilemap->layers[i].tiles[x + y * tilemap->width];
						tile_t* tile = &tileset->tiles[tileid];
						if (tile->type == tile_type_collider) {
							SDL_Rect rect = { x * 32, y * 32, 32, 32 };
							SDL_SetRenderDrawColor(renderer, 255, 0, 0, 127);
							SDL_RenderFillRect(renderer, &rect);
						}
					}
				}
			}
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
		}

		// draw player
		{
			int px = (int)(game->player.x * TILE_SIZE);
			int py = (int)(game->player.y * TILE_SIZE);
			draw_tile(px, py, game->player.tile, &game->tilemap->tileset, game->texture, game->player.tile_flipped);
		}

		// draw hud
		{
			player_t* p = &game->player;
			static int key_tile = 20;
			int x = 16;
			int y = WINDOW_HEIGHT - 40;


			char key_str[4] = { 'N', 'O' };

			if (p->has_key) {
				strcpy(key_str, "YES");
			}

			draw_text(x, y, "COINS: %2d KEY: %s", p->coins, key_str);
		}
	}
	else if (game->state == state_menu) {
		menu_draw();
	}
	
	SDL_RenderPresent(renderer);
}