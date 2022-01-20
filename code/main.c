#include "tilemap.h"
#include "video.h"
#include "game.h"

int SDL_main(int argc, char** argv)
{
	int success = video_init();

	if (!success) {
		return -1;
	}

	game_t game;
	game_init(&game);
	
	SDL_Event ev = { 0 };
	Uint64 old_time = SDL_GetPerformanceCounter();
	Uint64 elapsed_time = 0;
	float dt = 0.0f;
	bool quit = false;

	while (game.is_running) {
		elapsed_time = SDL_GetPerformanceCounter() - old_time;
		old_time = SDL_GetPerformanceCounter();
		dt = (float)elapsed_time / SDL_GetPerformanceFrequency();

		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_QUIT) {
				game.is_running = false;
			}
			else if (ev.type == SDL_KEYDOWN) {
				if (ev.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
					game.is_running = false;
				}
				else {
					game_on_key_event(&game, &ev.key);
				}
			}
			else if (ev.type == SDL_KEYUP) {
				game_on_key_event(&game, &ev.key);
			}
		}
		
		game_update(&game, dt);
		game_draw(renderer, &game);
	}

	video_shutdown();

	return 0;
}