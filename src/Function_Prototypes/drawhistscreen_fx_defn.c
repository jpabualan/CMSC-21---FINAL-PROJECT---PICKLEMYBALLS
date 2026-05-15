#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ctype.h>  // for tolower()

#include "constants.h"
#include "types.h"
#include "draw_fx_prototypes.h"
#include "game_fx_prototypes.h"

void DrawHistoryScreen(Appstate *state) {
    Vector2 mouse = GetMousePosition();
    int w = SCREEN_W;
    int h = SCREEN_H;
    
    DrawRectangle(0, 0, w, h, (Color){20, 30, 40, 255});
    
    // Header
    DrawText("MATCH HISTORY", w/2 - MeasureText("MATCH HISTORY", 40)/2, 30, 40, GOLD);
    DrawText(TextFormat("Total Matches: %d", state->saved_count), w/2 - 100, 80, 20, LIGHTGRAY);
    
    // --- Search Section ---
    DrawText("SEARCH PLAYER:", w/2 - 200, 120, 18, GOLD);
    Rectangle search_box = {w/2 - 100, 115, 300, 30};
    DrawRectangleRec(search_box, Fade(WHITE, 0.2f));
    DrawRectangleLinesEx(search_box, 2, state->search_typing ? YELLOW : LIGHTGRAY);
    
    // Search Typing Logic
    if (state->search_typing) {
        int key = GetCharPressed();
        while (key > 0) {
            if (key >= 32 && key <= 126 && strlen(state->search_buffer) < MAX_NAME - 1) {
                int len = strlen(state->search_buffer);
                state->search_buffer[len] = (char)key;
                state->search_buffer[len + 1] = '\0';
                SearchMatches(state); // Search as you type
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE)) {
            int len = strlen(state->search_buffer);
            if (len > 0) {
                state->search_buffer[len - 1] = '\0';
                SearchMatches(state); 
            }
        }
        if (IsKeyPressed(KEY_ENTER)) state->search_typing = 0;
    }

    // Display search text or placeholder
    const char* searchText = state->search_typing ? state->search_buffer : state->search_name;
    if (strlen(searchText) > 0) {
        DrawText(searchText, search_box.x + 10, search_box.y + 7, 18, WHITE);
    } else {
        DrawText("Type to search...", search_box.x + 10, search_box.y + 7, 16, GRAY);
    }

    // Use the DrawMenuButton helper for Clear and Back
    if (drawMenuButton((Rectangle){w/2 + 210, 115, 80, 30}, "CLEAR", RED, mouse, true)) {
        state->search_name[0] = '\0';
        state->search_buffer[0] = '\0';
        state->search_typing = 0;
        SearchMatches(state);
    }

    // --- Matches List ---
    int y = 170;
    int total = (strlen(state->search_name) > 0) ? state->search_result_count : state->saved_count;
    int max_display = (total > 5) ? 5 : total;

    if (total == 0) {
        DrawText("No matches found", w/2 - 80, 250, 20, RED);
    } else {
        for (int i = 0; i < max_display; i++) {
            int idx = (state->search_result_count > 0) ? state->search_results[i] : i;
            Rectangle card = {w/2 - 480, y, 960, 85};
            
            DrawRectangleRec(card, (Color){30, 40, 60, 200});
            DrawRectangleLinesEx(card, 1, Fade(WHITE, 0.3f));

            // Line 1: The Matchup and Date
            DrawText(TextFormat("%s vs %s", state->saved[idx].name1, state->saved[idx].name2), card.x + 20, card.y + 15, 22, GOLD);
            DrawText(state->saved[idx].date, card.x + card.width - 150, card.y + 15, 16, LIGHTGRAY);

            // Line 2: The Stats
            char stats[300];
            sprintf(stats, "Score: %d-%d | Faults: %d-%d | Aces: %d-%d | Duration: %.0fs",
                    state->saved[idx].score1, state->saved[idx].score2,
                    state->saved[idx].faults1, state->saved[idx].faults2,
                    state->saved[idx].aces1, state->saved[idx].aces2,
                    state->saved[idx].length);
            DrawText(stats, card.x + 20, card.y + 50, 17, WHITE);

            y += 95;
        }
    }

    // Footer/Back Button
    if (total > 5) {
        DrawText(TextFormat("Showing first 5 of %d matches", total), w/2 - 100, y + 10, 16, LIGHTGRAY);
    }

    if (drawMenuButton((Rectangle){w/2 - 100, h - 70, 200, 50}, "BACK", GRAY, mouse, true)) {
        state->screen = 0;
        state->search_name[0] = '\0';
    }
}