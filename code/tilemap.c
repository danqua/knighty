#define _CRT_SECURE_NO_WARNINGS
#include "tilemap.h"
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <json.h>

void string_to_lowercase(char* buffer, size_t buffer_size, const char* str)
{
	for (size_t i = 0; i < buffer_size && i < strlen(str); i++) {
		buffer[i] = tolower(str[i]);
	}
}

int create_tileset_from_file(tileset_t* tileset, const char* filename)
{
	FILE* fp = fopen(filename, "rb");

	if (!fp) {
		printf("Failed to open file!\n");
		return 0;
	}

	size_t filesize = 0;
	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	rewind(fp);
	char* buffer = malloc(filesize + 1);
	assert(buffer);
	buffer[filesize] = 0;
	fread(buffer, 1, filesize, fp);
	fclose(fp);

	struct json_value_s* root = json_parse(buffer, strlen(buffer));
	assert(root->type == json_type_object);

	struct json_object_s* object = (struct json_object_s*)root->payload;
	assert(object);

	struct json_object_element_s* element = object->start;
	assert(element);

	while (element) {
		if (strcmp(element->name->string, "columns") == 0) {
			struct json_number_s* n = json_value_as_number(element->value);
			tileset->columns = atoi(n->number);
		}
		else if (strcmp(element->name->string, "image") == 0) {
			struct json_string_s* str = json_value_as_string(element->value);
			strcpy(tileset->image_path, str->string);
		}
		else if (strcmp(element->name->string, "imagewidth") == 0) {
			struct json_number_s* n = json_value_as_number(element->value);
			tileset->image_width = atoi(n->number);
		}
		else if (strcmp(element->name->string, "imageheight") == 0) {
			struct json_number_s* n = json_value_as_number(element->value);
			tileset->image_height = atoi(n->number);
		}
		else if (strcmp(element->name->string, "tilecount") == 0) {
			struct json_number_s* n = json_value_as_number(element->value);
			tileset->tile_count = atoi(n->number);
			// allocate tiles
			tileset->tiles = malloc(sizeof(tile_t) * tileset->tile_count);
			assert(tileset->tiles);
			memset(tileset->tiles, 0, sizeof(tile_t) * tileset->tile_count);
		}
		else if (strcmp(element->name->string, "tilewidth") == 0) {
			struct json_number_s* n = json_value_as_number(element->value);
			tileset->tile_width = atoi(n->number);
		}
		else if (strcmp(element->name->string, "tileheight") == 0) {
			struct json_number_s* n = json_value_as_number(element->value);
			tileset->tile_height = atoi(n->number);
		}
		else if (strcmp(element->name->string, "tiles") == 0) {
			struct json_array_s* array = (struct json_array_s*)element->value->payload;
			struct json_array_element_s* item = array->start;

			// Parse tiles
			while (item) {
				struct json_object_s* obj = json_value_as_object(item->value);
				struct json_object_element_s* el = obj->start;

				// get the tile id first for assignment
				while (strcmp(el->name->string, "id"))
				el = el->next;

				struct json_number_s* n = json_value_as_number(el->value);
				int tile_id = atoi(n->number);
				tileset->tiles[tile_id].id = tile_id;

				el = obj->start;

				// Parse tiles
				while (el) {
					if (strcmp(el->name->string, "type") == 0) {
						struct json_string_s* str = json_value_as_string(el->value);
						if (strcmp(str->string, "collider") == 0) {
							tileset->tiles[tile_id].type = tile_type_collider;
						}
						else if (strcmp(str->string, "coin") == 0) {
							tileset->tiles[tile_id].type = tile_type_coin;
						}
						else if (strcmp(str->string, "key") == 0) {
							tileset->tiles[tile_id].type = tile_type_key;
						}
						else if (strcmp(str->string, "door") == 0) {
							tileset->tiles[tile_id].type = tile_type_door;
						}
						else if (strcmp(str->string, "chest") == 0) {
							tileset->tiles[tile_id].type = tile_type_chest;
						}
						else if (strcmp(str->string, "spawn") == 0) {
							tileset->tiles[tile_id].type = tile_type_spawn;
						}
						else {
							tileset->tiles[tile_id].type = tile_type_unknown;
						}
					}
					else if (strcmp(el->name->string, "animation") == 0) {
						struct json_array_s* anim_array = (struct json_array_s*)el->value->payload;
						struct json_array_element_s* anim_array_el = anim_array->start;


						while (anim_array_el) {
							struct json_object_s* anim_obj = json_value_as_object(anim_array_el->value);
							struct json_object_element_s* anim_el = anim_obj->start;

							while (anim_el) {
								if (strcmp(anim_el->name->string, "duration") == 0) {
									struct json_number_s* num = json_value_as_number(anim_el->value);
									tileset->tiles[tile_id].frames[tileset->tiles[tile_id].frame_count].duration = atoi(num->number);
								}
								else if (strcmp(anim_el->name->string, "tileid") == 0) {
									struct json_number_s* num = json_value_as_number(anim_el->value);
									tileset->tiles[tile_id].frames[tileset->tiles[tile_id].frame_count].tile = atoi(num->number);
								}
								anim_el = anim_el->next;
							}
							tileset->tiles[tile_id].frame_count++;

							anim_array_el = anim_array_el->next;
						}

					}
					el = el->next;
				}
				item = item->next;
			}
		}
		element = element->next;
	}

	free(buffer);
	return 1;
}

tilemap_t* create_tilemap_from_file(const char* filename)
{
	FILE* fp = fopen(filename, "rb");

	if (!fp) {
		printf("Failed to open file!\n");
		return 0;
	}

	size_t filesize = 0;
	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	rewind(fp);
	char* buffer = malloc(filesize + 1);
	assert(buffer);
	buffer[filesize] = 0;
	fread(buffer, 1, filesize, fp);
	fclose(fp);

	tilemap_t* tilemap = malloc(sizeof(tilemap_t));
	assert(tilemap);

	struct json_value_s* root = json_parse(buffer, strlen(buffer));
	assert(root->type == json_type_object);

	struct json_object_s* object = (struct json_object_s*)root->payload;
	assert(object);

	struct json_object_element_s* element = object->start;
	assert(element);

	while (element) {
		if (strcmp(element->name->string, "width") == 0) {
			struct json_number_s* n = json_value_as_number(element->value);
			tilemap->width = atoi(n->number);
		}
		else if (strcmp(element->name->string, "height") == 0) {
			struct json_number_s* n = json_value_as_number(element->value);
			tilemap->height = atoi(n->number);
		}
		else if (strcmp(element->name->string, "tilewidth") == 0) {
			struct json_number_s* n = json_value_as_number(element->value);
			tilemap->tile_width = atoi(n->number);
		}
		else if (strcmp(element->name->string, "tileheight") == 0) {
			struct json_number_s* n = json_value_as_number(element->value);
			tilemap->tile_height = atoi(n->number);
		}
		else if (strcmp(element->name->string, "layers") == 0) {
			struct json_array_s* layer_arr = json_value_as_array(element->value);
			struct json_array_element_s* layer_el = layer_arr->start;
			
			tilemap->layer_count = (int)layer_arr->length;
			tilemap_layer_t* current_layer = tilemap->layers;

			while (layer_el) {
				struct json_object_s* layer_obj = json_value_as_object(layer_el->value);
				struct json_object_element_s* el = layer_obj->start;

				while (el) {
					char name_buffer[32] = { 0 };

					if (strcmp(el->name->string, "name") == 0) {
						struct json_string_s* str = json_value_as_string(el->value);
						string_to_lowercase(name_buffer, 32, str->string);

						if (strcmp(name_buffer, "background") == 0) {
							current_layer->type = layer_type_background;
						}
						else if (strcmp(name_buffer, "midground") == 0) {
							current_layer->type = layer_type_midground;
						}
						else if (strcmp(name_buffer, "foreground") == 0) {
							current_layer->type = layer_type_foreground;
						}
						else {
							printf("Layer with name '%s' will be ignored\n", el->name->string);
						}
					}
					else if (strcmp(el->name->string, "data") == 0)
					{
						struct json_array_s* data_array = json_value_as_array(el->value);
						struct json_array_element_s* data_array_element = data_array->start;

						current_layer->tile_count = (int)data_array->length;
						current_layer->tiles = malloc(sizeof(int) * data_array->length);
						assert(current_layer->tiles);
						int* current_tile = current_layer->tiles;

						while (data_array_element) {
							struct json_number_s* data_array_value = json_value_as_number(data_array_element->value);
							*current_tile++ = atoi(data_array_value->number) - 1;
							data_array_element = data_array_element->next;
						}
					}
					el = el->next;
				}
				current_layer++;
				layer_el = layer_el->next;
			}
		}
		else if (strcmp(element->name->string, "tilesets") == 0) {
			struct json_array_s* json_array = json_value_as_array(element->value);
			struct json_array_element_s* json_array_element = json_array->start;
			struct json_object_s* json_object = json_value_as_object(json_array_element->value);
			struct json_object_element_s* json_object_element = json_object->start;
			while (json_object_element) {
				if (strcmp(json_object_element->name->string, "source") == 0) {
					struct json_string_s* str = json_value_as_string(json_object_element->value);
					create_tileset_from_file(&tilemap->tileset, str->string);
				}
				json_object_element = json_object_element->next;
			}
		}
		element = element->next;
	}
	return tilemap;
}

void destroy_tilemap(tilemap_t* tilemap)
{
	for (int i = 0; i < tilemap->layer_count; i++) {
		if (tilemap->layers[i].tiles) {
			free(tilemap->layers[i].tiles);
		}
	}
	free(tilemap->tileset.tiles);
}

void update_tilemap(tilemap_t* tilemap, float dt)
{
	tileset_t* tileset = &tilemap->tileset;
	tile_t* tile = tileset->tiles;
	while (tile != &tileset->tiles[tileset->tile_count]) {
		if (tile->frame_count > 0) {
			tile->frame_timer += dt;
			int ms = (int)(tile->frame_timer * 1000.0f);
			if (ms >= tile->frames[tile->current_frame].duration) {
				tile->current_frame = (tile->current_frame + 1) % tile->frame_count;
				tile->frame_timer = 0.0f;
			}
		}
		tile++;
	}
}

tilemap_layer_t* find_tilemap_layer(tilemap_t* tilemap, layer_type_t type)
{
	for (int i = 0; i < tilemap->layer_count; i++) {
		if (tilemap->layers[i].type == type) {
			return &tilemap->layers[i];
		}
	}
	return 0;
}