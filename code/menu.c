#include "menu.h"
#include "video.h"
#include <string.h>

typedef struct {
	SDL_Texture* texture;
	int selection;
	bool inited;
} menu_t;

static menu_t menu;

void menu_init() {
	menu.texture = load_texture_from_file("tileset.png");
	menu.selection = 0;
	menu.inited = true;
}
void menu_on_key_event(game_t* game, struct SDL_KeyboardEvent* ev)
{
	if (ev->type == SDL_KEYDOWN) {
		if (ev->keysym.scancode == SDL_SCANCODE_UP) {
			menu.selection = (menu.selection + 1) % 2;
		}
		else if (ev->keysym.scancode == SDL_SCANCODE_DOWN) {
			menu.selection = menu.selection - 1;
			if (menu.selection < 0)
				menu.selection = 1;
		}
		else if (ev->keysym.scancode == SDL_SCANCODE_RETURN) {
			if (menu.selection == 0) {
				game->state = state_game;
			}
			else if (menu.selection == 1) {
				game->state = state_quit;
			}
		}
	}
}

void menu_update(float dt)
{
	if (!menu.inited) {
		menu_init();
	}
}

void menu_draw()
{
	// draw background
	{
		SDL_Rect src_rect = { 40, 32, 8, 8 };
		SDL_Rect dst_rect = { 0, 0, TILE_SIZE, TILE_SIZE };
		for (int y = 0; y < WINDOW_HEIGHT / TILE_SIZE + 1; y++) {
			for (int x = 0; x < WINDOW_WIDTH / TILE_SIZE; x++) {
				dst_rect.x = x * TILE_SIZE;
				dst_rect.y = y * TILE_SIZE;
				
				SDL_RenderCopy(renderer, menu.texture, &src_rect, &dst_rect);
			}
		}
	}

	// draw logo
	{
		int width = 104 * 3;
		int height = 16 * 3;
		int x = WINDOW_WIDTH / 2 - width / 2;
		int y = WINDOW_HEIGHT / 2 - height * 2;

		SDL_Rect src_rect = { 0, 72, 104, 16 };
		SDL_Rect dst_rect = { x, y, width, height };
		SDL_RenderCopy(renderer, menu.texture, &src_rect, &dst_rect);
	}

	// Draw Start Game
	{
		char text[] = " START GAME";
		if (menu.selection == 0) text[0] = '#';
		int num_chars = (int)strlen(text);
		int width = num_chars * 16;
		int height = 16;
		int x = WINDOW_WIDTH / 2 - width / 2;
		int y = WINDOW_HEIGHT / 2 + height * 2;

		draw_text(x, y, text);
	}

	// Draw Quit Game
	{
		char text[] = " QUIT GAME";
		if (menu.selection == 1) text[0] = '#';
		int num_chars = (int)strlen(text);
		int width = num_chars * 16;
		int height = 16;
		int x = WINDOW_WIDTH / 2 - width / 2;
		int y = WINDOW_HEIGHT / 2 + height * 4;

		draw_text(x, y, text);
	}
}