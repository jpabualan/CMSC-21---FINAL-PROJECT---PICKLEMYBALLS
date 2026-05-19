#include "raylib.h"
#include "raylib.h"
#include <stdio.h>

#include "constants.h"
#include "types.h"
#include "core_game_score.h"
#include "ui_draw.h"

// Helper: draw a centred string inside a rectangle
static void DrawTextCentred(const char *text, Rectangle rect, int fontSize, Color color) {
    int tw = MeasureText(text, fontSize);
    DrawText(text,
             (int)(rect.x + (rect.width  - tw) / 2.0f),
             (int)(rect.y + (rect.height - fontSize) / 2.0f),
             fontSize, color);
}
 
// Helper: draw a button with border + hover brightening, returns true if clicked
static bool DrawButton(Rectangle rect, const char *text, int fontSize,
                       Color base, Color hoverBase, Color textColor, Vector2 mouse) {
    bool hovered = CheckCollisionPointRec(mouse, rect);
    Color bg = hovered ? hoverBase : base;
    DrawRectangleRec(rect, bg);
    // Inner highlight line at top for depth
    DrawRectangle((int)rect.x + 1, (int)rect.y + 1, (int)rect.width - 2, 2,
                  Fade(WHITE, hovered ? 0.35f : 0.2f));
    // Border — brighter on hover
    DrawRectangleLinesEx(rect, 2, hovered ? WHITE : Fade(WHITE, 0.55f));
    DrawTextCentred(text, rect, fontSize, textColor);
    return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}
 
void DrawGameScreen(Appstate *state) {
 
    // ---- Layout constants --------------------------------------------------
    // Use actual window size so this works if the window is resizable.
    int w        = GetScreenWidth();
    int h        = GetScreenHeight();
    int court_w  = w / 2;               // left panel width
    int panel_w  = w - court_w;         // right panel width
    int panel_x  = court_w;             // right panel left edge
 
    // Two player column origins inside the right panel (20 px margin each)
    int p1_col   = panel_x + 20;
    int p2_col   = panel_x + panel_w / 2 + 10;
 
    // ---- Background panels -------------------------------------------------
    DrawRectangle(0,        0, court_w, h, (Color){30,  50,  70,  255});
    DrawRectangle(panel_x,  0, panel_w, h, (Color){10,  10,  15,  255}); // fully opaque — no court bleed
    // Subtle vertical separator line between court and panel
    DrawRectangle(panel_x, 0, 3, h, (Color){80, 80, 120, 255});
 
    // ---- 3D Court (scissor + draw handled inside DrawCourt) ----------------
    Camera3D cam = {0};
    cam.position = (Vector3){25.0f, 18.0f, 25.0f};
    cam.target   = (Vector3){ 0.0f,  0.0f,  0.0f};
    cam.up       = (Vector3){ 0.0f,  1.0f,  0.0f};
    cam.fovy     = 55.0f;
    cam.projection = CAMERA_PERSPECTIVE;
 
    DrawCourt(state, cam);
 
    Vector2 mouse = GetMousePosition();
 
    // ---- Header
    // Header bar
    DrawRectangle(panel_x, 0, panel_w, 85, (Color){15, 15, 25, 255});
    DrawRectangle(panel_x, 83, panel_w, 2, (Color){80, 80, 120, 255});
    DrawText("MATCH TRACKER", panel_x + 20, 20, 26, GOLD);
 
    // Timer — NOTE: timer is updated in UpdateGameTimer(), not here
    int mins = (int)state->game_timer / 60;
    int secs = (int)state->game_timer % 60;
    DrawText(TextFormat("TIME: %02d:%02d", mins, secs),
             panel_x + 20, 55, 18, LIGHTGRAY);
 
    // ---- Score cards
    // Card backgrounds for each player score section
    Rectangle p1_card = {p1_col - 10, 90, panel_w / 2 - 20, 125};
    Rectangle p2_card = {p2_col - 10, 90, panel_w / 2 - 20, 125};
    DrawRectangleRec(p1_card, (Color){20, 30, 60, 255});
    DrawRectangleLinesEx(p1_card, 2, (Color){60, 100, 200, 200});
    DrawRectangleRec(p2_card, (Color){60, 20, 20, 255});
    DrawRectangleLinesEx(p2_card, 2, (Color){200, 60, 60, 200});
 
    // Player names and scores
    DrawText(state->game.p1.name,                    p1_col, 100, 22, (Color){120, 160, 255, 255});
    DrawText(TextFormat("%d", state->game.p1.score), p1_col, 125, 52, WHITE);
    DrawText(state->game.p2.name,                    p2_col, 100, 22, (Color){255, 120, 120, 255});
    DrawText(TextFormat("%d", state->game.p2.score), p2_col, 125, 52, WHITE);
 
    // Serve indicator pill
    if (!state->game.game_over) {
        const char *serveLabel = "SERVING";
        if (state->game.server == 1) {
            Rectangle pill = {p1_col - 2, 180, MeasureText(serveLabel, 13) + 16, 20};
            DrawRectangleRec(pill, GOLD);
            DrawRectangleLinesEx(pill, 1, WHITE);
            DrawText(serveLabel, pill.x + 8, pill.y + 3, 13, BLACK);
        } else if (state->game.server == 2) {
            Rectangle pill = {p2_col - 2, 180, MeasureText(serveLabel, 13) + 16, 20};
            DrawRectangleRec(pill, GOLD);
            DrawRectangleLinesEx(pill, 1, WHITE);
            DrawText(serveLabel, pill.x + 8, pill.y + 3, 13, BLACK);
        }
    }
 
    // ---- Stats section
    DrawRectangle(panel_x, 225, panel_w, 1, (Color){60, 60, 90, 255}); // divider
    DrawText("STATS", p1_col, 233, 16, (Color){180, 150, 60, 255});
 
    // Player 1
    DrawText(TextFormat("Faults : %d", state->game.p1.faults), p1_col, 255, 15, RAYWHITE);
    DrawText(TextFormat("Aces   : %d", state->game.p1.aces),   p1_col, 273, 15, RAYWHITE);
    DrawText(TextFormat("Outs   : %d", state->game.p1.outs),   p1_col, 291, 15, RAYWHITE);
 
    // Player 2
    DrawText(TextFormat("Faults : %d", state->game.p2.faults), p2_col, 255, 15, RAYWHITE);
    DrawText(TextFormat("Aces   : %d", state->game.p2.aces),   p2_col, 273, 15, RAYWHITE);
    DrawText(TextFormat("Outs   : %d", state->game.p2.outs),   p2_col, 291, 15, RAYWHITE);
 
    // ---- Control buttons
    DrawRectangle(panel_x, 315, panel_w, 1, (Color){60, 60, 90, 255});
 
    // Section labels
    DrawText("P1 CONTROLS", p1_col, 322, 14, (Color){120, 160, 255, 255});
    DrawText("P2 CONTROLS", p2_col, 322, 14, (Color){255, 120, 120, 255});
 
    // ---- Player 1 buttons
    //  Row 1
    Rectangle p1_score = {p1_col,       342, 85, 38};
    Rectangle p1_fault = {p1_col + 95,  342, 85, 38};
    Rectangle p1_ace   = {p1_col + 190, 342, 85, 38};
 
    // P1 score button — greyed out if P1 is receiving
    bool p1_can_score = (state->game.server == 1);
    Color p1_base  = p1_can_score ? (Color){30,80,180,255}  : (Color){50,50,50,255};
    Color p1_hover = p1_can_score ? (Color){60,120,255,255} : (Color){50,50,50,255};
    Color p1_text  = p1_can_score ? WHITE : GRAY;
    if (DrawButton(p1_score, "+1 PT", 16, p1_base, p1_hover, p1_text, mouse) && p1_can_score) addPoint(state, 1);
    if (DrawButton(p1_fault, state->game.server == 1 ? "FAULT" : "ERROR", 14, (Color){80,80,80,255}, (Color){130,130,130,255}, WHITE, mouse)) addFault(state, 1);
    if (DrawButton(p1_ace,   "ACE",    16, p1_base, p1_hover, p1_text, mouse) && p1_can_score) addAce(state, 1);
 
    //  Row 2
    Rectangle p1_out = {p1_col,      390, 85, 38};
    Rectangle p1_in  = {p1_col + 95, 390, 85, 38};
 
    if (DrawButton(p1_out, "OUT", 16, (Color){100,20,120,255},  (Color){150,40,180,255}, WHITE, mouse)) addOut(state, 1);
    if (DrawButton(p1_in,  "IN",  16, (Color){20,100,40,255},   (Color){30,160,60,255},  WHITE, mouse)) addIn(state, 1);
 
    // ---- Player 2 buttons
    //  Row 1
    Rectangle p2_score = {p2_col,       342, 85, 38};
    Rectangle p2_fault = {p2_col + 95,  342, 85, 38};
    Rectangle p2_ace   = {p2_col + 190, 342, 85, 38};
 
    // P2 score button — greyed out if P2 is receiving
    bool p2_can_score = (state->game.server == 2);
    Color p2_base  = p2_can_score ? (Color){160,30,30,255}  : (Color){50,50,50,255};
    Color p2_hover = p2_can_score ? (Color){220,60,60,255}  : (Color){50,50,50,255};
    Color p2_text  = p2_can_score ? WHITE : GRAY;
    if (DrawButton(p2_score, "+1 PT", 16, p2_base, p2_hover, p2_text, mouse) && p2_can_score) addPoint(state, 2);
    if (DrawButton(p2_fault, state->game.server == 2 ? "FAULT" : "ERROR", 14, (Color){80,80,80,255}, (Color){130,130,130,255}, WHITE, mouse)) addFault(state, 2);
    if (DrawButton(p2_ace,   "ACE",    16, p2_base, p2_hover, p2_text, mouse) && p2_can_score) addAce(state, 2);
 
    //  Row 2
    Rectangle p2_out = {p2_col,      390, 85, 38};
    Rectangle p2_in  = {p2_col + 95, 390, 85, 38};
 
    if (DrawButton(p2_out, "OUT", 16, (Color){100,20,120,255},  (Color){150,40,180,255}, WHITE, mouse)) addOut(state, 2);
    if (DrawButton(p2_in,  "IN",  16, (Color){20,100,40,255},   (Color){30,160,60,255},  WHITE, mouse)) addIn(state, 2);
 
    // ---- Shared utility buttons
    DrawRectangle(panel_x, 442, panel_w, 1, (Color){60, 60, 90, 255}); // divider
 
    // UNDO — centred in the panel
    int undo_x = panel_x + (panel_w - 140) / 2;
    Rectangle undo = {undo_x, 452, 140, 40};
    if (DrawButton(undo, "UNDO LAST", 16, (Color){140,80,0,255}, (Color){210,120,0,255}, WHITE, mouse))
        UndoLastAction(state);
 
    // RESET — bottom-left of panel
    Rectangle reset = {panel_x + 20, h - 58, 130, 40};
    if (DrawButton(reset, "RESET", 16, (Color){100,70,0,255}, (Color){180,120,0,255}, WHITE, mouse))
        ResetGame(state);
 
    // BACK — bottom-right of panel
    Rectangle back = {panel_x + panel_w - 150, h - 58, 130, 40};
    if (DrawButton(back, "BACK", 16, (Color){50,50,70,255}, (Color){90,90,120,255}, WHITE, mouse))
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
