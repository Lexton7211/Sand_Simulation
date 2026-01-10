#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <algorithm>
#include <cmath>

const int EMPTYCELL = 0;
const int SANDCELL = 1;
const int WATERCELL = 2;
const int WETSANDCELL = 3;

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

SDL_Color wetSandColor = {178, 164, 93, 255};
SDL_Color sandColor = {255, 211, 106, 255};
SDL_Color waterColor = {41, 104, 255, 200};


//make grid
//if a cell = 0 it is empty and if it = 1 it has a sand cell in it 
//if a cell = 2 it has a water cell in it
//if a cell 3 it has wetSand in it

const int cellSize = 4;
int grid[WIDTH][HEIGHT] = {0};

bool running = true;
SDL_Event event;
while (running){

while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
        running = false;
    }
}
        int mouseX;
        int mouseY;
        Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);


        if ( mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
        double circle_radius = 5;

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
                  grid[x][y] = WATERCELL;
                }
              }
            }
          }
        }

        }

        if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        double circle_radius = 5;

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
                  grid[x][y] = SANDCELL;
                }
              }
            }
          }
        }

        }



//update particle
  updateParticle(grid);

for(int i = 0; i < 4; i++){
  for(int x = 0; x < WIDTH; x++){
    for(int y = 0; y < HEIGHT; y++){
      if((grid[x][y] == SANDCELL || grid[x][y] == WETSANDCELL) && grid[x][y + 1] == WATERCELL){
        grid[x][y] = EMPTYCELL;
        grid[x][y + 1] = EMPTYCELL;

        grid[x][y] = WATERCELL;
        grid[x][y + 1] = WETSANDCELL;
        break;
      }
    }
  } 
} 

for(int x = 0; x < WIDTH; x++){
  for(int y = 0; y < HEIGHT; y++){
    if(grid[x][y] == SANDCELL){
      if(y + 1 < HEIGHT && grid[x][y + 1] == WATERCELL){
        grid[x][y] = WETSANDCELL;
      }
      else if(y - 1 > 0 && grid[x][y - 1] == WATERCELL){
        grid[x][y] = WETSANDCELL;
      }
      else if(x + 1 < WIDTH && grid[x + 1][y] == WATERCELL){
        grid[x][y] = WETSANDCELL;
      }
      else if(x - 1 > 0 && grid[x - 1][y] == WATERCELL){
        grid[x][y] = WETSANDCELL;
      }
    }
  }
}

SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
SDL_RenderClear(renderer);

for(int x = 0; x < WIDTH; x++){
  for(int y = 0; y < HEIGHT; y++){
    if(grid[x][y] == SANDCELL){
      SDL_SetRenderDrawColor(renderer, sandColor.r, sandColor.g, sandColor.b, sandColor.a);
      int pixelX = x * cellSize;
      int pixelY = y * cellSize;
      SDL_Rect rect = { pixelX, pixelY, cellSize, cellSize };

      SDL_RenderFillRect(renderer, &rect);
    }
    else if(grid[x][y] == WATERCELL){
      SDL_SetRenderDrawColor(renderer, waterColor.r, waterColor.g, waterColor.b, waterColor.a);

      int pixelX = x * cellSize;
      int pixelY = y * cellSize;
      SDL_Rect rect = { pixelX, pixelY, cellSize, cellSize };

      SDL_RenderFillRect(renderer, &rect);
    }
    else if(grid[x][y] == WETSANDCELL){
      SDL_SetRenderDrawColor(renderer, wetSandColor.r, wetSandColor.g, wetSandColor.b, wetSandColor.a);

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
      int CELLTYPE = grid[x][y];

      if(y + 1 < HEIGHT && grid[x][y + 1] == EMPTYCELL){
        if(CELLTYPE != EMPTYCELL){
          grid[x][y] = EMPTYCELL;
          grid[x][y + 1] = CELLTYPE;
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
        if(CELLTYPE != 0 && + 1 < WIDTH && y + 1 < HEIGHT && grid[x + 1][y + 1] == EMPTYCELL){
          if(CELLTYPE != EMPTYCELL){
            grid[x][y] = EMPTYCELL;
            grid[x + 1][y + 1] = CELLTYPE;
            continue;
          }
        }
      }
        else{
          if(x - 1 > 0 && y + 1 < HEIGHT && grid[x - 1][y + 1] == EMPTYCELL){
            if(CELLTYPE != EMPTYCELL){
              grid[x][y] = EMPTYCELL;
              grid[x - 1][y + 1] = CELLTYPE;
              continue;
            }
          }

        }

  bool leftFirst = rand() & 1;
  //Check sideways for water
      if(CELLTYPE == WATERCELL){
        if(leftFirst){
          if(x - 1 >= 0 && grid[x - 1][y] == EMPTYCELL){
            grid[x][y] = EMPTYCELL;
            grid[x - 1][y] = WATERCELL;
            continue;
          }
        }
        else{
          if(x + 1 < WIDTH && grid[x + 1][y] == EMPTYCELL){
            grid[x][y] = EMPTYCELL;
            grid[x + 1][y] = WATERCELL;
            continue;
          }
        }
      }
    }
  }
}
