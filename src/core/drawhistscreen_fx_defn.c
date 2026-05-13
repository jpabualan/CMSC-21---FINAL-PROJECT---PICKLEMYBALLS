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
#include "global_variables.h"


// ======================= HISTORY SCREEN =======================
void DrawHistoryScreen() {
    Vector2 mouse = GetMousePosition();
    int w = SCREEN_W;
    int h = SCREEN_H;
    
    DrawRectangle(0, 0, w, h, (Color){20, 30, 40, 255});
    
    DrawText("MATCH HISTORY", w/2 - MeasureText("MATCH HISTORY", 40)/2, 30, 40, GOLD);
    DrawText(TextFormat("Total Matches: %d", saved_count), w/2 - 100, 80, 20, LIGHTGRAY);
    
    // SEARCH BAR
    DrawText("SEARCH PLAYER:", w/2 - 200, 120, 18, GOLD);
    Rectangle search_box = {w/2 - 100, 115, 400, 30};
    DrawRectangleRec(search_box, Fade(WHITE, 0.2f));
    DrawRectangleLinesEx(search_box, 2, search_typing ? YELLOW : LIGHTGRAY);
    
    if (strlen(search_name) > 0) {
        DrawText(search_name, w/2 - 90, 122, 18, WHITE);
    } else {
        DrawText("Type player name to search...", w/2 - 90, 122, 16, (Color){150, 150, 150, 255});
    }
    
    // Search box click handling
    if (CheckCollisionPointRec(mouse, search_box) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        search_typing = 1;
        strcpy(search_buffer, search_name);
    }
    
    // Search typing
    if (search_typing) {
        int key = GetCharPressed();
        while (key > 0) {
            if (key >= 32 && key <= 126 && strlen(search_buffer) < MAX_NAME - 1) {
                int len = strlen(search_buffer);
                search_buffer[len] = (char)key;
                search_buffer[len + 1] = '\0';
            }
            key = GetCharPressed();
        }
        
        if (IsKeyPressed(KEY_BACKSPACE)) {
            int len = strlen(search_buffer);
            if (len > 0) search_buffer[len - 1] = '\0';
        }
        
        if (IsKeyPressed(KEY_ENTER)) {
            strcpy(search_name, search_buffer);
            search_typing = 0;
            SearchMatches();
        }
        
        char display_text[MAX_NAME + 1];
        strcpy(display_text, search_buffer);
        
        if ((int)(GetTime() * 2) % 2 == 0) {
            int len = strlen(display_text);
            display_text[len] = '_';
            display_text[len + 1] = '\0';
        }
        DrawText(display_text, w/2 - 90, 122, 18, YELLOW);
    }
    
    // Clear search button
    Rectangle clear_btn = {w/2 + 310, 115, 80, 30};
    DrawRectangleRec(clear_btn, CheckCollisionPointRec(mouse, clear_btn) ? RED : MAROON);
    DrawText("CLEAR", clear_btn.x + 18, clear_btn.y + 8, 16, WHITE);
    if (CheckCollisionPointRec(mouse, clear_btn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        strcpy(search_name, "");
        search_typing = 0;
        SearchMatches();
    }
    
    // Display matches
    if (saved_count == 0) {
        DrawText("No matches played yet", w/2 - MeasureText("No matches played yet", 20)/2, 250, 20, LIGHTGRAY);
    } else if (search_result_count == 0 && strlen(search_name) > 0) {
        DrawText("No matches found for that player", w/2 - MeasureText("No matches found for that player", 20)/2, 250, 20, RED);
    } else {
        int y = 170;
        int max_display = (search_result_count > 0) ? search_result_count : saved_count;
        if (max_display > 8) max_display = 8;
        
        for (int i = 0; i < max_display; i++) {
            int match_index = (search_result_count > 0) ? search_results[i] : i;
            
            Rectangle box = {w/2 - 480, y, 960, 60};
            DrawRectangleRec(box, CheckCollisionPointRec(mouse, box) ? DARKBLUE : (Color){30, 40, 60, 200});
            DrawRectangleLinesEx(box, 1, WHITE);
            
            char text[350];
            sprintf(text, "%s vs %s | Winner: %s | Score: %d-%d | Faults: %d-%d | Aces: %d-%d | Outs: %d-%d | Duration: %.0fs | %s",
                    saved[match_index].name1, saved[match_index].name2,
                    saved[match_index].winner_num == 1 ? saved[match_index].name1 : saved[match_index].name2,
                    saved[match_index].score1, saved[match_index].score2,
                    saved[match_index].faults1, saved[match_index].faults2,
                    saved[match_index].aces1, saved[match_index].aces2,
                    saved[match_index].outs1, saved[match_index].outs2,
                    saved[match_index].length, saved[match_index].date);
            DrawText(text, w/2 - 470, y + 18, 13, WHITE);
            y += 70;
        }
        
        if ((search_result_count > 0 ? search_result_count : saved_count) > 8) {
            DrawText(TextFormat("Showing first 8 of %d matches", (search_result_count > 0 ? search_result_count : saved_count)), 
                     w/2 - 150, y + 10, 14, LIGHTGRAY);
        }
    }
    
    // Back button
    Rectangle back = {w/2 - 100, h - 70, 200, 50};
    DrawRectangleRec(back, CheckCollisionPointRec(mouse, back) ? DARKGRAY : GRAY);
    DrawRectangleLinesEx(back, 2, WHITE);
    DrawText("BACK", back.x + 75, back.y + 15, 20, WHITE);
    if (CheckCollisionPointRec(mouse, back) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        screen = 0;
        search_typing = 0;
        strcpy(search_name, "");
    }
}