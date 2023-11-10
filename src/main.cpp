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
    int move = 0;
    float score[2] = {0, 0};

    std::cout << "[INFO] Loading SDL...\n";

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

    // Rects
    SDL_Rect pointerRect = {0, gridY - 70, 100, 50}; // Pointer rect
    SDL_Rect winnerDisplayRect = {gridX, gridY - (8 * winnerDisplayRectScale) - 30, 0, 8 * winnerDisplayRectScale}; // Winner display rect
    SDL_Rect thinkingRect = {gridX + gridWidth + 10, gridY, 118, 22}; // "Thinking..." rect

    // Mouse coordinates
    int mouseX = 0;
    int mouseY = 0;

    std::cout << "[INFO] Loading textures...\n";
    std::cout << "[INFO] Score: [" << score[0] << " - " << score[1] << "]\n";

    // Load surfaces, textures, and fonts
    SDL_Surface* imageSurface = nullptr;

    imageSurface = IMG_Load("/home/maxave/projects/Connect-4-Desktop/assets/images/slot.png");
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

    imageSurface = IMG_Load("/home/maxave/projects/Connect-4-Desktop/assets/images/thinking.png");
    SDL_Texture* thinkingTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_FreeSurface(imageSurface);

    SDL_Event event; // SDL event
    bool isRunning = true;

    bool computerMove = false;

    // Mouse press state
    bool mouseDown = false;

    std::cout << "[INFO] Started new game\n";

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
                    switch(winner)
                    {
                        case 1:
                        score[0]++;
                        break;
                        case -1:
                        score[1]++;
                        break;
                        case 0:
                        score[0] += 0.5f;
                        score[1] += 0.5f;
                        break;
                    }
                    move = 0;
                    selectedColumn = 0;
                    player1Turn = true;
                    mainBoard = {{0ULL, 0ULL}, 0x1};
                    winner = 0;
                    std::cout << "[INFO] Started new game\n";
                    std::cout << "[INFO] Score: [" << score[0] << " - " << score[1] << "]\n";
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

        // "Thinking..."
        if(computerMove) {
            SDL_RenderCopy(renderer, thinkingTexture, nullptr, &thinkingRect);
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
            move++;
            const int depth = 10;
            std::cout << "[INFO] Thinking... (depth=" << depth << ")\n";
            const search::Eval position_eval = search::minimax(mainBoard, false, depth, INT32_MIN, INT32_MAX);
            std::cout << "[INFO] Move: " << move << "\n";
            std::cout << "[INFO] Eval: " << position_eval.eval << "\n";
            std::cout << "[INFO] Best move: " << (int)position_eval.move << "\n";
            std::cout << "-------------------\n";
            bitboards::place_disc(mainBoard, position_eval.move);
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

    std::cout << "[INFO] Cleaning up...\n";

    SDL_DestroyTexture(slotTexture);
    SDL_DestroyTexture(redPointerTexture);
    SDL_DestroyTexture(yellowPointerTexture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << "[INFO] Score: [" << score[0] << " - " << score[1] << "]\n";
    std::cout << "Session finished.\n";

    return 0;
}
