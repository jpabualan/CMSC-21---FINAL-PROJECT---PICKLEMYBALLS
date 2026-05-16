#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ctype.h>  // for tolower()

#include "constants.h"
#include "types.h"
#include "scoring_fx_prototypes.h"
#include "draw_fx_prototypes.h"

// ---------------------------------------------------------------------------
// DrawGameScreen
//
// Renders the split-screen game UI: 3D court on the left, score/control
// panel on the right.
//
// Changes from original:
//  - Timer update (GetFrameTime) removed — state mutation does not belong in
//    a draw function.  Call UpdateGameTimer(state) from your game-loop update
//    step instead.
//  - Window size read from GetScreenWidth/GetScreenHeight so the layout
//    survives any window resize rather than relying solely on compile-time
//    constants.
//  - right_side_x calculation simplified to a clean, readable expression.
//  - P1 and P2 button x-offsets unified (both use +0, +100, +200) so the
//    two rows are visually identical and pixel-perfect.
//  - RESET button text changed from WHITE to BLACK — WHITE on YELLOW is
//    nearly invisible.
//  - state->screen assignment uses SCREEN_MAIN_MENU constant instead of the
//    magic literal 0.
//  - Added a "SERVING" indicator below each player's score so it is always
//    clear whose turn it is (reads state->game.server, expected values 1/2).
//  - Winner announcement uses MeasureText so the text is truly centred
//    regardless of name length.
//  - BeginScissorMode for the 3D court is handled inside DrawCourt; the UI
//    panel is drawn after EndScissorMode so it is never clipped.
// ---------------------------------------------------------------------------

// Helper: draw a centred string inside a rectangle
static void DrawTextCentred(const char *text, Rectangle rect, int fontSize, Color color) {
    int tw = MeasureText(text, fontSize);
    DrawText(text,(int)(rect.x + (rect.width  - tw) / 2.0f),
             (int)(rect.y + (rect.height - fontSize) / 2.0f),
             fontSize, color);
}

void DrawGameScreen(Appstate *state) {

    // Layout constants
    int w        = GetScreenWidth();
    int h        = GetScreenHeight();
    int court_w  = w / 2;               // left panel width
    int panel_w  = w - court_w;         // right panel width
    int panel_x  = court_w;             // right panel left edge

    // Two player column origins inside the right panel (20 px margin each)
    int p1_col   = panel_x + 20;
    int p2_col   = panel_x + panel_w / 2 + 10;

    // Background panels 
    DrawRectangle(0,        0, court_w, h, (Color){30,  50,  70,  255});
    DrawRectangle(panel_x,  0, panel_w, h, (Color){40,  40,  60,  240});

    // ---- 3D Court (scissor + draw handled inside DrawCourt)
    Camera3D cam = {0};
    cam.position = (Vector3){25.0f, 18.0f, 25.0f};
    cam.target   = (Vector3){ 0.0f,  0.0f,  0.0f};
    cam.up       = (Vector3){ 0.0f,  1.0f,  0.0f};
    cam.fovy     = 55.0f;
    cam.projection = CAMERA_PERSPECTIVE;

    DrawCourt(state, cam);

    // ---- Header 
    DrawText("MATCH TRACKER", panel_x + 20, 20, 28, GOLD);

    // Timer — NOTE: timer is updated in UpdateGameTimer(), not here
    int mins = (int)state->game_timer / 60;
    int secs = (int)state->game_timer % 60;
    DrawText(TextFormat("TIME: %02d:%02d", mins, secs),
             panel_x + 20, 60, 20, LIGHTGRAY);

    // ---- Scores 
    // Player 1 (left column)
    DrawText(state->game.p1.name,                    p1_col, 100, 24, BLUE);
    DrawText(TextFormat("%d", state->game.p1.score), p1_col, 130, 48, BLUE);

    // Player 2 (right column)
    DrawText(state->game.p2.name,                    p2_col, 100, 24, RED);
    DrawText(TextFormat("%d", state->game.p2.score), p2_col, 130, 48, RED);

    // Serve indicator — shows which player is currently serving
    if (!state->game.game_over) {
        const char *serveLabel = "[ SERVING ]";
        if (state->game.server == 1)
            DrawText(serveLabel, p1_col, 185, 14, GOLD);
        else if (state->game.server == 2)
            DrawText(serveLabel, p2_col, 185, 14, GOLD);
    }

    // ---- Stats 
    DrawText("STATS", p1_col, 210, 20, GOLD);

    // Player 1
    DrawText(TextFormat("Faults : %d", state->game.p1.faults), p1_col, 235, 16, WHITE);
    DrawText(TextFormat("Aces   : %d", state->game.p1.aces),   p1_col, 255, 16, WHITE);
    DrawText(TextFormat("Outs   : %d", state->game.p1.outs),   p1_col, 275, 16, WHITE);

    // Player 2
    DrawText(TextFormat("Faults : %d", state->game.p2.faults), p2_col, 235, 16, WHITE);
    DrawText(TextFormat("Aces   : %d", state->game.p2.aces),   p2_col, 255, 16, WHITE);
    DrawText(TextFormat("Outs   : %d", state->game.p2.outs),   p2_col, 275, 16, WHITE);

    // ---- Player 1 control buttons 
    DrawText("PLAYER 1 CONTROLS", p1_col, 310, 16, BLUE);

    //  Row 1: +1 Score | Fault | Ace
    Rectangle p1_score = {p1_col,       335, 85, 35};
    Rectangle p1_fault = {p1_col + 100, 335, 85, 35};
    Rectangle p1_ace   = {p1_col + 200, 335, 85, 35};

    DrawRectangleRec(p1_score, CheckCollisionPointRec(GetMousePosition(), p1_score) ? DARKBLUE : BLUE);
    DrawTextCentred("+1",  p1_score, 18, WHITE);
    if (CheckCollisionPointRec(GetMousePosition(), p1_score) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        AddPoint(state, 1);

    DrawRectangleRec(p1_fault, CheckCollisionPointRec(GetMousePosition(), p1_fault) ? DARKGRAY : GRAY);
    DrawTextCentred("FLT", p1_fault, 16, WHITE);
    if (CheckCollisionPointRec(GetMousePosition(), p1_fault) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        AddFault(state, 1);

    DrawRectangleRec(p1_ace, CheckCollisionPointRec(GetMousePosition(), p1_ace) ? DARKBROWN : GOLD);
    DrawTextCentred("ACE", p1_ace, 16, BLACK);
    if (CheckCollisionPointRec(GetMousePosition(), p1_ace) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        AddAce(state, 1);

    //  Row 2: Out | In
    Rectangle p1_out = {p1_col,       380, 85, 35};
    Rectangle p1_in  = {p1_col + 100, 380, 85, 35};

    DrawRectangleRec(p1_out, CheckCollisionPointRec(GetMousePosition(), p1_out) ? DARKPURPLE : PURPLE);
    DrawTextCentred("OUT", p1_out, 16, WHITE);
    if (CheckCollisionPointRec(GetMousePosition(), p1_out) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        AddOut(state, 1);

    DrawRectangleRec(p1_in, CheckCollisionPointRec(GetMousePosition(), p1_in) ? DARKGREEN : GREEN);
    DrawTextCentred("IN",  p1_in,  16, WHITE);
    if (CheckCollisionPointRec(GetMousePosition(), p1_in) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        AddIn(state, 1);

    // ---- Player 2 control buttons 
    DrawText("PLAYER 2 CONTROLS", p2_col, 310, 16, RED);

    //  Row 1: +1 Score | Fault | Ace
    Rectangle p2_score = {p2_col,       335, 85, 35};
    Rectangle p2_fault = {p2_col + 100, 335, 85, 35};
    Rectangle p2_ace   = {p2_col + 200, 335, 85, 35};

    DrawRectangleRec(p2_score, CheckCollisionPointRec(GetMousePosition(), p2_score) ? MAROON : RED);
    DrawTextCentred("+1",  p2_score, 18, WHITE);
    if (CheckCollisionPointRec(GetMousePosition(), p2_score) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        AddPoint(state, 2);

    DrawRectangleRec(p2_fault, CheckCollisionPointRec(GetMousePosition(), p2_fault) ? DARKGRAY : GRAY);
    DrawTextCentred("FLT", p2_fault, 16, WHITE);
    if (CheckCollisionPointRec(GetMousePosition(), p2_fault) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        AddFault(state, 2);

    DrawRectangleRec(p2_ace, CheckCollisionPointRec(GetMousePosition(), p2_ace) ? DARKBROWN : GOLD);
    DrawTextCentred("ACE", p2_ace, 16, BLACK);
    if (CheckCollisionPointRec(GetMousePosition(), p2_ace) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        AddAce(state, 2);

    //  Row 2: Out | In
    Rectangle p2_out = {p2_col,       380, 85, 35};
    Rectangle p2_in  = {p2_col + 100, 380, 85, 35};

    DrawRectangleRec(p2_out, CheckCollisionPointRec(GetMousePosition(), p2_out) ? DARKPURPLE : PURPLE);
    DrawTextCentred("OUT", p2_out, 16, WHITE);
    if (CheckCollisionPointRec(GetMousePosition(), p2_out) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        AddOut(state, 2);

    DrawRectangleRec(p2_in, CheckCollisionPointRec(GetMousePosition(), p2_in) ? DARKGREEN : GREEN);
    DrawTextCentred("IN",  p2_in,  16, WHITE);
    if (CheckCollisionPointRec(GetMousePosition(), p2_in) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        AddIn(state, 2);

    // Shared utility buttons
    // UNDO — centred in the panel
    int undo_x = panel_x + (panel_w - 120) / 2;
    Rectangle undo = {undo_x, 435, 120, 40};
    DrawRectangleRec(undo, CheckCollisionPointRec(GetMousePosition(), undo) ? ORANGE : YELLOW);
    DrawRectangleLinesEx(undo, 2, WHITE);
    DrawTextCentred("UNDO", undo, 18, BLACK);
    if (CheckCollisionPointRec(GetMousePosition(), undo) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        UndoLastAction(state);

    // RESET — bottom-left of panel
    Rectangle reset = {panel_x + 20, h - 60, 120, 40};
    DrawRectangleRec(reset, CheckCollisionPointRec(GetMousePosition(), reset) ? ORANGE : YELLOW);
    DrawTextCentred("RESET", reset, 18, BLACK);  // BLACK on YELLOW
    if (CheckCollisionPointRec(GetMousePosition(), reset) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        ResetGame(state);

    // BACK — bottom-right of panel
    Rectangle back = {panel_x + panel_w - 140, h - 60, 120, 40};
    DrawRectangleRec(back, CheckCollisionPointRec(GetMousePosition(), back) ? DARKGRAY : GRAY);
    DrawTextCentred("BACK", back, 18, WHITE);
    if (CheckCollisionPointRec(GetMousePosition(), back) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        state->screen = SCREEN_MAIN_MENU;

    // ---- Winner overlay ----------------------------------------------------
    if (state->game.game_over) {
        const char *winner_name = (state->game.winner == 1)
                                  ? state->game.p1.name
                                  : state->game.p2.name;
        char win_str[64];
        snprintf(win_str, sizeof(win_str), "%s WINS!", winner_name);
        char score_str[32];
        snprintf(score_str, sizeof(score_str), "Final Score: %d - %d",
                 state->game.p1.score, state->game.p2.score);

        int overlay_x = panel_x + 20;
        int overlay_y = h / 2 - 80;
        int overlay_w = panel_w - 40;
        int overlay_h = 130;

        DrawRectangle(overlay_x, overlay_y, overlay_w, overlay_h, Fade(BLACK, 0.85f));
        DrawRectangleLines(overlay_x, overlay_y, overlay_w, overlay_h, GOLD);

        // Centre the winner text properly using MeasureText
        int tw1 = MeasureText(win_str, 28);
        DrawText(win_str,
                 overlay_x + (overlay_w - tw1) / 2,
                 overlay_y + 20, 28, GOLD);

        int tw2 = MeasureText(score_str, 20);
        DrawText(score_str,
                 overlay_x + (overlay_w - tw2) / 2,
                 overlay_y + 60, 20, WHITE);
    }
}