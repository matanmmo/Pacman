/* SDL-Man (Shared Header)
 * Version 0.3 (23/02-08)
 * Copyright 2008 Kjetil Erga (kobolt.anarion -AT- gmail -DOT- com)
 *
 *
 * This file is part of SDL-Man.
 *
 * SDL-Man is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * SDL-Man is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SDL-Man.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SDLMAN_H
#define _SDLMAN_H
#define _CRT_SECURE_NO_DEPRECATE

/* Common includes. */
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

/* Shared definitions. */
#define SDLMAN_BLOCK_SIZE 32
#define SDLMAN_WORLD_X_SIZE 20
#define SDLMAN_WORLD_Y_SIZE 15

#define SDLMAN_GAMELOOP_OK 0
#define SDLMAN_GAMELOOP_FAIL -1
#define SDLMAN_GAMELOOP_QUIT -2

#define SDLMAN_GAME_SPEED 20

/* Prototype for gameloop. */
int sdlman_gameloop(SDL_Surface *screen, char *world_layout_file, 
  char *world_graphic_file, int enemy_speed, int *score);

#endif /* _SDLMAN_H */
