#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <sstream>
#include <cmath>
#include <thread>

const int WIDTH  = 320;
const int HEIGHT = 200;

enum class CellType{
  Empty,
  Sand,
  WetSand,
  Water,
  Oil,
  Fire
};

void updateParticle(CellType grid[WIDTH][HEIGHT], bool updated[WIDTH][HEIGHT]);
void calculateHeatTransfer(CellType grid[WIDTH][HEIGHT], int temperatureMap[WIDTH][HEIGHT], int x, int y);
void swapLiquids(CellType grid[WIDTH][HEIGHT], bool updated[WIDTH][HEIGHT], int x, int y);
void swapPowdersAndLiquids(CellType grid[WIDTH][HEIGHT], bool updated[WIDTH][HEIGHT], int x, int y);
void updateWetSand(CellType grid[WIDTH][HEIGHT], int x, int y);

//temperature is in Celsius
const int FIRETEMPERATURE = 600;
const int ROOMTEMPERATURE = 20;
const double PI = 3.14159;

int main (int argc, char *argv[]) {
  double deltaTime = 1.0 / 60.0;



int window_width = 1600;
int window_height = 1000;

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

//particle colors
SDL_Color wetSandColor = {178, 164, 93, 255};
SDL_Color sandColor = {255, 211, 106, 255};
SDL_Color waterColor = {41, 104, 255, 200};
SDL_Color oilColor = {75, 22, 7, 200};
SDL_Color fireColor = {255, 59, 1, 255};


//grids
const int cellSize = 5;
CellType grid[WIDTH][HEIGHT] = {};

bool updated[WIDTH][HEIGHT] = {};

//temperature map
int temperatureMap[WIDTH][HEIGHT] = {};

for(int x = 0; x < WIDTH; x++){
  for(int y = 0; y < HEIGHT; y++){
    temperatureMap[x][y] = ROOMTEMPERATURE;
  }
}

bool running = true;
SDL_Event event;
while (running){
  CellType chosenCell;

  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      running = false;
    }


    if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_1) chosenCell = CellType::Sand;
    if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_2) chosenCell = CellType::WetSand;
    if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_3) chosenCell = CellType::Water;
    if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_4) chosenCell = CellType::Oil;
    if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_5) chosenCell = CellType::Fire;

  }
  int mouseX;
  int mouseY;
  Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
 
  double circle_radius = 5;

  // Convert to grid coordinates
  int gridX = mouseX / cellSize;
  int gridY = mouseY / cellSize;
  
  if ( mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) {

    if (gridX >= 0 && gridX < WIDTH && gridY >= 0 && gridY < HEIGHT) {
      int r2 = circle_radius * circle_radius; // radius squared
      for (int i = -circle_radius; i <= circle_radius; i++) {
        for (int j = -circle_radius; j <= circle_radius; j++) {
          int x = gridX + j;
          int y = gridY + i;
          if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
            if (i * i + j * j <= r2) { // inside circle
              if(grid[x][y] != CellType::Empty) continue;
              grid[x][y] = chosenCell;
            }
          }
        }
      }
    }
  }

  else if ( mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) {

    if (gridX >= 0 && gridX < WIDTH && gridY >= 0 && gridY < HEIGHT) {
      int r2 = circle_radius * circle_radius; // radius squared
      for (int i = -circle_radius; i <= circle_radius; i++) {
        for (int j = -circle_radius; j <= circle_radius; j++) {
          int x = gridX + j;
          int y = gridY + i;
          if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
            if (i * i + j * j <= r2) { // inside circle
              grid[x][y] = CellType::Empty;
            }
          }
        }
      }
    }
  }

  
  for(int x = 0; x < WIDTH; x++){
    for(int y = 0; y < HEIGHT; y++){
      if(grid[x][y] == CellType::Fire){
        temperatureMap[x][y] = FIRETEMPERATURE;
      }
      calculateHeatTransfer(grid, temperatureMap, x, y);
      if(grid[x][y] == CellType::Water && temperatureMap[x][y] >= 100){
        grid[x][y] = CellType::Empty;
        temperatureMap[x][y] = ROOMTEMPERATURE;
      }
      if(grid[x][y] == CellType::WetSand && temperatureMap[x][y] >= 100){
        grid[x][y] = CellType::Sand;
      }
    }
  }

//update particles
updateParticle(grid, updated);
for(int x = 0; x < WIDTH; x++){
  for(int y = 0; y < HEIGHT; y++){
    swapPowdersAndLiquids(grid, updated, x, y);
    swapLiquids(grid, updated, x, y);
    updateWetSand(grid, x, y);
  }
}


SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
SDL_RenderClear(renderer);

for(int x = 0; x < WIDTH; x++){
  for(int y = 0; y < HEIGHT; y++){
    switch (grid[x][y]){
      case CellType::Sand:
        SDL_SetRenderDrawColor(renderer, sandColor.r, sandColor.g, sandColor.b, sandColor.a);
        break;

     case CellType::WetSand:
        SDL_SetRenderDrawColor(renderer, wetSandColor.r, wetSandColor.g, wetSandColor.b, wetSandColor.a);
        break;

      case CellType::Water:
        SDL_SetRenderDrawColor(renderer, waterColor.r, waterColor.g, waterColor.b, waterColor.a);
        break;

      case CellType::Oil:
        SDL_SetRenderDrawColor(renderer, oilColor.r, oilColor.g, oilColor.b, oilColor.a);
        break;

      case CellType::Fire:
        SDL_SetRenderDrawColor(renderer, fireColor.r, fireColor.g, fireColor.b, fireColor.a);
        break;

    }

    if(grid[x][y] != CellType::Empty){
      int pixelX = x * cellSize;
      int pixelY = y * cellSize;
      SDL_Rect rect = { pixelX, pixelY, cellSize, cellSize };

      SDL_RenderFillRect(renderer, &rect);
    }
    updated[x][y] = false;
  }
}

    SDL_RenderPresent(renderer);
    SDL_Delay(10); // 100 FPS
  }

SDL_DestroyRenderer(renderer);
SDL_DestroyWindow(window);
SDL_Quit();
  return 0;
}



void updateParticle(CellType grid[WIDTH][HEIGHT], bool updated[WIDTH][HEIGHT]){
  //check below
  for(int x = 0; x < WIDTH; x++){
    for(int y = HEIGHT - 2; y >= 0; y--){
      CellType CELLTYPE = grid[x][y];
      if(updated[x][y]) continue;

      if(CELLTYPE != CellType::Fire && y + 1 < HEIGHT && grid[x][y + 1] == CellType::Empty){
        if(CELLTYPE != CellType::Empty){
          grid[x][y] = CellType::Empty;
          grid[x][y + 1] = CELLTYPE;

          updated[x][y] = true;
          updated[x][y + 1] = true;
          continue;
        }
      }
      if(CELLTYPE == CellType::Fire && y - 1 > HEIGHT && grid[x][y - 1] == CellType::Empty){
        grid[x][y] = CellType::Empty;
        grid[x][y - 1] = CELLTYPE;

        updated[x][y] = true;
        updated[x][y - 1] = true;
        continue;
      }
  
  //check down left and down right
  if(CELLTYPE == CellType::Sand || CELLTYPE == CellType::WetSand){
    if((rand() % 100) < 70){
      continue;
    }
  }
  bool rightFirst;
  if((rand() % 100) < 50){
    rightFirst = true;
  }
  else{
    rightFirst = false;
  }


if(updated[x][y]) continue;
if(CELLTYPE != CellType::Fire){
      if(rightFirst){
        if(CELLTYPE != CellType::Empty && + 1 < WIDTH && y + 1 < HEIGHT && grid[x + 1][y + 1] == CellType::Empty){
          if(CELLTYPE != CellType::Empty){
            grid[x][y] = CellType::Empty;
            grid[x + 1][y + 1] = CELLTYPE;

            updated[x][y] = true;
            updated[x + 1][y + 1] = true;
            continue;
          }
        }
      }
        else{
          if(x - 1 > 0 && y + 1 < HEIGHT && grid[x - 1][y + 1] == CellType::Empty){
            if(CELLTYPE != CellType::Empty){
              grid[x][y] = CellType::Empty;
              grid[x - 1][y + 1] = CELLTYPE;

              updated[x][y] = true;
              updated[x - 1][y + 1] = true;
              continue;
            }
          }

        }
      }

  if(updated[x][y]) continue;
  if(CELLTYPE == CellType::Fire){
    if(rightFirst){
      if(x + 1 < WIDTH && y - 1 > 0 && grid[x + 1][y - 1] == CellType::Empty){
        grid[x][y] = CellType::Empty;
        grid[x + 1][y - 1] = CELLTYPE;

        updated[x][y] = true;
        updated[x + 1][y - 1] = true;
        continue;
      }
    }
    else{
      if(x - 1 > 0 && y - 1 > 0 && grid[x - 1][y - 1] == CellType::Empty){
        grid[x][y] = CellType::Empty;
        grid[x - 1][y - 1] = CELLTYPE;

        updated[x][y] = true;
        updated[x - 1][y - 1] = true;
        continue;
      }
    }
  }
  bool leftFirst = rand() & 1;
  if(CELLTYPE == CellType::Oil){
    if((rand() % 100) < 65){
      continue;
    }
  }
  //check sideways for liquids

  if(updated[x][y]) continue;
      if(CELLTYPE == CellType::Water || CELLTYPE == CellType::Oil || CELLTYPE == CellType::Fire){
        if(leftFirst){
          if(x - 1 >= 0 && grid[x - 1][y] == CellType::Empty){
            grid[x][y] = CellType::Empty;
            grid[x - 1][y] = CELLTYPE;

            updated[x][y] = true;
            updated[x - 1][y] = true;
            continue;
          }
        }
        else{
          if(x + 1 < WIDTH && grid[x + 1][y] == CellType::Empty){
            grid[x][y] = CellType::Empty;
            grid[x + 1][y] = CELLTYPE;

            updated[x][y] = true;
            updated[x + 1][y] = true;
            continue;
          }
        }
      }
    }
  }
}


void calculateHeatTransfer(CellType grid[WIDTH][HEIGHT], int temperatureMap[WIDTH][HEIGHT], int x, int y){
      if(grid[x][y] == CellType::Empty) return;
      int ny = y - 1;
      if (ny < 0 || grid[x][ny] != CellType::Fire){
        return;
      }
      double k = 0.6;
      double heatCapacity = 10.0;

      double T_sand = temperatureMap[x][y];
      double T_fire = temperatureMap[x][ny];

      double Q = k * (T_fire - T_sand);

      // apply heat
      temperatureMap[x][y]  += Q / heatCapacity;
      temperatureMap[x][ny] -= Q / heatCapacity;
}


void swapLiquids(CellType grid[WIDTH][HEIGHT], bool updated[WIDTH][HEIGHT], int x, int y){
  /*int dir;
  if((rand() % 100) < 50){
    dir = 1;
  }
  else{
    dir = -1;
  }
if(CELLTYPE != CellType::Fire){
      if(rightFirst){
        if(CELLTYPE != CellType::Empty && + 1 < WIDTH && y + 1 < HEIGHT && grid[x + 1][y + 1] == CellType::Empty){
          if(CELLTYPE != CellType::Empty){
            grid[x][y] = CellType::Empty;
            grid[x + 1][y + 1] = CELLTYPE;
            continue;
          }
        }
      }
        else{
          if(x - 1 > 0 && y + 1 < HEIGHT && grid[x - 1][y + 1] == CellType::Empty){
            if(CELLTYPE != CellType::Empty){
              grid[x][y] = CellType::Empty;
              grid[x - 1][y + 1] = CELLTYPE;
              continue;
            }
          }

        }
      }
      */

  //swaps water and oil
  if((rand() % 100) < 55){
    return;
  }

  if(y + 1 < HEIGHT && (grid[x][y] == CellType::Water) && (grid[x][y + 1] == CellType::Oil)){
    grid[x][y] = CellType::Oil;
    grid[x][y + 1] = CellType::Water;

    return;
  }
  else if(x + 1 < WIDTH && y + 1 < HEIGHT && (grid[x][y] == CellType::Water && grid[x + 1][y + 1] == CellType::Oil)){
    grid[x][y] = CellType::Oil;
    grid[x + 1][y + 1] = CellType::Water;

    return;
  }
  else if(x - 1 > 0 && y + 1 < HEIGHT && (grid[x][y] == CellType::Water && grid[x - 1][y + 1] == CellType::Oil)){
    grid[x][y] = CellType::Oil;
    grid[x - 1][y + 1] = CellType::Water;

    return;
  }
  else if(x - 1 > 0 && (grid[x][y] == CellType::Water && grid[x - 1][y] == CellType::Oil)){
    grid[x][y] = CellType::Oil;
    grid[x - 1][y] = CellType::Water;

    return;
  }
  else if(x + 1 < WIDTH && (grid[x][y] == CellType::Water && grid[x + 1][y] == CellType::Oil)){
    grid[x][y] = CellType::Oil;
    grid[x + 1][y] = CellType::Water;

    return;
  }
}


void swapPowdersAndLiquids(CellType grid[WIDTH][HEIGHT], bool updated[WIDTH][HEIGHT], int x, int y){
  int dir;
  if((rand() % 100) < 50){
    dir = 1;
  }
  else{
    dir = -1;
  }
  
  if(updated[x][y]) return;
  //swaps sand and liquids
  if((rand() % 100) < 55){
    return;
  }
  if(grid[x][y] != CellType::Sand && grid[x][y] != CellType::WetSand) return;
  if(y + 1 >= HEIGHT) return;
  if (grid[x][y] == CellType::Sand || grid[x][y] == CellType::WetSand) {

    if (y + 1 < HEIGHT) {

      CellType below = grid[x][y + 1];

      if (below == CellType::Water || below == CellType::Oil) {
        grid[x][y + 1] = CellType::WetSand;
        grid[x][y]     = below;
        return;
      }
    }
  }

  if(dir == 1 && x + 1 > WIDTH) return;
  if(dir == -1 && x + -1 < 0) return;

  if(grid[x + dir][y + 1] == CellType::Water || grid[x + dir][y + 1] == CellType::Oil){
    CellType tempCell = grid[x + dir][y + 1];

    grid[x][y] = tempCell;
    grid[x + dir][y + 1] = CellType::WetSand;
  }
}

void updateWetSand(CellType grid[WIDTH][HEIGHT], int x, int y){
  //turns sand into wet sand if its touching water
  if(grid[x][y] == CellType::Sand){

    if((y + 1 < HEIGHT && grid[x][y + 1] == CellType::Water || y + 1 < WIDTH && grid[x][y + 1] == CellType::Oil)){
      grid[x][y] = CellType::WetSand;
      grid[x][y + 1] = CellType::Empty;
    }
    if((y - 1 > 0 && grid[x][y - 1] == CellType::Water || y - 1 > 0 && grid[x][y - 1] == CellType::Oil)){
      grid[x][y] = CellType::WetSand;
      grid[x][y - 1] = CellType::Empty;
    }
    if((x + 1 < WIDTH && grid[x + 1][y] == CellType::Water || x + 1 < WIDTH && grid[x + 1][y] == CellType::Oil)){
      grid[x][y] = CellType::WetSand;
      grid[x + 1][y] = CellType::Empty;
    }
    if((x - 1 > 0 && grid[x - 1][y] == CellType::Water || x - 1 > 0 && grid[x - 1][y] == CellType::Oil)){
      grid[x][y] = CellType::WetSand;
      grid[x - 1][y] = CellType::Empty;
    }
  }
}

