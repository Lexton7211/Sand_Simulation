#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <algorithm>
#include <cmath>





//still have to implemnt sand settling


const int WIDTH  = 175;
const int HEIGHT = 175;

void updateParticle(int grid[][HEIGHT], int &gridX, int &gridY, int cellID, bool &moving, bool &settled, int &oldGridX, int &oldGridY);




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

SDL_Color sandColor = {255, 211, 106, 255};
SDL_Color waterColor = {41, 104, 255, 200};


//make grid
//if a cell = 0 it is empty and if it = 1 it has a sand cell in it 
//if a cell = 2 it has a water cell in it

const int cellSize = 4;
int grid[WIDTH][HEIGHT] = {0};


struct sandCell{
    int gridX;
    int gridY;
    int oldGridX;
    int oldGridY;
    int cellID = 1;
    bool moving;
    bool settled;
};

struct waterCell {
    int gridX;
    int gridY;
    int oldGridX;
    int oldGridY;
    int cellID = 2;
    bool moving;

    void updateWater(int grid[][HEIGHT]){
      //check down
      if(gridY + 1 < HEIGHT && grid[gridX][gridY + 1] == 0){
        moving = true;
      }


      //check down-left and down-right
      bool leftFirst = rand() & 1;

      int dirs[2] = {-1, 1};
      if (!leftFirst) std::swap(dirs[0], dirs[1]);

      for (int d : dirs) {
        int nx = gridX + d;
        int ny = gridY + 1;
        if (nx >= 0 && nx < WIDTH && ny < HEIGHT &&
          grid[nx][ny] == 0) {
          moving = true;
          return;
        }
      }

      // Sideways flow
      for (int d : dirs) {
        int nx = gridX + d;
        if (nx >= 0 && nx < WIDTH &&
          grid[nx][gridY] == 0) {

          
          moving = true;
          return;
        }
      }

    }
};

std::vector<sandCell> sandCells;
std::vector <waterCell> waterCells;
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
                  waterCells.push_back({x, y, x, y, 2, true});
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
                  sandCells.push_back({x, y, x, y, 1, true, false});
                }
              }
            }
          }
        }

        }
    }
}

//gravity
for (auto& sand : sandCells) {

    sand.moving = false;

        
    
        if (rand() & 1) {
          updateParticle(grid, sand.gridX, sand.gridY, sand.cellID, sand.moving, sand.settled, sand.oldGridX, sand.oldGridY);
        } else {
           updateParticle(grid, sand.gridX, sand.gridY, sand.cellID, sand.moving, sand.settled, sand.oldGridX, sand.oldGridY);
        }
    

    sand.gridX = std::clamp(sand.gridX, 0, WIDTH - 1);
    sand.gridY = std::clamp(sand.gridY, 0, HEIGHT - 1);


}


for (auto& water : waterCells) {
    grid[water.oldGridX][water.oldGridY] = 0;

    water.moving = false;

    if (water.gridY < HEIGHT - 1 && grid[water.gridX][water.gridY + 1] == 0) {
        water.moving = true;
    } else {
        water.updateWater(grid);
    }

    water.gridX = std::clamp(water.gridX, 0, WIDTH - 1);
    water.gridY = std::clamp(water.gridY, 0, HEIGHT - 1);


    water.oldGridX = water.gridX;
    water.oldGridY = water.gridY;
}


for(auto& sand : sandCells){
  int belowY = sand.gridY + 1;
  if(belowY < HEIGHT && grid[sand.gridX][belowY] == 2){
    for(auto& water : waterCells){
      if(water.gridX == sand.gridX && water.gridY == belowY){
        std::swap(water.gridY, sand.gridY);
        sand.moving = true;
        break;
      }
    }
  }
}



SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
SDL_RenderClear(renderer);
SDL_SetRenderDrawColor(renderer, sandColor.r, sandColor.g, sandColor.b, sandColor.a);
for(auto& sand : sandCells){
  int pixelX = sand.gridX * cellSize;
  int pixelY = sand.gridY * cellSize;
  SDL_Rect rect = { pixelX, pixelY, cellSize, cellSize };

  SDL_RenderFillRect(renderer, &rect);
}

SDL_SetRenderDrawColor(renderer, waterColor.r, waterColor.g, waterColor.b, waterColor.a);
for(auto& water : waterCells){
  int pixelX = water.gridX * cellSize;
  int pixelY = water.gridY * cellSize;
  SDL_Rect rect = { pixelX, pixelY, cellSize, cellSize };

  SDL_RenderFillRect(renderer, &rect);
}

    SDL_RenderPresent(renderer);
    SDL_Delay(16); // ~60 FPS
}
SDL_DestroyRenderer(renderer);
SDL_DestroyWindow(window);
SDL_Quit();
  return 0;
}



  void updateParticle(int grid[][HEIGHT], int &gridX, int &gridY, int cellID, bool &moving, bool &settled, int &oldGridX, int &oldGridY){
  bool allowedToMove;
  int xOffset = 0;
  int yOffset = 0;
      if (gridY < HEIGHT - 1 && grid[gridX][gridY + 1] == 0){
        yOffset += 1;
        moving = true;
      }
  //check below
  if(gridY + 1 >= HEIGHT){
    moving = false;
  }
  else if(grid[gridX][gridY + 1] == 1){
    moving = false;
  }

  if (settled && gridY + 1 < HEIGHT &&
    grid[gridX][gridY + 1] == 0) {
    settled = false;
    moving = true;
    return;
  }

  if (!moving) {
    settled = true;
  }

 //check left
 // before sliding
  if ((rand() % 100) < 70) {
    return; // 70% chance to not slide this frame
  }

  if (!moving && gridX > 0 && gridY + 1 < HEIGHT && grid[gridX - 1][gridY + 1] == 0) {
    moving = true;
    xOffset = -1;
    yOffset = 1;
  }
  if (!moving) {
    settled = true;
  }

  //check right
  if (!moving && gridX < WIDTH - 1 && gridY + 1 < HEIGHT && grid[gridX + 1][gridY + 1] == 0) {
    moving = true;
    xOffset = 1;
    yOffset = 1;
  }
  if (!moving) {
    settled = true;
  }



 //check if allowedToMove
 if(grid[gridX + xOffset][gridY + yOffset] == 0){
    allowedToMove = true;
  }
  else{
    allowedToMove = false;
  }
  if(!allowedToMove){
    return;
  }

  //update particale location
  gridX += xOffset;
  gridY += yOffset;

  grid[gridX][gridY] = cellID;
  grid[oldGridX][oldGridY] = 0;

  oldGridX = gridX;
  oldGridY = gridY;
}

