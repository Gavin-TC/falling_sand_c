#include "SDL2/SDL_keycode.h"
#include "SDL2/SDL_mouse.h"
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_surface.h"
#include "SDL2/SDL_video.h"
#include <stdio.h>
#include <math.h>


#define WIDTH 960
#define HEIGHT 640
#define CELL_SIZE 5 // in px
#define ROWS WIDTH / CELL_SIZE
#define COLS HEIGHT / CELL_SIZE
#define GRID_THICKNESS 1

#define EMPTY_TILE 0
#define SAND_TILE 1
#define WALL_TILE 2

#define EMPTY_COLOR 0x00000000
#define GRID_COLOR 0x33333333
#define SAND_COLOR 0xFFD4A017
#define WALL_COLOR 0xfffffff

void copy_arr(int source[ROWS][COLS], int dest[ROWS][COLS]) {
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			dest[i][j] = source[i][j];
		}
	}
}

void simulate(int env[ROWS][COLS], int steps, int sim_ticks) {
    if (steps < 1) {
        return;
    }

	// Simulate for x steps
	for (int s = 0; s < steps; s++) {
        int n_env[ROWS][COLS] = {0};

		for (int i = 0; i < ROWS; i++) {
			for (int j = COLS; j >= 0; j--) {
			//for (int j = 0; j < COLS; j++) {
				int tile = env[i][j];

				if (tile == WALL_TILE) {
					n_env[i][j] = WALL_TILE;
					continue;
				}

				if (tile == SAND_TILE) {
					if (j + 1 >= COLS) {
						n_env[i][j] = SAND_TILE;
						continue;
					}

					if (env[i][j + 1] == EMPTY_TILE) {
						n_env[i][j + 1] = SAND_TILE;
						continue;
					}

					if (env[i][j + 1] == WALL_TILE) {
						n_env[i][j] = SAND_TILE;
						continue;
					}

					int right_clear = (i + 1 < ROWS) && env[i + 1][j + 1] == EMPTY_TILE;
					int left_clear = (i - 1 >= 0) && env[i - 1][j + 1] == EMPTY_TILE;

					if (right_clear && left_clear) {
						int rnd = rand() % 2;

						if (rnd == 0) {
							n_env[i + 1][j + 1] = SAND_TILE;
						} else {
							n_env[i - 1][j + 1] = SAND_TILE;
						}
						continue;
					}

					if (right_clear) {
						n_env[i + 1][j + 1] = SAND_TILE;
						continue;
					}

					if (left_clear) {
						n_env[i - 1][j + 1] = SAND_TILE;
						continue;
					}

					n_env[i][j] = SAND_TILE;
				}
			}
		}

        copy_arr(n_env, env);
	}
}


void render_screen(SDL_Surface* screen, int env[ROWS][COLS], int grid) {
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			SDL_Rect tile = { i * CELL_SIZE, j * CELL_SIZE, CELL_SIZE, CELL_SIZE };

			switch (env[i][j]) {
				case SAND_TILE:
					SDL_FillRect(screen, &tile, SAND_COLOR);
					break;

				case WALL_TILE:
					SDL_FillRect(screen, &tile, WALL_COLOR);
					break;

				case EMPTY_TILE:
					SDL_FillRect(screen, &tile, EMPTY_TILE);
					break;
			}
		}
	}

	if (grid == 1) {
		for (int i = 0; i < ROWS; i++) {
			for (int j = 0; j < COLS; j++) {
				SDL_Rect vertical_line = {i * CELL_SIZE, j * CELL_SIZE, GRID_THICKNESS, CELL_SIZE};
				SDL_Rect horizontal_line = {i * CELL_SIZE, j * CELL_SIZE, CELL_SIZE, GRID_THICKNESS};

				SDL_FillRect(screen, &vertical_line, GRID_COLOR);
				SDL_FillRect(screen, &horizontal_line, GRID_COLOR);
			}
		}
	}
}

void initialize_environment(int env[ROWS][COLS]) {
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			env[i][j] = EMPTY_TILE;
		}
	}
}

int main() {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("Falling Sand Simulation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
	SDL_Surface* screen = SDL_GetWindowSurface(window);

	SDL_Rect clear_screen_rect = {0, 0, WIDTH, HEIGHT};
	SDL_Event event;


	int fps = 120;
	int sim_ticks = 0;
	int game_ticks = 0;
	int draw_grid = 0;
    int sim_paused = 0;
	int sand_toggle = 1; // 1 = sand, 0 = wall
	int env[ROWS][COLS];
	int simulation_running = 1;
	
	int x, y;
	int mouse_1_pressed = 0;
	int mouse_2_pressed = 0;

	int brush_size = 1;

	int virtual_sand_px = 0;
	int actual_sand_px = 0;
	
	initialize_environment(env);

	while (simulation_running ) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				simulation_running = 0;
			}

			if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					simulation_running = 0;
				}

				if (event.key.keysym.sym == SDLK_SPACE) {
					sand_toggle = !sand_toggle;
				}

				if (event.key.keysym.sym == SDLK_p) {
					sim_paused = !sim_paused;
				}

				if (event.key.keysym.sym == SDLK_g) {
					draw_grid = !draw_grid;
				}

				if (event.key.keysym.sym == SDLK_n && sim_paused == 1) {
                    simulate(env, 1, sim_ticks);
				}

				if (event.key.keysym.sym == SDLK_UP) {
					fps = fmax(1, fps + 5);
				}

				if (event.key.keysym.sym == SDLK_DOWN) {
					fps = fmax(1, fps - 5);
				}

				if (event.key.keysym.sym == SDLK_r) {
					initialize_environment(env);
				}

				if (event.key.keysym.sym == SDLK_c) {
					actual_sand_px = 0;

					for (int i = 0; i < ROWS; i++) {
						for (int j = 0; j < COLS; j++) {
							if (env[i][j] == SAND_TILE) {
								actual_sand_px++;
							}
						}
					}
				}

				if (event.key.keysym.sym == SDLK_x) {
					virtual_sand_px = 0;
					actual_sand_px = 0;
				}
			}

			if (event.type == SDL_MOUSEBUTTONDOWN) {
				if (event.button.button == SDL_BUTTON_LEFT) {
					mouse_1_pressed = 1;
				} else if (event.button.button == SDL_BUTTON_RIGHT) {
					mouse_2_pressed = 1;
				}

			} else if (event.type == SDL_MOUSEBUTTONUP) {
				if (event.button.button & SDL_BUTTON(SDL_BUTTON_LEFT)) {
					mouse_1_pressed = 0;
				} else if (event.button.button & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
					mouse_2_pressed = 0;
				}
			}

			if (event.type == SDL_MOUSEWHEEL) {
				brush_size = fmax(1, brush_size + event.wheel.y);
			}
		}

		if (SDL_GetMouseState(&x, &y)) {
			int col = x / CELL_SIZE;
			int row = y / CELL_SIZE;

			if (mouse_1_pressed == 1 && env[col][row] != WALL_TILE) {
				for (int i = 0; i < brush_size; i++) {
					for (int j = 0; j < brush_size; j++) {
						if (env[i + col - brush_size / 2][j + row - brush_size / 2] != WALL_TILE &&
							env[i + col - brush_size / 2][j + row - brush_size / 2] != SAND_TILE) {
							env[i + col - brush_size / 2][j + row - brush_size / 2] = sand_toggle ? SAND_TILE : WALL_TILE;
						}
					}
				}
			} else if (mouse_2_pressed == 1) {
				for (int i = 0; i < brush_size; i++) {
					for (int j = 0; j < brush_size; j++) {
						env[i + col - brush_size / 2][j + row - brush_size / 2] = EMPTY_TILE;
					}
				}
			}
		}

		SDL_FillRect(screen, &clear_screen_rect, 0x00000000); // Clear the screen
		render_screen(screen, env, draw_grid);

        if (sim_paused == 0) {
            simulate(env, 1, sim_ticks);
			sim_ticks++;
        }

		if (game_ticks % 1 == 0) {
			printf("actual_sand_px: %d\n", actual_sand_px);
		}
		game_ticks++;

		SDL_UpdateWindowSurface(window); // Update the screen
		SDL_Delay((int) 1000 / fps);
	}
}
