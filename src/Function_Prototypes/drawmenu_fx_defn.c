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

bool drawMenuButton(Rectangle bounds, const char *text, Color baseColor, Vector2 mouse, bool enabled){
    bool hovered = CheckCollisionPointRec(mouse, bounds);
    
    //ternary op for visual state
    Color drawColor = !enabled ? GRAY : (hovered ? ColorBrightness(baseColor, 0.2f) : baseColor);

    DrawRectangleRec(bounds, drawColor);
    //extended
    DrawRectangleLinesEx(bounds, 2, RAYWHITE);

    //text centering
    int fontSize = 24;
    int textWidth = MeasureText(text, fontSize);
    int textX = bounds.x + (bounds.width / 2) - (textWidth / 2);
    int textY = bounds.y + (bounds.height / 2) - (fontSize / 2);

    DrawText(text, textX, textY, fontSize, RAYWHITE);

    return (enabled && hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
}

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

    //Player input boxes
    Rectangle boxes[2] = { {w/2 - 140, 195, 200, 30}, {w/2 - 140, 235, 200, 30} };
    const char* labels[2] = { "Player 1:", "Player 2:" };
    Color labelColors[2] = { BLUE, RED };
    char* playerNames[2] = { state->p1_name, state->p2_name };

    for (int i = 0; i < 2; i++) {
        DrawText(labels[i], w/2 - 220, boxes[i].y + 5, 18, labelColors[i]);
        DrawRectangleRec(boxes[i], Fade(WHITE, 0.2f));
        
        // Highlight box if currently typing
        bool isTypingThis = (state->typing && state->typing_who == i);
        DrawRectangleLinesEx(boxes[i], 2, isTypingThis ? YELLOW : LIGHTGRAY);

        if (isTypingThis) {
            // Show typing buffer with cursor
            const char* blink = ((int)(GetTime() * 2) % 2 == 0) ? "_" : "";
            DrawText(TextFormat("%s%s", state->typing_buffer, blink), boxes[i].x + 10, boxes[i].y + 7, 18, YELLOW);
        } else {
            // Show saved name or placeholder
            bool hasName = strlen(playerNames[i]) > 0;
            DrawText(hasName ? playerNames[i] : "Type name...", boxes[i].x + 10, boxes[i].y + 7, 18, hasName ? WHITE : GRAY);
        }

        if (CheckCollisionPointRec(mouse, boxes[i]) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            state->typing = 1;
            state->typing_who = i;
            strcpy(state->typing_buffer, playerNames[i]);
        }
    }

    //Input handling
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
            if (state->typing_who == 0) strcpy(state->p1_name, state->typing_buffer);
            else strcpy(state->p2_name, state->typing_buffer);
            state->typing = 0;
        }
    }

    // --- Buttons Section ---
    bool canStart = (strlen(state->p1_name) > 0 && strlen(state->p2_name) > 0);
    if (!canStart) DrawText("Please enter both player names!", w/2 - 132, 310, 16, RED);

    // Main Action Buttons
    if (drawMenuButton((Rectangle){w/2 - 150, 340, 300, 60}, "START MATCH", BLUE, mouse, canStart)) {
        StartNewGame(state);
        state->screen = SCREEN_GAME;
        state->typing = 0;
    }

    if (drawMenuButton((Rectangle){w/2 - 150, 420, 300, 60}, "MATCH HISTORY", DARKGREEN, mouse, true)) {
        LoadHistory(state);
        state->search_name[0] = '\0';
        SearchMatches(state);
        state->screen = SCREEN_HISTORY;
    }

    if (drawMenuButton((Rectangle){w/2 - 150, 500, 300, 60}, "EXIT", MAROON, mouse, true)) {
        // Safe exit logic
        state->should_close = true; 
    }

    DrawText("Click on name box to type | Press ENTER to save", 
    w/2 - MeasureText("Click on name box to type | Press ENTER to save", 18)/2, 590, 18, WHITE);

    DrawText("A CMSC21 Project", w/2 - 55, 700, 12, LIGHTGRAY);
}
