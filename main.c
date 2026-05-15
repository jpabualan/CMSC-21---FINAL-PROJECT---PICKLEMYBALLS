#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ctype.h>  // for tolower()

//include local files
#include "src/Function_Prototypes/constants.h"
#include "src/Function_Prototypes/types.h"
#include "src/Function_Prototypes/game_fx_prototypes.h"
#include "src/Function_Prototypes/scoring_fx_prototypes.h"
#include "src/Function_Prototypes/draw_fx_prototypes.h"

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
    
    while (!WindowShouldClose()) {
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