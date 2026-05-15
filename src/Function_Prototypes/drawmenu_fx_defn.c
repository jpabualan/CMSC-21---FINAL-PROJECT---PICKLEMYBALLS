#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ctype.h>  // for tolower()

#include "constants.h"
#include "types.h"
#include "Function_Prototypes/draw_fx_prototypes.h"

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
        StartNewGame();
        state->screen = 1;
        state->typing = 0;
    }
    
    Rectangle hist = {w/2 - 150, 420, 300, 60};
    DrawRectangleRec(hist, CheckCollisionPointRec(mouse, hist) ? GREEN : DARKGREEN);
    DrawRectangleLinesEx(hist, 2, WHITE);
    DrawText("MATCH HISTORY", hist.x + 65, hist.y + 18, 24, WHITE);
    if (CheckCollisionPointRec(mouse, hist) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        LoadHistory();
        strcpy(state->search_name, "");
        SearchMatches();
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

// Game screen
void DrawGameScreen(Appstate *state) {
    Vector2 mouse = GetMousePosition();
    int w = SCREEN_W;
    int h = SCREEN_H;
    int court_w = w / 2;
    
    // Update timer
    if (state->game.timer_on && !state->game.game_over) {
        state->game_timer += GetFrameTime();
    }
    
    // Left side - Court
    DrawRectangle(0, 0, court_w, h, (Color){30, 50, 70, 255});
    
    Camera3D cam = {0};
    cam.position = (Vector3){25, 18, 25};
    cam.target = (Vector3){0, 0, 0};
    cam.up = (Vector3){0, 1, 0};
    cam.fovy = 55;
    
    DrawCourt(state, cam);
    
    // Right side - UI Panel
    DrawRectangle(court_w, 0, w - court_w, h, (Color){40, 40, 60, 240});
    
    DrawText("MATCH TRACKER", court_w + 20, 20, 28, GOLD);
    
    int mins = (int)state->game_timer / 60;
    int secs = (int)state->game_timer % 60;
    DrawText(TextFormat("TIME: %02d:%02d", mins, secs), court_w + 20, 60, 20, LIGHTGRAY);
    
    // Scores - PLAYER 1 ON LEFT SIDE OF UI
    DrawText(state->game.p1.name, court_w + 20, 100, 24, BLUE);
    DrawText(TextFormat("%d", state->game.p1.score), court_w + 20, 135, 48, BLUE);
    
    // Scores - PLAYER 2 ON RIGHT SIDE OF UI
    int right_side_x = court_w + (w - court_w) - 200;
    DrawText(state->game.p2.name, right_side_x, 100, 24, RED);
    DrawText(TextFormat("%d", state->game.p2.score), right_side_x, 135, 48, RED);
    
    // Stats - Player 1 on left
    DrawText("STATS", court_w + 20, 290, 20, GOLD);
    DrawText(TextFormat("Faults: %d", state->game.p1.faults), court_w + 20, 320, 16, WHITE);
    DrawText(TextFormat("Aces: %d", state->game.p1.aces), court_w + 20, 340, 16, WHITE);
    DrawText(TextFormat("Outs: %d", state->game.p1.outs), court_w + 20, 360, 16, WHITE);
    
    // Stats - Player 2 on right
    DrawText(TextFormat("Faults: %d", state->game.p2.faults), right_side_x, 320, 16, WHITE);
    DrawText(TextFormat("Aces: %d", state->game.p2.aces), right_side_x, 340, 16, WHITE);
    DrawText(TextFormat("Outs: %d", state->game.p2.outs), right_side_x, 360, 16, WHITE);
    
    // PLAYER 1 BUTTONS (LEFT SIDE)
    DrawText("PLAYER 1 CONTROLS", court_w + 20, 400, 18, BLUE);
    
    Rectangle p1_score = {court_w + 20, 420, 90, 35};
        DrawRectangleRec(p1_score, CheckCollisionPointRec(mouse, p1_score) ? DARKBLUE : BLUE);
        DrawText("+1", p1_score.x + 30, p1_score.y + 8, 18, WHITE);
            if (CheckCollisionPointRec(mouse, p1_score) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) AddPoint(state, 1);
    
    Rectangle p1_fault = {court_w + 120, 420, 90, 35};
        DrawRectangleRec(p1_fault, CheckCollisionPointRec(mouse, p1_fault) ? DARKGRAY : GRAY);
        DrawText("FLT", p1_fault.x + 30, p1_fault.y + 8, 16, WHITE);
            if (CheckCollisionPointRec(mouse, p1_fault) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) AddFault(state, 1);
    
    Rectangle p1_ace = {court_w + 220, 420, 90, 35};
        DrawRectangleRec(p1_ace, CheckCollisionPointRec(mouse, p1_ace) ? DARKBROWN : GOLD);
        DrawText("ACE", p1_ace.x + 28, p1_ace.y + 8, 16, BLACK);
            if (CheckCollisionPointRec(mouse, p1_ace) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) AddAce(state, 1);
    
    Rectangle p1_out = {court_w + 20, 465, 90, 35};
        DrawRectangleRec(p1_out, CheckCollisionPointRec(mouse, p1_out) ? DARKPURPLE : PURPLE);
        DrawText("OUT", p1_out.x + 30, p1_out.y + 8, 16, WHITE);
            if (CheckCollisionPointRec(mouse, p1_out) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) AddOut(state, 1);
    
    Rectangle p1_in = {court_w + 120, 465, 90, 35};
        DrawRectangleRec(p1_in, CheckCollisionPointRec(mouse, p1_in) ? DARKGREEN : GREEN);
        DrawText("IN", p1_in.x + 33, p1_in.y + 8, 16, WHITE);
            if (CheckCollisionPointRec(mouse, p1_in) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) AddIn(state, 1);
    
    // PLAYER 2 BUTTONS (RIGHT SIDE)
    DrawText("PLAYER 2 CONTROLS", right_side_x, 400, 18, RED);
    
    Rectangle p2_score = {right_side_x, 420, 90, 35};
        DrawRectangleRec(p2_score, CheckCollisionPointRec(mouse, p2_score) ? MAROON : RED);
        DrawText("+1", p2_score.x + 30, p2_score.y + 8, 18, WHITE);
            if (CheckCollisionPointRec(mouse, p2_score) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) AddPoint(state, 2);
    
    Rectangle p2_fault = {right_side_x + 100, 420, 90, 35};
        DrawRectangleRec(p2_fault, CheckCollisionPointRec(mouse, p2_fault) ? DARKGRAY : GRAY);
        DrawText("FLT", p2_fault.x + 30, p2_fault.y + 8, 16, WHITE);
            if (CheckCollisionPointRec(mouse, p2_fault) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) AddFault(state, 2);
    
    Rectangle p2_ace = {right_side_x + 200, 420, 90, 35};
        DrawRectangleRec(p2_ace, CheckCollisionPointRec(mouse, p2_ace) ? DARKBROWN : GOLD);
        DrawText("ACE", p2_ace.x + 28, p2_ace.y + 8, 16, BLACK);
            if (CheckCollisionPointRec(mouse, p2_ace) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) AddAce(state, 2);
    
    Rectangle p2_out = {right_side_x, 465, 90, 35};
        DrawRectangleRec(p2_out, CheckCollisionPointRec(mouse, p2_out) ? DARKPURPLE : PURPLE);
        DrawText("OUT", p2_out.x + 30, p2_out.y + 8, 16, WHITE);
            if (CheckCollisionPointRec(mouse, p2_out) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) AddOut(state, 2);
    
    Rectangle p2_in = {right_side_x + 100, 465, 90, 35};
        DrawRectangleRec(p2_in, CheckCollisionPointRec(mouse, p2_in) ? DARKGREEN : GREEN);
        DrawText("IN", p2_in.x + 33, p2_in.y + 8, 16, WHITE);
            if (CheckCollisionPointRec(mouse, p2_in) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) AddIn(state, 2);
    
    // UNDO BUTTON (centered)
    Rectangle undo = {court_w + (w - court_w)/2 - 60, 530, 120, 40};
        DrawRectangleRec(undo, CheckCollisionPointRec(mouse, undo) ? ORANGE : YELLOW);
        DrawRectangleLinesEx(undo, 2, WHITE);
        DrawText("UNDO", undo.x + 35, undo.y + 10, 18, BLACK);
            if (CheckCollisionPointRec(mouse, undo) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) UndoLastAction(state);
    
    // Control buttons
    Rectangle reset = {court_w + 20, 600, 120, 40};
        DrawRectangleRec(reset, CheckCollisionPointRec(mouse, reset) ? ORANGE : YELLOW);
        DrawText("RESET", reset.x + 35, reset.y + 10, 18, WHITE);
            if (CheckCollisionPointRec(mouse, reset) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) ResetGame(state);
    
    Rectangle back = {court_w + (w - court_w) - 140, 600, 140, 40};
        DrawRectangleRec(back, CheckCollisionPointRec(mouse, back) ? DARKGRAY : GRAY);
        DrawText("BACK", back.x + 45, back.y + 10, 18, WHITE);
            if (CheckCollisionPointRec(mouse, back) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) state->screen = 0;
    
    // Winner announcement
    if (state->game.game_over) {
        DrawRectangle(court_w + 20, h/2 - 80, w - court_w - 40, 120, Fade(BLACK, 0.85f));
        DrawRectangleLines(court_w + 20, h/2 - 80, w - court_w - 40, 120, GOLD);
        DrawText(TextFormat("%s WINS!", state->game.winner == 1 ? state->game.p1.name : state->game.p2.name), 
                 court_w + (w - court_w)/2 - 100, h/2 - 50, 26, GOLD);
        DrawText(TextFormat("Final Score: %d - %d", state->game.p1.score, state->game.p2.score), 
                 court_w + (w - court_w)/2 - 100, h/2 - 15, 20, WHITE);
    }
}