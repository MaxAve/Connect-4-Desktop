#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>
#include "../include/bitboard.h"
#include "../include/search.h"

int main(int argc, char* argv[]) {
    srand(time(NULL));

    // Game data
    int selectedColumn = 0;
    bool player1Turn = true;
    bitboards::Board mainBoard = {{0ULL, 0ULL}, 0x1};
    bitboards::load_win_masks();
    int winner = 0;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Connect 4", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load connect 4 grid
    std::vector<SDL_Rect*> gridRects;

    const int slotSize = 129;
    const int gridWidth = slotSize * 7;
    const int gridHeight = slotSize * 6;
    SDL_DisplayMode currentDisplayMode;
    SDL_GetCurrentDisplayMode(0, &currentDisplayMode);
    const int gridX = currentDisplayMode.w/2 - gridWidth/2;
    const int gridY = currentDisplayMode.h/2 - gridHeight/2;

    for(int y = 0; y < 6; y++) {
        for(int x = 0; x < 7; x++) {
            gridRects.push_back(new SDL_Rect({x * slotSize + gridX, y * slotSize + gridY, slotSize, slotSize}));
        }
    }

    const int winnerDisplayRectScale = 4;

    SDL_Rect pointerRect = {0, gridY - 70, 100, 50}; // Pointer rect
    SDL_Rect winnerDisplayRect = {gridX, gridY - (8 * winnerDisplayRectScale) - 30, 0, 8 * winnerDisplayRectScale}; // Winner display rect

    // Mouse coordinates
    int mouseX = 0;
    int mouseY = 0;

    // Load surfaces, textures, and fonts
    SDL_Surface* imageSurface = IMG_Load("/home/maxave/projects/Connect-4-Desktop/assets/images/slot.png");
    SDL_Texture* slotTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_FreeSurface(imageSurface);

    imageSurface = IMG_Load("/home/maxave/projects/Connect-4-Desktop/assets/images/slot_with_red_disc.png");
    SDL_Texture* slotRedTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_FreeSurface(imageSurface);

    imageSurface = IMG_Load("/home/maxave/projects/Connect-4-Desktop/assets/images/slot_with_yellow_disc.png");
    SDL_Texture* slotYellowTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_FreeSurface(imageSurface);

    imageSurface = IMG_Load("/home/maxave/projects/Connect-4-Desktop/assets/images/pointer_red.png");
    SDL_Texture* redPointerTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_FreeSurface(imageSurface);

    imageSurface = IMG_Load("/home/maxave/projects/Connect-4-Desktop/assets/images/pointer_yellow.png");
    SDL_Texture* yellowPointerTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_FreeSurface(imageSurface);

    imageSurface = IMG_Load("/home/maxave/projects/Connect-4-Desktop/assets/images/red_win.png");
    SDL_Texture* redWinTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_FreeSurface(imageSurface);

    imageSurface = IMG_Load("/home/maxave/projects/Connect-4-Desktop/assets/images/yellow_win.png");
    SDL_Texture* yellowWinTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_FreeSurface(imageSurface);

    imageSurface = IMG_Load("/home/maxave/projects/Connect-4-Desktop/assets/images/draw.png");
    SDL_Texture* drawTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_FreeSurface(imageSurface);

    SDL_Event event; // SDL event
    bool isRunning = true;

    bool computerMove = false;

    // Mouse press state
    bool mouseDown = false;

    // Game loop
    while (isRunning) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }

            // Mouse click
            if(event.type == SDL_MOUSEBUTTONDOWN) {
                if(!mouseDown) {
                    mouseDown = true;
                }
            }

            // Mouse release
            if(event.type == SDL_MOUSEBUTTONUP) {
                // Player move
                if(winner == 0 && mouseDown && bitboards::get_top(mainBoard, selectedColumn) < 6) {
                    bitboards::place_disc(mainBoard, selectedColumn);
                    winner = bitboards::get_winner(mainBoard);
                    if(winner == 1)
                        winnerDisplayRect.w = 64 * winnerDisplayRectScale;
                    else if(winner == -1)
                        winnerDisplayRect.w = 89 * winnerDisplayRectScale;
                    else if(winner == 0 && (mainBoard.bitboards[0] | mainBoard.bitboards[1]) == FULL)
                        winnerDisplayRect.w = 35 * winnerDisplayRectScale;
                    player1Turn = !player1Turn;
                    mouseDown = false;
                    computerMove = true;
                }
            }

            // Key press
            if(event.type == SDL_KEYDOWN)
            {
                // C
                if(event.key.keysym.sym == SDL_KeyCode::SDLK_c)
                {
                    // Clear board and restart game
                    selectedColumn = 0;
                    player1Turn = true;
                    mainBoard = {{0ULL, 0ULL}, 0x1};
                    winner = 0;
                }
            }
        }

        // Get mouse position
        SDL_GetGlobalMouseState(&mouseX, &mouseY);

        // Update selected column
        selectedColumn = (mouseX - gridX - slotSize/2) / slotSize;

        // Update pointer position
        if(selectedColumn >= 0 && selectedColumn < 7) {
            pointerRect.x = selectedColumn * slotSize + gridX + (slotSize - pointerRect.w) / 2;
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Pointer
        if(!computerMove && winner == 0 && selectedColumn >= 0 && selectedColumn < 7) {
            SDL_RenderCopy(renderer, player1Turn ? redPointerTexture : yellowPointerTexture, nullptr, &pointerRect);
        }

        // Grid slots
        for(int i = 0; i < gridRects.size(); i++) {
            SDL_RenderCopy(renderer, (mainBoard.bitboards[0] & (1ULL << (41-i))) ? slotRedTexture : (mainBoard.bitboards[1] & (1ULL << (41-i))) ? slotYellowTexture : slotTexture, nullptr, gridRects[i]);
        }

        // Winner
        switch(winner)
        {
            case 0:
            if((mainBoard.bitboards[0] | mainBoard.bitboards[1]) == FULL)
                SDL_RenderCopy(renderer, drawTexture, nullptr, &winnerDisplayRect);
            break;
            case 1:
            SDL_RenderCopy(renderer, redWinTexture, nullptr, &winnerDisplayRect);
            break;
            case -1:
            SDL_RenderCopy(renderer, yellowWinTexture, nullptr, &winnerDisplayRect);
            break;
        }

        SDL_RenderPresent(renderer);

        // CPU move
        if(computerMove)
        {
            bitboards::place_disc(mainBoard, search::minimax(mainBoard, false, 10, INT32_MIN, INT32_MAX).move);
            winner = bitboards::get_winner(mainBoard);
            if(winner == 1)
                winnerDisplayRect.w = 64 * winnerDisplayRectScale;
            else if(winner == -1)
                winnerDisplayRect.w = 89 * winnerDisplayRectScale;
            else if(winner == 0 && (mainBoard.bitboards[0] | mainBoard.bitboards[1]) == FULL)
                winnerDisplayRect.w = 35 * winnerDisplayRectScale;
            player1Turn = !player1Turn;
            computerMove = false;
        }
    }

    /*
    Final cleanup
    */

    SDL_DestroyTexture(slotTexture);
    SDL_DestroyTexture(redPointerTexture);
    SDL_DestroyTexture(yellowPointerTexture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
