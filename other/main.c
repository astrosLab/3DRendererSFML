#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>

int main() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("SDL_Init ERROR: %s\n", SDL_GetError());
	}

	SDL_Window* window = SDL_CreateWindow("SDL3 Renderer",
					      800, 600,
					      SDL_WINDOW_OPENGL);

	if (!window) {
		printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
	}

	printf("Starting Window\n");
	SDL_Event event;
	int running = 1;

	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				running = 0;
			}
		}
	}

	printf("Closing\n");

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

