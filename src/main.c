#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ctype.h>  // for tolower()

//include local files
#include "constants.h"
#include "types.h"
#include "Function_Prototypes/game_fx_prototypes.h"
#include "Function_Prototypes/scoring_fx_prototypes.h"
#include "Function_Prototypes/draw_fx_prototypes.h"

// GLOBAL VARIABLES
Match game = {0};
SavedMatch saved[MAX_SAVED];
int saved_count = 0;
int screen = 0; // 0=main menu, 1=game, 2=history
char p1_name[MAX_NAME] = "";  // EMPTY, user will type
char p2_name[MAX_NAME] = "";  // EMPTY, user will type
int typing = 0;
int typing_who = 0; // 0=player1, 1=player2
char typing_buffer[MAX_NAME] = "";
float game_timer = 0;
int name_error = 0;  // show error if name is empty

// Search variables
char search_name[MAX_NAME] = "";
int searching = 0;
int search_typing = 0;
char search_buffer[MAX_NAME] = "";
int search_results[MAX_SAVED];
int search_result_count = 0;

// visuals
float ball_x = 0, ball_y = 0.25f, ball_z = 0;
float player1_x = -22.0f, player1_z = -4.0f;
float player2_x = 22.0f, player2_z = 4.0f;

// ======================= MAIN =======================
int main() {
    InitWindow(SCREEN_W, SCREEN_H, "Pickleball Score Tracker");
    SetTargetFPS(60);
    
    StartNewGame();
    LoadHistory();
    SearchMatches();
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(SKYBLUE);
        
        if (screen == 0) DrawMainMenu();
        else if (screen == 1) DrawGameScreen();
        else if (screen == 2) DrawHistoryScreen();
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}