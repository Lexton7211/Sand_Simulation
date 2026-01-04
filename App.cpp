#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <algorithm>
#include <cmath>


const int WIDTH  = 175;
const int HEIGHT = 175;

void updateParticle(int grid[WIDTH][HEIGHT]);




const double PI = 3.14159;

int main (int argc, char *argv[]) {
  double deltaTime = 1.0 / 60.0;

if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
}
  
int window_width = 700;
int window_height = 700;

     // Create a window
    SDL_Window* window = SDL_CreateWindow(
        "Sand Simulation",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

SDL_Color wetSandColor = {149, 125, 68, 255};
SDL_Color sandColor = {255, 211, 106, 255};
SDL_Color waterColor = {41, 104, 255, 200};


//make grid
//if a cell = 0 it is empty and if it = 1 it has a sand cell in it 
//if a cell = 2 it has a water cell in it

const int cellSize = 4;
int grid[WIDTH][HEIGHT] = {0};

bool running = true;
SDL_Event event;
while (running){
while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
        running = false;
    }

    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_RIGHT) {
        double circle_radius = 15;

        int mouseX = event.button.x;
        int mouseY = event.button.y;

        // Convert to grid coordinates
        int gridX = mouseX / cellSize;
        int gridY = mouseY / cellSize;

        // Safety check
        if (gridX >= 0 && gridX < 175 && gridY >= 0 && gridY < 175) {
          int r2 = circle_radius * circle_radius; // radius squared
          for (int i = -circle_radius; i <= circle_radius; i++) {
            for (int j = -circle_radius; j <= circle_radius; j++) {
              int x = gridX + j;
              int y = gridY + i;
              if (x >= 0 && x < 175 && y >= 0 && y < 175) {
                if (i * i + j * j <= r2) { // inside circle
                  if(grid[x][y] == 1 || grid[x][y] == 2) continue;
                  grid[x][y] = 2;
                }
              }
            }
          }
        }

        }
        if (event.button.button == SDL_BUTTON_LEFT) {
        double circle_radius = 15;

        int mouseX = event.button.x;
        int mouseY = event.button.y;

        // Convert to grid coordinates
        int gridX = mouseX / cellSize;
        int gridY = mouseY / cellSize;

        // Safety check
        if (gridX >= 0 && gridX < 175 && gridY >= 0 && gridY < 175) {
          int r2 = circle_radius * circle_radius; // radius squared
          for (int i = -circle_radius; i <= circle_radius; i++) {
            for (int j = -circle_radius; j <= circle_radius; j++) {
              int x = gridX + j;
              int y = gridY + i;
              if (x >= 0 && x < 175 && y >= 0 && y < 175) {
                if (i * i + j * j <= r2) { // inside circle
                  if(grid[x][y] == 1 || grid[x][y] == 2) continue;
                  grid[x][y] = 1;
                }
              }
            }
          }
        }

        }
    }
}




//update particle
  updateParticle(grid);

  for(int x = 0; x < WIDTH; x++){
    for(int y = 0; y < HEIGHT; y++){
      if(grid[x][y] == 1 && grid[x][y + 1] == 2){
        grid[x][y] = 0;
        grid[x][y + 1] = 0;

        grid[x][y] = 2;
        grid[x][y + 1] = 1;
        break;
      }
    }
  }
   
SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
SDL_RenderClear(renderer);

SDL_SetRenderDrawColor(renderer, sandColor.r, sandColor.g, sandColor.b, sandColor.a);
for(int x = 0; x < WIDTH; x++){
  for(int y = 0; y < HEIGHT; y++){
    if(grid[x][y] == 1){
      int pixelX = x * cellSize;
      int pixelY = y * cellSize;
      SDL_Rect rect = { pixelX, pixelY, cellSize, cellSize };

      SDL_RenderFillRect(renderer, &rect);
    }
  }
}


SDL_SetRenderDrawColor(renderer, waterColor.r, waterColor.g, waterColor.b, waterColor.a);
for(int x = 0; x < WIDTH; x++){
  for(int y = HEIGHT - 2; y >= 0; y--){
    if(grid[x][y] == 2){
      int pixelX = x * cellSize;
      int pixelY = y * cellSize;
      SDL_Rect rect = { pixelX, pixelY, cellSize, cellSize };

      SDL_RenderFillRect(renderer, &rect);
    }
  }
}


    SDL_RenderPresent(renderer);
    SDL_Delay(16); // ~60 FPS
    }
SDL_DestroyRenderer(renderer);
SDL_DestroyWindow(window);
SDL_Quit();
  return 0;
}



void updateParticle(int grid[WIDTH][HEIGHT]){
  //check below
  for(int x = 0; x < WIDTH; x++){
    for(int y = HEIGHT - 2; y >= 0; y--){
      if(y + 1 < HEIGHT && grid[x][y + 1] == 0){
        if(grid[x][y] == 1){
          grid[x][y] = 0;
          grid[x][y + 1] = 1;
          continue;
        }

        else if(grid[x][y] == 2){
          grid[x][y] = 0;
          grid[x][y + 1] = 2;
          continue;
        }
      }

  //check left and right
  if((rand() % 100) < 70){
    continue;
  }
  bool rightFirst;
  if((rand() % 100) < 50){
    rightFirst = true;
  }
  else{
    rightFirst = false;
  }

      if(rightFirst){
        if(grid[x][y] != 0 && + 1 < WIDTH && y + 1 < HEIGHT && grid[x + 1][y + 1] == 0){
          if(grid[x][y] == 1){
            grid[x][y] = 0;
            grid[x + 1][y + 1] = 1;
            continue;
          }
          else if(grid[x][y] == 2){
            grid[x][y] = 0;
            grid[x + 1][y + 1] = 2;
            continue;
          }
        }
        else{
          if(x - 1 > 0 && y + 1 < HEIGHT && grid[x - 1][y + 1] == 0){
            if(grid[x][y] == 1){
              grid[x][y] = 0;
              grid[x - 1][y + 1] = 1;
              continue;
            }
            else if(grid[x][y] == 2){
              grid[x][y] = 0;
              grid[x - 1][y + 1] = 2;
              continue;
            }
          }

        }
      }

  bool leftFirst = rand() & 1;
  //Check sideways for water
      if(grid[x][y] == 2){
        if(leftFirst){
          if(x - 1 >= 0 && grid[x - 1][y] == 0){
            grid[x][y] = 0;
            grid[x - 1][y] = 2;
            continue;
          }
        }
        else{
          if(x + 1 < WIDTH && grid[x + 1][y] == 0){
            grid[x][y] = 0;
            grid[x + 1][y] = 2;
            continue;
          }
        }
      }
    }
  }
}
