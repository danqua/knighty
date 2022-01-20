#ifndef TILEMAP_H
#define TILEMAP_H

#define MAX_ANIMATION_FRAMES 8
#define MAX_IMAGE_PATH 64
#define MAX_LAYER_NAME 32
#define MAX_LAYER_TILES 1024

typedef enum {
	tile_type_unknown = 0,
	tile_type_collider,
	tile_type_coin,
	tile_type_key,
	tile_type_door,
	tile_type_chest,
	tile_type_spawn
} tile_type_t;

typedef struct {
	int tile;
	int duration;
} tile_animation_frame_t;

typedef struct {
	int id;
	tile_type_t type;
	int frame_count;
	float frame_timer;
	int current_frame;
	tile_animation_frame_t frames[MAX_ANIMATION_FRAMES];
} tile_t;

typedef struct {
	int columns;
	int image_width;
	int image_height;
	int tile_width;
	int tile_height;
	int tile_count;
	tile_t* tiles;
	char image_path[MAX_IMAGE_PATH];
} tileset_t;

typedef enum {
	layer_type_background,
	layer_type_midground,
	layer_type_foreground,

	layer_type_count
} layer_type_t;

typedef struct {
	layer_type_t type;
	int tile_count;
	int* tiles;
} tilemap_layer_t;

typedef struct {
	int width;
	int height;
	int tile_width;
	int tile_height;
	int layer_count;
	tileset_t tileset;
	tilemap_layer_t layers[layer_type_count];
} tilemap_t;

tilemap_t* create_tilemap_from_file(const char* filename);
void destroy_tilemap(tilemap_t* tilemap);
void update_tilemap(tilemap_t* tilemap, float dt);
tilemap_layer_t* find_tilemap_layer(tilemap_t* tilemap, layer_type_t type);

#endif