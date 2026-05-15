#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ctype.h>  // for tolower()

#include "constants.h"
#include "types.h"
#include "src/Function_Prototypes/draw_fx_prototypes.h"

void DrawMainMenu(Appstate *state) {
    Vector2 mouse = GetMousePosition();
    int w = SCREEN_W;
    int h = SCREEN_H;
    
    DrawRectangle(0, 0, w, h, (Color){20, 30, 40, 255});
    
    DrawText("PICKLEBALL SCORE TRACKER", w/2 - MeasureText("PICKLEBALL SCORE TRACKER", 40)/2, 50, 40, GOLD);
    DrawText("Pickling Balls", w/2 - MeasureText("Pickling Balls", 30)/2, 100, 30, LIGHTGRAY);
    
    // Name box
    DrawRectangle(w/2 - 250, 150, 500, 150, Fade(BLACK, 0.7f));
    DrawRectangleLines(w/2 - 250, 150, 500, 150, GOLD);
    DrawText("ENTER PLAYER NAMES", w/2 - 100, 160, 20, GOLD);
    
    // Player 1 name input
    DrawText("Player 1:", w/2 - 220, 200, 18, BLUE);
    Rectangle p1_box = {w/2 - 140, 195, 200, 30};
    DrawRectangleRec(p1_box, Fade(WHITE, 0.2f));
    DrawRectangleLinesEx(p1_box, 2, (state->typing && state->typing_who == 0) ? YELLOW : LIGHTGRAY);
    
    if (strlen(state->p1_name) > 0) {
        DrawText(state->p1_name, w/2 - 130, 202, 18, WHITE);
    } else if (!state->typing || state->typing_who != 0) {
        DrawText("Type name here...", w/2 - 130, 202, 18, (Color){150, 150, 150, 255});
    }
    
    // Player 2 name input
    DrawText("Player 2:", w/2 - 220, 240, 18, RED);
    Rectangle p2_box = {w/2 - 140, 235, 200, 30};
    DrawRectangleRec(p2_box, Fade(WHITE, 0.2f));
    DrawRectangleLinesEx(p2_box, 2, (state->typing && state->typing_who == 1) ? YELLOW : LIGHTGRAY);
    
    if (strlen(state->p2_name) > 0) {
        DrawText(state->p2_name, w/2 - 130, 242, 18, WHITE);
    } else if (!state->typing || state->typing_who != 1) {
        DrawText("Type name here...", w/2 - 130, 242, 18, (Color){150, 150, 150, 255});
    }
    
    // Click to edit
    if (CheckCollisionPointRec(mouse, p1_box) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        state->typing = 1;
        state->typing_who = 0;
        strcpy(state->typing_buffer, state->p1_name);
    }
    if (CheckCollisionPointRec(mouse, p2_box) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        state->typing = 1;
        state->typing_who = 1;
        strcpy(state->typing_buffer, state->p2_name);
    }
    
    // Handle state->typing
    if (state->typing) {
        int key = GetCharPressed();
        while (key > 0) {
            if (key >= 32 && key <= 126 && strlen(state->typing_buffer) < MAX_NAME - 1) {
                int len = strlen(state->typing_buffer);
                state->typing_buffer[len] = (char)key;
                state->typing_buffer[len + 1] = '\0';
            }
            key = GetCharPressed();
        }
        
        if (IsKeyPressed(KEY_BACKSPACE)) {
            int len = strlen(state->typing_buffer);
            if (len > 0) state->typing_buffer[len - 1] = '\0';
        }
        
        if (IsKeyPressed(KEY_ENTER)) {
            if (state->typing_who == 0) {
                strcpy(state->p1_name, state->typing_buffer);
            } else {
                strcpy(state->p2_name, state->typing_buffer);
            }
            state->typing = 0;
            state->name_error = 0;
        }
        
        char display_text[MAX_NAME + 1];
        strcpy(display_text, state->typing_buffer);
        
        if ((int)(GetTime() * 2) % 2 == 0) {
            int len = strlen(display_text);
            display_text[len] = '_';
            display_text[len + 1] = '\0';
        }
        
        int cursor_y = (state->typing_who == 0) ? 202 : 242;
        DrawText(display_text, w/2 - 130, cursor_y, 18, YELLOW);
    }
    
    if (strlen(state->p1_name) == 0 || strlen(state->p2_name) == 0) {
        DrawText("Please enter both player names!", w/2 - 150, 310, 16, RED);
    }
    
    Rectangle start = {w/2 - 150, 340, 300, 60};
    Color start_color = (strlen(state->p1_name) > 0 && strlen(state->p2_name) > 0) ? BLUE : DARKGRAY;
    DrawRectangleRec(start, start_color);
    DrawRectangleLinesEx(start, 2, WHITE);
    DrawText("START MATCH", start.x + 85, start.y + 18, 24, WHITE);
    
    if (CheckCollisionPointRec(mouse, start) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && strlen(state->p1_name) > 0 && strlen(state->p2_name) > 0) {
        StartNewGame(state);
        state->screen = 1;
        state->typing = 0;
    }
    
    Rectangle hist = {w/2 - 150, 420, 300, 60};
    DrawRectangleRec(hist, CheckCollisionPointRec(mouse, hist) ? GREEN : DARKGREEN);
    DrawRectangleLinesEx(hist, 2, WHITE);
    DrawText("MATCH HISTORY", hist.x + 65, hist.y + 18, 24, WHITE);
    if (CheckCollisionPointRec(mouse, hist) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        LoadHistory(state);
        strcpy(state->search_name, "");
        SearchMatches(state);
        state->screen = 2;
    }
    
    Rectangle exit = {w/2 - 150, 500, 300, 60};
    DrawRectangleRec(exit, CheckCollisionPointRec(mouse, exit) ? RED : MAROON);
    DrawRectangleLinesEx(exit, 2, WHITE);
    DrawText("EXIT", exit.x + 130, exit.y + 18, 24, WHITE);
    if (CheckCollisionPointRec(mouse, exit) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        CloseWindow();
    }
    
    DrawText("Click on name box to type | Press ENTER to save", w/2 - 190, 590, 16, LIGHTGRAY);
}
