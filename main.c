#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ctype.h>

//include local files

//must be listed to define variables
#include "src/UI/constants.h"
#include "src/UI/types.h"

#include "src/UI/ui_draw.h"
#include "src/core/core_game.h"
#include "src/core/core_storage.h"
#include "src/core/core_game_score.h"

int main() {
    InitWindow(SCREEN_W, SCREEN_H, "Pickleball Score Tracker");
    SetTargetFPS(60);

    //Set all values as 0
    Appstate state = {0};

    //Initializaing visuals
    state.ball_y = 0.25f;
    state.player1_x = -22.0f;
    state.player1_z = -4.0f;
    state.player2_x = 22.0f;
    state.player2_z = 4.0f;

    // Pass the reference of the state to functions
    StartNewGame(&state);
    LoadHistory(&state);
    SearchMatches(&state);
    
    while (!WindowShouldClose() && !state.should_close) {
        // Update Timer
        if (state.game.timer_on && !state.game.game_over) {
        state.game_timer += GetFrameTime();
        }

        BeginDrawing();
        ClearBackground(SKYBLUE);
        
        if (state.screen == 0) DrawMainMenu(&state);
        else if (state.screen == 1) DrawGameScreen(&state);
        else if (state.screen == 2) DrawHistoryScreen(&state);
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}