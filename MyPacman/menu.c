/* SDL-Man (Menu)
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

#include "sdlman.h"
#include "resource.h"
#include <SDL_main.h> /* To remap main() for portability. */
#include <time.h> /* To seed randomizer. */
#include<Windows.h>


#define SDLMAN_FILENAME_LENGTH 16
#define SDLMAN_MAX_ENEMY_SPEED 9
#define SDLMAN_MAX_WORLD 5
#define SDLMAN_MENU_NUMBER_WIDTH 26
#define SDLMAN_MENU_NUMBER_HEIGHT 32

#define SDLMAN_HIGHSCORE_FILE "highscore.dat"



static void sdlman_draw_menu_number(SDL_Surface * s, SDL_Surface * ns,
	int x, int y, int n)
{
	int i, pow, digits;
	SDL_Rect src, dst;

	src.w = dst.w = SDLMAN_MENU_NUMBER_WIDTH;
	src.h = dst.h = SDLMAN_MENU_NUMBER_HEIGHT;

	dst.x = x;
	dst.y = y;
	src.y = 0;

	/* Find digits in number. */
	if (n > 0) {
		digits = 0;
		i = n;
		while (i != 0) {
			i = i / 10;
			digits++;
		}
	}
	else {
		digits = 1;
		n = 0; /* Force to zero if negative. */
	}

	while (digits > 0) {

		pow = 1;
		for (i = 0; i < digits - 1; i++)
			pow *= 10;

		switch ((n / pow) % 10) {
		case 1:
			src.x = 0;
			break;
		case 2:
			src.x = SDLMAN_MENU_NUMBER_WIDTH;
			break;
		case 3:
			src.x = SDLMAN_MENU_NUMBER_WIDTH * 2;
			break;
		case 4:
			src.x = SDLMAN_MENU_NUMBER_WIDTH * 3;
			break;
		case 5:
			src.x = SDLMAN_MENU_NUMBER_WIDTH * 4;
			break;
		case 6:
			src.x = SDLMAN_MENU_NUMBER_WIDTH * 5;
			break;
		case 7:
			src.x = SDLMAN_MENU_NUMBER_WIDTH * 6;
			break;
		case 8:
			src.x = SDLMAN_MENU_NUMBER_WIDTH * 7;
			break;
		case 9:
			src.x = SDLMAN_MENU_NUMBER_WIDTH * 8;
			break;
		case 0:
			src.x = SDLMAN_MENU_NUMBER_WIDTH * 9;
			break;
		default:
			return; /* Not found, just return. */
		}

		SDL_BlitSurface(ns, &src, s, &dst);

		digits--;
		dst.x += SDLMAN_MENU_NUMBER_WIDTH;
	}
}



static void sdlman_load_highscore(int* table, char* filename)
{
	int i;
	FILE* fh;

	fh = fopen(filename, "r");
	if (fh == NULL) {
		fprintf(stderr, "Warning: Unable to load highscore file.\n");
		/* Set all highscores to zero. */
		for (i = 0; i < SDLMAN_MAX_WORLD; i++) {
			table[i] = 0;
		}
	}
	else {
		fread(table, sizeof(int), SDLMAN_MAX_WORLD, fh);
		fclose(fh);
	}
}



static void sdlman_save_highscore(int* table, char* filename)
{
	FILE* fh;

	fh = fopen(filename, "w");
	if (fh == NULL) {
		fprintf(stderr, "Warning: Unable to save highscore file.\n");
	}
	else {
		fwrite(table, sizeof(int), SDLMAN_MAX_WORLD, fh);
		fclose(fh);
	}
}

static SDL_Surface* sdlman_load_surface_from_resources(int resource_id) {
	HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE(resource_id), "PICTURE");
	if (hRes == NULL) {
		return NULL;
	}

	unsigned int resource_size = SizeofResource(NULL, hRes);
	HGLOBAL hResLoad = LoadResource(NULL, hRes);
	if (hResLoad == NULL)
	{
		return NULL;
	}

	unsigned char* resource_data = (unsigned char*)LockResource(hResLoad);
	FILE* ptr = fopen("menu.bmp", "r");
	char arrayy[1024];
	//fwrite(resource_data, sizeof(char), resource_size - 16, ptr);
	fread(arrayy, sizeof(char), 1024, ptr);
	SDL_Surface* surface = SDL_LoadBMP_RW(SDL_RWFromConstMem(resource_data, resource_size), 1);
	fprintf(stderr, "Error: Unable to initalize SDL: %s\n", SDL_GetError());
	return surface;
}



int main(int argc, char* argv[])
{
	SDL_Event event;
	SDL_Surface* screen, * menu_surface, * number_surface, * temp_surface;
	int game_done, game_result, world_number, enemy_speed, game_score;
	int layout_resources_ids[SDLMAN_MAX_WORLD] = {IDR_WOTLD_1_LAYOUT, IDR_WOTLD_2_LAYOUT,
		                                           IDR_WOTLD_3_LAYOUT ,IDR_WOTLD_4_LAYOUT ,IDR_WOTLD_5_LAYOUT };
	int graphic_resources_ids[SDLMAN_MAX_WORLD] = { IDB_WORLD_1_BITMAP, IDB_WORLD_2_BITMAP,
												   IDB_WORLD_3_BITMAP ,IDB_WORLD_4_BITMAP ,IDB_WORLD_5_BITMAP };
	int high_score[SDLMAN_MAX_WORLD];

	/* Use srand() instead of srandom() to be more portable. */
	srand((unsigned)time(NULL));

	/* Try to load highscores from persistent file. */
	sdlman_load_highscore(high_score, SDLMAN_HIGHSCORE_FILE);

	/* Video and screen initialization here, because of the graphical menu. */
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
		fprintf(stderr, "Error: Unable to initalize SDL: %s\n", SDL_GetError());
		return 1;
	}
	atexit(SDL_Quit);
	screen = SDL_SetVideoMode(SDLMAN_WORLD_X_SIZE * SDLMAN_BLOCK_SIZE,
		SDLMAN_WORLD_Y_SIZE * SDLMAN_BLOCK_SIZE, 24, SDL_DOUBLEBUF);
	if (screen == NULL) {
		fprintf(stderr, "Error: Unable to set video mode: %s\n", SDL_GetError());
		return 1;
	}
	SDL_WM_SetCaption("SDL-Man", "SDL-Man");


	/* Load and convert menu graphic files. */
	temp_surface = sdlman_load_surface_from_resources(IDB_MENU_BITMAP);
	if (temp_surface == NULL) {
		fprintf(stderr, "Error: Unable to load menu graphics: %s\n",
			SDL_GetError());
		return 1;
	}
	else {
		menu_surface = SDL_DisplayFormat(temp_surface);
		if (menu_surface == NULL) {
			fprintf(stderr, "Error: Unable to convert menu graphics: %s\n",
				SDL_GetError());
			return 1;
		}
		SDL_FreeSurface(temp_surface);
	}

	temp_surface = sdlman_load_surface_from_resources(IDB_NUMBER_BITMAP);
	if (temp_surface == NULL) {
		fprintf(stderr, "Error: Unable to load number graphics: %s\n",
			SDL_GetError());
		SDL_FreeSurface(menu_surface);
		return 1;
	}
	else {
		number_surface = SDL_DisplayFormat(temp_surface);
		if (number_surface == NULL) {
			fprintf(stderr, "Error: Unable to convert enemy graphics: %s\n",
				SDL_GetError());
			SDL_FreeSurface(menu_surface);
			return 1;
		}
		SDL_FreeSurface(temp_surface);
	}


	/* Default values. */
	enemy_speed = 5;
	world_number = 1;


	/* Main menu loop. */
	game_done = 0;
	while (!game_done) {

		/* Poll for user input. */
		if (SDL_PollEvent(&event) == 1) {
			switch (event.type) {
			case SDL_QUIT:
				game_done = 1;
				break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_q:
					game_done = 1;
					break;

				case SDLK_RETURN:
				case SDLK_SPACE:
					game_result = sdlman_gameloop(screen, layout_resources_ids[world_number-1], graphic_resources_ids[world_number-1],
						enemy_speed, &game_score);

					if (game_result == SDLMAN_GAMELOOP_OK) {
						/* Update high score. */
						if (game_score > high_score[world_number - 1])
							high_score[world_number - 1] = game_score;
					}
					else {
						game_done = 1; /* Quit or fail. */
					}
					break;

				case SDLK_w:
					world_number++;
					if (world_number > SDLMAN_MAX_WORLD)
						world_number = 1;
					break;

				case SDLK_s:
					enemy_speed++;
					if (enemy_speed > SDLMAN_MAX_ENEMY_SPEED)
						enemy_speed = 1;
					break;

				default:
					break;
				}
				break;

			default:
				continue;
			}
		}


		/* Draw graphics. */
		SDL_BlitSurface(menu_surface, NULL, screen, NULL);
		sdlman_draw_menu_number(screen, number_surface, 385, 163, world_number);
		sdlman_draw_menu_number(screen, number_surface, 385, 211, enemy_speed);
		sdlman_draw_menu_number(screen, number_surface, 385, 259,
			high_score[world_number - 1]);

		SDL_Flip(screen);
		SDL_Delay(SDLMAN_GAME_SPEED);
	}


	/* Cleanup. */
	SDL_FreeSurface(menu_surface);
	SDL_FreeSurface(number_surface);
	SDL_FreeSurface(screen);
	sdlman_save_highscore(high_score, SDLMAN_HIGHSCORE_FILE);

	return 0;
}

