#include "player.h"
#include "game.h"
#include "tilemap.h"
#include <stdbool.h>
#include <math.h>
#include <stdio.h>

void on_tile_collision(game_t* game, int* tileid, tile_type_t type)
{
	if (type == tile_type_coin)
	{
		*tileid = -1;
		game->player.coins++;
	}
	else if (type == tile_type_key)
	{
		*tileid = -1;
		game->player.has_key = 1;
	}
	else if (type == tile_type_chest && game->player.is_using)
	{
		*tileid = *tileid + 1;
		game->player.coins += 100;
	}
	else if (type == tile_type_door)
	{
		if (game->player.has_key) {
			*tileid = *tileid + 1;
			game->next_level = 1;
			game->player.has_key = 0;
		}
	}
}

void update_player(game_t* game, float dt)
{
	bool hit = false;

	player_t* p = &game->player;

	float margin_x = 0.1f;
	float margin_y = 0.1f;

	p->tile = p->idle_tile;

	if (p->vx > 0) {
		p->tile_flipped = false;
	}
	else if (p->vx < 0) {
		p->tile_flipped = true;
	}

	// first check map collision on horizontal axes
	if (p->vx != 0) {
		
		p->tile = p->walk_tile;
		p->x += p->vx * dt;

		int x = (int)(p->vx > 0.0f ? p->x + 1.0f : p->x);
		int y = (int)(p->y + margin_y);

		tilemap_layer_t* layer = game->tilemap->layers;
		while (layer != &game->tilemap->layers[game->tilemap->layer_count]) {
			int tileid = layer->tiles[x + y * game->tilemap->width];
			if (tileid != -1) {
				tile_t* tile = &game->tilemap->tileset.tiles[tileid];
				if (tile->type == tile_type_collider) {
					hit = true;
					break;
				}
			}
			layer++;
		}

		y = (int)(p->y + 1.0f - margin_y);

		layer = game->tilemap->layers;
		while (layer != &game->tilemap->layers[game->tilemap->layer_count]) {
			int tileid = layer->tiles[x + y * game->tilemap->width];
			if (tileid != -1) {
				tile_t* tile = &game->tilemap->tileset.tiles[tileid];
				if (tile->type == tile_type_collider) {
					hit = true;
					break;
				}
			}
			layer++;
		}

		if (hit) {
			p->x = (p->vx > 0.0f ? (float)x - 1.0f : (float)(x + 1));
			p->vx = 0.0f;
		}
	}

	// apply gravity
	{
		p->on_ground = false;

		int x = (int)(p->x + margin_x);
		int y = (int)(p->y + 1.0f + margin_y);

		tilemap_layer_t* layer = game->tilemap->layers;
		while (layer != &game->tilemap->layers[game->tilemap->layer_count]) {
			int tileid = layer->tiles[x + y * game->tilemap->width];
			if (tileid != -1) {
				tile_t* tile = &game->tilemap->tileset.tiles[tileid];
				if (tile->type == tile_type_collider) {
					p->on_ground = true;
					break;
				}
			}
			layer++;
		}

		x = (int)(p->x + 1.0f - margin_x);
		
		layer = game->tilemap->layers;
		while (layer != &game->tilemap->layers[game->tilemap->layer_count]) {
			int tileid = layer->tiles[x + y * game->tilemap->width];
			if (tileid != -1) {
				tile_t* tile = &game->tilemap->tileset.tiles[tileid];
				if (tile->type == tile_type_collider) {
					p->on_ground = true;
					break;
				}
			}
			layer++;
		}

		if (!p->on_ground) {
			p->vy += 150.0f * dt;
		}
		else {
			// apply some friction
			p->vx *= 0.9f;
			if (fabsf(p->vx) < 0.0001f) {
				p->vx = 0.0f;
			}
		}
	}

	// then check map collision on vertical axes
	if (p->vy != 0) {
		
		hit = false;
		p->y += p->vy * dt;

		int x = (int)(p->x + margin_x);
		int y = (int)(p->vy > 0.0f ? p->y + 1.0f : p->y);

		tilemap_layer_t* layer = game->tilemap->layers;
		while (layer != &game->tilemap->layers[game->tilemap->layer_count]) {
			int tileid = layer->tiles[x + y * game->tilemap->width];
			if (tileid != -1) {
				tile_t* tile = &game->tilemap->tileset.tiles[tileid];
				if (tile->type == tile_type_collider) {
					hit = true;
					break;
				}
			}
			layer++;
		}

		x = (int)(p->x + 1.0f - margin_x);

		layer = game->tilemap->layers;
		while (layer != &game->tilemap->layers[game->tilemap->layer_count]) {
			int tileid = layer->tiles[x + y * game->tilemap->width];
			if (tileid != -1) {
				tile_t* tile = &game->tilemap->tileset.tiles[tileid];
				if (tile->type == tile_type_collider) {
					hit = true;
					break;
				}
			}
			layer++;
		}

		if (hit) {
			p->y = (p->vy > 0.0f ? (float)y - 1.0f : (float)(y + 1));
			p->vy = 0.0f;
		}
	}

	// Check collision with anything other than colliders
	int mx = (int)(p->x + 0.5f);
	int my = (int)(p->y + 0.5f);

	tilemap_layer_t* layer = game->tilemap->layers;
	while (layer != &game->tilemap->layers[game->tilemap->layer_count]) {
		int tileid = layer->tiles[mx + my * game->tilemap->width];
		if (tileid != -1) {
			tile_t* tile = &game->tilemap->tileset.tiles[tileid];
			if (tile->type != tile_type_collider) {
				on_tile_collision(game, &layer->tiles[mx + my * game->tilemap->width], tile->type);
			}
		}
		layer++;
	}
}