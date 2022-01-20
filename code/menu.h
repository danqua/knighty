#ifndef MENU_H
#define MENU_H
#include "game.h"

void menu_on_key_event(game_t* game, struct SDL_KeyboardEvent* ev);
void menu_update(float dt);
void menu_draw();

#endif