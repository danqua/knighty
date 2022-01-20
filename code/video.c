#define  _CRT_SECURE_NO_WARNINGS
#include "video.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <stdio.h>
#include <assert.h>
#include <stdarg.h>

SDL_Window* window;
SDL_Renderer* renderer;

typedef struct {
	int cwidth;
	int cheight;
	int cofs;
	int columns;
	SDL_Texture* texture;
} font_config_t;

font_config_t font;

void font_init()
{
	font.texture = load_texture_from_file("font.png");
	font.cwidth = 7;
	font.cheight = 9;
	font.columns = 18;
	font.cofs = 32;
}

int video_init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("%s\n", SDL_GetError());
		return 0;
	}
	
	window = SDL_CreateWindow(
		"Platformer",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		SDL_WINDOW_SHOWN
	);

	if (!window) {
		printf("%s\n", SDL_GetError());
		return 0;
	}

	renderer = SDL_CreateRenderer(
		window,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!renderer) {
		printf("%s\n", SDL_GetError());
		return 0;
	}

	font_init();

	return 1;
}

void video_shutdown()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

struct SDL_Texture* load_texture_from_file(const char* filename)
{
	int width, height;
	stbi_uc* pixels = stbi_load(filename, &width, &height, 0, STBI_rgb_alpha);
	SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom(pixels, width, height, 32, width  * 4, SDL_PIXELFORMAT_ABGR8888);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	stbi_image_free(pixels);
	return texture;
}

void draw_text(int x, int y, char* format, ...)
{
	va_list args;
	va_start(args, format);
	char buffer[512] = { 0 };
	vsprintf(buffer, format, args);
	va_end(args);

	char* c = buffer;
	while (*c) {
		int tileid = *c - font.cofs;
		++c;
		SDL_Rect src_rect = {
			(int)(tileid % font.columns) * font.cwidth,
			(int)(tileid / font.columns) * font.cheight,
			font.cwidth,
			font.cheight
		};

		SDL_Rect dst_rect = { x, y, font.cwidth * 2, font.cheight * 2 };
		x+= font.cwidth * 2;

		SDL_RenderCopy(renderer, font.texture, &src_rect, &dst_rect);
	}
}