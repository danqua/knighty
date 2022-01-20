#ifndef VIDEO_H
#define VIDEO_H

#include <stdbool.h>
#include <SDL.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 632

extern SDL_Window* window;
extern SDL_Renderer* renderer;

int video_init();
void video_shutdown();

SDL_Texture* load_texture_from_file(const char* filename);
void draw_text(int x, int y, char* format, ...);

#endif