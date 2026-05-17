#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ctype.h>  // for tolower()

// CONSTANTS
#define MAX_NAME 50
#define WIN_POINTS 11
#define WIN_BY 2
#define MAX_SAVED 1000  
#define MAX_UNDO 100    
#define SCREEN_W 1280
#define SCREEN_H 720

// DATA STRUCTURES
typedef struct {
    char name[MAX_NAME];
    int score;
    int faults;
    int aces;
    int outs;
    int rallies;
} Player;

typedef struct {
    Player p1;
    Player p2;
    int game_over;
    int winner;
    int set_num;
    float timer;
    int timer_on;
    int ball_in;
    int last_action;
    int last_who;
    int last_score1;  // For undo functionality
    int last_score2;
    int last_faults1;
    int last_faults2;
    int last_aces1;
    int last_aces2;
    int last_outs1;
    int last_outs2;
} Match;

// Undo state structure
typedef struct {
    int score1;
    int score2;
    int faults1;
    int faults2;
    int aces1;
    int aces2;
    int outs1;
    int outs2;
    int game_over;
    int winner;
    // Removed timer from undo state
} UndoState;

typedef struct {
    char name1[MAX_NAME];
    char name2[MAX_NAME];
    int score1;
    int score2;
    int faults1;
    int faults2;
    int aces1;
    int aces2;
    int outs1;
    int outs2;
    float length;
    char date[30];
    int winner_num;
} SavedMatch;

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

// Undo stack
UndoState undo_stack[MAX_UNDO];
int undo_stack_ptr = 0;

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

// FUNCTION PROTOTYPES 
void StartNewGame();
void SaveGameToHistory();
void LoadHistory();
void DrawMainMenu();
void DrawGameScreen();
void DrawHistoryScreen();
void DrawCourt(Camera3D cam);
void AddPoint(int who);
void AddFault(int who);
void AddAce(int who);
void AddOut(int who);
void AddIn(int who);
void UndoLastAction();
void ResetGame();
void SearchMatches();
void PushUndoState();
void PopUndoState();

// Push current state to undo stack 
void PushUndoState() {
    if (undo_stack_ptr < MAX_UNDO) {
        undo_stack[undo_stack_ptr].score1 = game.p1.score;
        undo_stack[undo_stack_ptr].score2 = game.p2.score;
        undo_stack[undo_stack_ptr].faults1 = game.p1.faults;
        undo_stack[undo_stack_ptr].faults2 = game.p2.faults;
        undo_stack[undo_stack_ptr].aces1 = game.p1.aces;
        undo_stack[undo_stack_ptr].aces2 = game.p2.aces;
        undo_stack[undo_stack_ptr].outs1 = game.p1.outs;
        undo_stack[undo_stack_ptr].outs2 = game.p2.outs;
        undo_stack[undo_stack_ptr].game_over = game.game_over;
        undo_stack[undo_stack_ptr].winner = game.winner;
        undo_stack_ptr++;
    }
}

// Pop and restore last state 
void PopUndoState() {
    if (undo_stack_ptr > 0) {
        undo_stack_ptr--;
        game.p1.score = undo_stack[undo_stack_ptr].score1;
        game.p2.score = undo_stack[undo_stack_ptr].score2;
        game.p1.faults = undo_stack[undo_stack_ptr].faults1;
        game.p2.faults = undo_stack[undo_stack_ptr].faults2;
        game.p1.aces = undo_stack[undo_stack_ptr].aces1;
        game.p2.aces = undo_stack[undo_stack_ptr].aces2;
        game.p1.outs = undo_stack[undo_stack_ptr].outs1;
        game.p2.outs = undo_stack[undo_stack_ptr].outs2;
        game.game_over = undo_stack[undo_stack_ptr].game_over;
        game.winner = undo_stack[undo_stack_ptr].winner;
        
        
        // Reset last action indicators
        game.last_action = 0;
    }
}

void StartNewGame() {
    // Clear undo stack
    undo_stack_ptr = 0;
    
    memset(&game, 0, sizeof(Match));
    
    strcpy(game.p1.name, p1_name);
    strcpy(game.p2.name, p2_name);
    
    game.p1.score = 0;
    game.p2.score = 0;
    game.p1.faults = 0;
    game.p2.faults = 0;
    game.p1.aces = 0;
    game.p2.aces = 0;
    game.p1.outs = 0;
    game.p2.outs = 0;
    game.game_over = 0;
    game.winner = 0;
    game.timer_on = 1;
    game_timer = 0;
    
    // Push initial state to undo stack
    PushUndoState();
}

void SaveGameToHistory() {
    if (saved_count < MAX_SAVED) {
        SavedMatch *s = &saved[saved_count];
        
        strcpy(s->name1, game.p1.name);
        strcpy(s->name2, game.p2.name);
        s->score1 = game.p1.score;
        s->score2 = game.p2.score;
        s->faults1 = game.p1.faults;
        s->faults2 = game.p2.faults;
        s->aces1 = game.p1.aces;
        s->aces2 = game.p2.aces;
        s->outs1 = game.p1.outs;
        s->outs2 = game.p2.outs;
        s->length = game_timer;
        s->winner_num = game.winner;
        
        time_t t = time(NULL);
        struct tm *tm = localtime(&t);
        strftime(s->date, sizeof(s->date), "%Y-%m-%d %H:%M", tm);
        
        saved_count++;
        
        FILE *f = fopen("save.dat", "wb");
        if (f) {
            fwrite(&saved_count, sizeof(int), 1, f);
            fwrite(saved, sizeof(SavedMatch), saved_count, f);
            fclose(f);
        }
    }
}

void LoadHistory() {
    FILE *f = fopen("save.dat", "rb");
    if (f) {
        fread(&saved_count, sizeof(int), 1, f);
        if (saved_count > MAX_SAVED) saved_count = MAX_SAVED;
        fread(saved, sizeof(SavedMatch), saved_count, f);
        fclose(f);
    }
}

void SearchMatches() {
    search_result_count = 0;
    
    if (strlen(search_name) == 0) {
        // Show all matches if search is empty
        for (int i = 0; i < saved_count; i++) {
            search_results[search_result_count++] = i;
        }
        return;
    }
    
    // Search for matches containing the search term (case insensitive)
    for (int i = 0; i < saved_count; i++) {
        char name1_lower[MAX_NAME], name2_lower[MAX_NAME], search_lower[MAX_NAME];
        
        strcpy(name1_lower, saved[i].name1);
        strcpy(name2_lower, saved[i].name2);
        strcpy(search_lower, search_name);
        
        // Convert to lowercase for case-insensitive search
        for (int j = 0; j < strlen(name1_lower); j++) name1_lower[j] = tolower(name1_lower[j]);
        for (int j = 0; j < strlen(name2_lower); j++) name2_lower[j] = tolower(name2_lower[j]);
        for (int j = 0; j < strlen(search_lower); j++) search_lower[j] = tolower(search_lower[j]);
        
        if (strstr(name1_lower, search_lower) != NULL || strstr(name2_lower, search_lower) != NULL) {
            search_results[search_result_count++] = i;
        }
    }
}

void AddPoint(int who) {
    if (game.game_over) return;
    
    // Push current state to undo stack before making changes
    PushUndoState();
    
    if (who == 1) {
        game.p1.score++;
    } else {
        game.p2.score++;
    }
    
    if (game.p1.score >= WIN_POINTS && (game.p1.score - game.p2.score) >= WIN_BY) {
        game.game_over = 1;
        game.winner = 1;
        game.timer_on = 0;
        SaveGameToHistory();
    } else if (game.p2.score >= WIN_POINTS && (game.p2.score - game.p1.score) >= WIN_BY) {
        game.game_over = 1;
        game.winner = 2;
        game.timer_on = 0;
        SaveGameToHistory();
    }
}

void AddFault(int who) {
    if (game.game_over) return;
    
    // Push current state to undo stack before making changes
    PushUndoState();
    
    if (who == 1) game.p1.faults++;
    else game.p2.faults++;
}

void AddAce(int who) {
    if (game.game_over) return;
    
    // Push current state to undo stack before making changes
    PushUndoState();
    
    if (who == 1) {
        game.p1.aces++;
        game.p1.score++;
    } else {
        game.p2.aces++;
        game.p2.score++;
    }
    
    if (game.p1.score >= WIN_POINTS && (game.p1.score - game.p2.score) >= WIN_BY) {
        game.game_over = 1;
        game.winner = 1;
        game.timer_on = 0;
        SaveGameToHistory();
    } else if (game.p2.score >= WIN_POINTS && (game.p2.score - game.p1.score) >= WIN_BY) {
        game.game_over = 1;
        game.winner = 2;
        game.timer_on = 0;
        SaveGameToHistory();
    }
}

void AddOut(int who) {
    if (game.game_over) return;
    
    // Push current state to undo stack before making changes
    PushUndoState();
    
    if (who == 1) game.p1.outs++;
    else game.p2.outs++;
}

void AddIn(int who) {
    if (game.game_over) return;
    game.ball_in = 1;
}

void UndoLastAction() {
    if (game.game_over) return;
    
    // Pop the last state from undo stack (timer is NOT affected)
    PopUndoState();
}

void ResetGame() {
    StartNewGame();
}
  
void DrawCourt(Camera3D cam) {
    BeginMode3D(cam);
    
    float wide = 20.0f;
    float len = 44.0f;
    float net_h = 3.0f;
    float kitchen = 7.0f;
    float half_w = wide / 2.0f;
    float half_l = len / 2.0f;
    float y_up = 0.02f;
    
    DrawPlane((Vector3){0, 0, 0}, (Vector2){len, wide}, (Color){30, 100, 80, 255});
    
    DrawPlane((Vector3){-kitchen/2, 0.01f, 0}, (Vector2){kitchen, wide}, (Color){255, 100, 100, 80});
    DrawPlane((Vector3){kitchen/2, 0.01f, 0}, (Vector2){kitchen, wide}, (Color){255, 100, 100, 80});
    
    DrawPlane((Vector3){-(half_l + kitchen)/2, 0.01f, -half_w/2}, (Vector2){half_l - kitchen, half_w}, (Color){100, 150, 200, 80});
    DrawPlane((Vector3){-(half_l + kitchen)/2, 0.01f, half_w/2}, (Vector2){half_l - kitchen, half_w}, (Color){100, 150, 200, 80});
    DrawPlane((Vector3){(half_l + kitchen)/2, 0.01f, -half_w/2}, (Vector2){half_l - kitchen, half_w}, (Color){200, 150, 100, 80});
    DrawPlane((Vector3){(half_l + kitchen)/2, 0.01f, half_w/2}, (Vector2){half_l - kitchen, half_w}, (Color){200, 150, 100, 80});
    
    DrawLine3D((Vector3){-kitchen, y_up, -half_w}, (Vector3){-kitchen, y_up, half_w}, YELLOW);
    DrawLine3D((Vector3){kitchen, y_up, -half_w}, (Vector3){kitchen, y_up, half_w}, YELLOW);
    DrawLine3D((Vector3){-half_l, y_up, -half_w}, (Vector3){-kitchen, y_up, -half_w}, WHITE);
    DrawLine3D((Vector3){-half_l, y_up, half_w}, (Vector3){-kitchen, y_up, half_w}, WHITE);
    DrawLine3D((Vector3){-half_l, y_up, -half_w}, (Vector3){-half_l, y_up, half_w}, WHITE);
    DrawLine3D((Vector3){-half_l, y_up, 0}, (Vector3){-kitchen, y_up, 0}, WHITE);
    DrawLine3D((Vector3){kitchen, y_up, -half_w}, (Vector3){half_l, y_up, -half_w}, WHITE);
    DrawLine3D((Vector3){kitchen, y_up, half_w}, (Vector3){half_l, y_up, half_w}, WHITE);
    DrawLine3D((Vector3){half_l, y_up, -half_w}, (Vector3){half_l, y_up, half_w}, WHITE);
    DrawLine3D((Vector3){kitchen, y_up, 0}, (Vector3){half_l, y_up, 0}, WHITE);
    
    DrawCube((Vector3){0, net_h/2, -half_w}, 0.2f, net_h, 0.2f, BROWN);
    DrawCube((Vector3){0, net_h/2, half_w}, 0.2f, net_h, 0.2f, BROWN);
    for (float z = -half_w + 0.3f; z <= half_w; z += 0.4f) {
        DrawLine3D((Vector3){-0.1f, 0.1f, z}, (Vector3){-0.1f, net_h - 0.1f, z}, LIGHTGRAY);
    }
    
    DrawCylinder((Vector3){player1_x, 0.5f, player1_z}, 0.5f, 0.5f, 1.0f, 8, BLUE);
    DrawSphere((Vector3){player1_x, 1.1f, player1_z}, 0.35f, BEIGE);
    DrawCube((Vector3){player1_x + 0.8f, 0.7f, player1_z}, 0.4f, 0.08f, 0.7f, DARKBLUE);
    
    DrawCylinder((Vector3){player2_x, 0.5f, player2_z}, 0.5f, 0.5f, 1.0f, 8, RED);
    DrawSphere((Vector3){player2_x, 1.1f, player2_z}, 0.35f, BEIGE);
    DrawCube((Vector3){player2_x - 0.8f, 0.7f, player2_z}, 0.4f, 0.08f, 0.7f, MAROON);
    
    DrawSphere((Vector3){ball_x, ball_y, ball_z}, 0.14f, YELLOW);
    
    DrawGrid(30, 2.0f);
    EndMode3D();
}


void DrawMainMenu() {
    Vector2 mouse = GetMousePosition();
    int w = SCREEN_W;
    int h = SCREEN_H;
    
    DrawRectangle(0, 0, w, h, (Color){20, 30, 40, 255});
    
    DrawText("PICKLEBALL SCORE TRACKER", w/2 - MeasureText("PICKLEBALL SCORE TRACKER", 40)/2, 50, 40, GOLD);
    DrawText("Pickling Balls", w/2 - MeasureText("Pickling Balls", 30)/2, 100, 30, LIGHTGRAY);
    
    // Name box
    DrawRectangle(w/2 - 250, 150, 500, 150, Fade(BLACK, 0.7f));
    DrawRectangleLines(w/2 - 250, 150, 500, 150, GOLD);
    DrawText("ENTER PLAYER NAMES", w/2 - 120, 160, 20, GOLD);
    
    // Player 1 name input
    DrawText("Player 1:", w/2 - 220, 200, 18, BLUE);
    Rectangle p1_box = {w/2 - 140, 195, 200, 30};
    DrawRectangleRec(p1_box, Fade(WHITE, 0.2f));
    DrawRectangleLinesEx(p1_box, 2, (typing && typing_who == 0) ? YELLOW : LIGHTGRAY);
    
    if (strlen(p1_name) > 0) {
        DrawText(p1_name, w/2 - 130, 202, 18, WHITE);
    } else if (!typing || typing_who != 0) {
        DrawText("Type name here...", w/2 - 130, 202, 18, (Color){150, 150, 150, 255});
    }
    
    DrawText("Player 2:", w/2 - 220, 240, 18, RED);
    Rectangle p2_box = {w/2 - 140, 235, 200, 30};
    DrawRectangleRec(p2_box, Fade(WHITE, 0.2f));
    DrawRectangleLinesEx(p2_box, 2, (typing && typing_who == 1) ? YELLOW : LIGHTGRAY);
    
    if (strlen(p2_name) > 0) {
        DrawText(p2_name, w/2 - 130, 242, 18, WHITE);
    } else if (!typing || typing_who != 1) {
        DrawText("Type name here...", w/2 - 130, 242, 18, (Color){150, 150, 150, 255});
    }
    
    
    if (CheckCollisionPointRec(mouse, p1_box) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        typing = 1;
        typing_who = 0;
        strcpy(typing_buffer, p1_name);
    }
    if (CheckCollisionPointRec(mouse, p2_box) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        typing = 1;
        typing_who = 1;
        strcpy(typing_buffer, p2_name);
    }
    
    // Handle typing
    if (typing) {
        int key = GetCharPressed();
        while (key > 0) {
            if (key >= 32 && key <= 126 && strlen(typing_buffer) < MAX_NAME - 1) {
                int len = strlen(typing_buffer);
                typing_buffer[len] = (char)key;
                typing_buffer[len + 1] = '\0';
            }
            key = GetCharPressed();
        }
        
        if (IsKeyPressed(KEY_BACKSPACE)) {
            int len = strlen(typing_buffer);
            if (len > 0) typing_buffer[len - 1] = '\0';
        }
        
        if (IsKeyPressed(KEY_ENTER)) {
            if (typing_who == 0) {
                strcpy(p1_name, typing_buffer);
            } else {
                strcpy(p2_name, typing_buffer);
            }
            typing = 0;
            name_error = 0;
        }
        
        char display_text[MAX_NAME + 1];
        strcpy(display_text, typing_buffer);
        
        if ((int)(GetTime() * 2) % 2 == 0) {
            int len = strlen(display_text);
            display_text[len] = '_';
            display_text[len + 1] = '\0';
        }
        
        int cursor_y = (typing_who == 0) ? 202 : 242;
        DrawText(display_text, w/2 - 130, cursor_y, 18, YELLOW);
    }
    
    if (strlen(p1_name) == 0 || strlen(p2_name) == 0) {
        DrawText("Please enter both player names!", w/2 - 120, 310, 16, RED);
    }
    
    Rectangle start = {w/2 - 150, 340, 300, 60};
    Color start_color = (strlen(p1_name) > 0 && strlen(p2_name) > 0) ? BLUE : DARKGRAY;
    DrawRectangleRec(start, start_color);
    DrawRectangleLinesEx(start, 2, WHITE);
    DrawText("START MATCH", start.x + 62, start.y + 18, 24, WHITE);
    
    if (CheckCollisionPointRec(mouse, start) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && strlen(p1_name) > 0 && strlen(p2_name) > 0) {
        StartNewGame();
        screen = 1;
        typing = 0;
    }
    
    Rectangle hist = {w/2 - 150, 420, 300, 60};
    DrawRectangleRec(hist, CheckCollisionPointRec(mouse, hist) ? GREEN : DARKGREEN);
    DrawRectangleLinesEx(hist, 2, WHITE);
    DrawText("MATCH HISTORY", hist.x + 50, hist.y + 18, 24, WHITE);
    if (CheckCollisionPointRec(mouse, hist) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        LoadHistory();
        strcpy(search_name, "");
        SearchMatches();
        screen = 2;
    }
    
    Rectangle exit = {w/2 - 150, 500, 300, 60};
    DrawRectangleRec(exit, CheckCollisionPointRec(mouse, exit) ? RED : MAROON);
    DrawRectangleLinesEx(exit, 2, WHITE);
    DrawText("EXIT", exit.x + 115, exit.y + 18, 24, WHITE);
    if (CheckCollisionPointRec(mouse, exit) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        CloseWindow();
    }
    
    DrawText("Click on name box to type | Press ENTER to save", w/2 - 190, 590, 16, LIGHTGRAY);
}

void DrawGameScreen() {
    Vector2 mouse = GetMousePosition();
    int w = SCREEN_W;
    int h = SCREEN_H;
    int court_w = 600;  // Fixed court width
    int ui_w = w - court_w;  // UI panel 
    
    // Update timer
    if (game.timer_on && !game.game_over) {
        game_timer += GetFrameTime();
    }
    
    // Left side - Court
    DrawRectangle(0, 0, court_w, h, (Color){30, 50, 70, 255});
    
    Camera3D cam = {0};
    cam.position = (Vector3){22, 16, 22};
    cam.target = (Vector3){0, 0, 0};
    cam.up = (Vector3){0, 1, 0};
    cam.fovy = 55;
    
    DrawCourt(cam);
    
    // Right side - UI Panel
    DrawRectangle(court_w, 0, ui_w, h, (Color){40, 40, 60, 240});
    
    // Title
    DrawText("MATCH TRACKER", court_w + 20, 20, 28, GOLD);
    
    // Timer
    int mins = (int)game_timer / 60;
    int secs = (int)game_timer % 60;
    DrawText(TextFormat("TIME: %02d:%02d", mins, secs), court_w + 20, 60, 20, LIGHTGRAY);
    
    // PLAYER 1 SECTION (Left side of UI)
    int p1_x = court_w + 20;
    int p2_x = court_w + ui_w - 180;
    
    // Player 1 header
    DrawRectangle(p1_x, 100, 150, 50, Fade(BLUE, 0.3f));
    DrawRectangleLines(p1_x, 100, 150, 50, BLUE);
    DrawText(game.p1.name, p1_x + 10, 110, 20, WHITE);
    DrawText(TextFormat("%d", game.p1.score), p1_x + 100, 110, 32, WHITE);
    
    // Player 2 header
    DrawRectangle(p2_x, 100, 150, 50, Fade(RED, 0.3f));
    DrawRectangleLines(p2_x, 100, 150, 50, RED);
    DrawText(game.p2.name, p2_x + 10, 110, 20, WHITE);
    DrawText(TextFormat("%d", game.p2.score), p2_x + 100, 110, 32, WHITE);
    
    // Stats section
    DrawText("STATS", court_w + 20, 180, 23, GOLD);
    
    // Player 1 stats
    DrawText(TextFormat("Faults: %d", game.p1.faults), p1_x, 210, 20, WHITE);
    DrawText(TextFormat("Aces: %d", game.p1.aces), p1_x, 235, 20, WHITE);
    DrawText(TextFormat("Outs: %d", game.p1.outs), p1_x, 260, 20, WHITE);
    
    // Player 2 stats
    DrawText(TextFormat("Faults: %d", game.p2.faults), p2_x, 210, 20, WHITE);
    DrawText(TextFormat("Aces: %d", game.p2.aces), p2_x, 235, 20, WHITE);
    DrawText(TextFormat("Outs: %d", game.p2.outs), p2_x, 260, 20, WHITE);
    
    // BUTTONS 
    int button_y = 330;
    int button_w = 113;
    int button_h = 40;
    int spacing = 10;
    
    // Player 1 buttons (left column)
    DrawText("CONTROLS", court_w + 20, button_y - 25, 23, GOLD);
    
    // Row 1
    Rectangle p1_score = {p1_x, button_y, button_w, button_h};
    DrawRectangleRec(p1_score, CheckCollisionPointRec(mouse, p1_score) ? DARKBLUE : BLUE);
    DrawText("+1 POINT", p1_score.x + 15, p1_score.y + 12, 20, WHITE);
    if (CheckCollisionPointRec(mouse, p1_score) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) AddPoint(1);
    
    // Row 2
    Rectangle p1_fault = {p1_x, button_y + button_h + spacing, button_w, button_h};
    DrawRectangleRec(p1_fault, CheckCollisionPointRec(mouse, p1_fault) ? DARKGRAY : GRAY);
    DrawText("FAULT", p1_fault.x + 22, p1_fault.y + 12, 20, WHITE);
    if (CheckCollisionPointRec(mouse, p1_fault) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) AddFault(1);
    
    // Row 3
    Rectangle p1_ace = {p1_x, button_y + (button_h + spacing) * 2, button_w, button_h};
    DrawRectangleRec(p1_ace, CheckCollisionPointRec(mouse, p1_ace) ? DARKBROWN : GOLD);
    DrawText("ACE", p1_ace.x + 34, p1_ace.y + 12, 20, BLACK);
    if (CheckCollisionPointRec(mouse, p1_ace) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) AddAce(1);
    
    // Row 4
    Rectangle p1_out = {p1_x, button_y + (button_h + spacing) * 3, button_w, button_h};
    DrawRectangleRec(p1_out, CheckCollisionPointRec(mouse, p1_out) ? DARKPURPLE : PURPLE);
    DrawText("OUT", p1_out.x + 34, p1_out.y + 12, 20, WHITE);
    if (CheckCollisionPointRec(mouse, p1_out) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) AddOut(1);
    
    // Row 5
    Rectangle p1_in = {p1_x, button_y + (button_h + spacing) * 4, button_w, button_h};
    DrawRectangleRec(p1_in, CheckCollisionPointRec(mouse, p1_in) ? DARKGREEN : GREEN);
    DrawText("IN", p1_in.x + 44, p1_in.y + 12, 20, WHITE);
    if (CheckCollisionPointRec(mouse, p1_in) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) AddIn(1);
    
    // Player 2 buttons (right column)
    // Row 1
    Rectangle p2_score = {p2_x, button_y, button_w, button_h};
    DrawRectangleRec(p2_score, CheckCollisionPointRec(mouse, p2_score) ? MAROON : RED);
    DrawText("+1 POINT", p2_score.x + 15, p2_score.y + 12, 20, WHITE);
    if (CheckCollisionPointRec(mouse, p2_score) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) AddPoint(2);
    
    // Row 2
    Rectangle p2_fault = {p2_x, button_y + button_h + spacing, button_w, button_h};
    DrawRectangleRec(p2_fault, CheckCollisionPointRec(mouse, p2_fault) ? DARKGRAY : GRAY);
    DrawText("FAULT", p2_fault.x + 22, p2_fault.y + 12, 20, WHITE);
    if (CheckCollisionPointRec(mouse, p2_fault) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) AddFault(2);
    
    // Row 3
    Rectangle p2_ace = {p2_x, button_y + (button_h + spacing) * 2, button_w, button_h};
    DrawRectangleRec(p2_ace, CheckCollisionPointRec(mouse, p2_ace) ? DARKBROWN : GOLD);
    DrawText("ACE", p2_ace.x + 34, p2_ace.y + 12,20, BLACK);
    if (CheckCollisionPointRec(mouse, p2_ace) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) AddAce(2);
    
    // Row 4
    Rectangle p2_out = {p2_x, button_y + (button_h + spacing) * 3, button_w, button_h};
    DrawRectangleRec(p2_out, CheckCollisionPointRec(mouse, p2_out) ? DARKPURPLE : PURPLE);
    DrawText("OUT", p2_out.x + 34, p2_out.y + 12, 20, WHITE);
    if (CheckCollisionPointRec(mouse, p2_out) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) AddOut(2);
    
    // Row 5
    Rectangle p2_in = {p2_x, button_y + (button_h + spacing) * 4, button_w, button_h};
    DrawRectangleRec(p2_in, CheckCollisionPointRec(mouse, p2_in) ? DARKGREEN : GREEN);
    DrawText("IN", p2_in.x + 44, p2_in.y + 12, 20, WHITE);
    if (CheckCollisionPointRec(mouse, p2_in) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) AddIn(2);
    
    // Action buttons at bottom
    int action_y = button_y + (button_h + spacing) * 5 + 20;
    
    // UNDO button
    Rectangle undo = {court_w + ui_w/2 - 60, action_y, 120, 40};
    DrawRectangleRec(undo, CheckCollisionPointRec(mouse, undo) ? DARKGRAY : DARKGREEN);
    DrawRectangleLinesEx(undo, 2, WHITE);
    DrawText("UNDO", undo.x + 38, undo.y + 10, 18, WHITE);
    if (CheckCollisionPointRec(mouse, undo) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) UndoLastAction();
    
    // RESET button
    Rectangle reset = {p1_x, action_y + 55, 120, 40};
    DrawRectangleRec(reset, CheckCollisionPointRec(mouse, reset) ? ORANGE : YELLOW);
    DrawText("RESET", reset.x + 27, reset.y + 10, 19, BLACK);
    if (CheckCollisionPointRec(mouse, reset) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) ResetGame();
    
    // BACK button
    Rectangle back = {p2_x, action_y + 55, 120, 40};
    DrawRectangleRec(back, CheckCollisionPointRec(mouse, back) ? DARKGRAY : GRAY);
    DrawText("MENU", back.x + 34, back.y + 10, 19, WHITE);
    if (CheckCollisionPointRec(mouse, back) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) screen = 0;
    
    // Winner announcement
    if (game.game_over) {
        DrawRectangle(court_w + 20, h/2 - 80, ui_w - 40, 120, Fade(BLACK, 0.85f));
        DrawRectangleLines(court_w + 20, h/2 - 80, ui_w - 40, 120, GOLD);
        DrawText(TextFormat("%s WINS!", game.winner == 1 ? game.p1.name : game.p2.name), 
                 court_w + ui_w/2 - 100, h/2 - 50, 26, GOLD);
        DrawText(TextFormat("Final Score: %d - %d", game.p1.score, game.p2.score), 
                 court_w + ui_w/2 - 100, h/2 - 15, 20, WHITE);
    }
}

void DrawHistoryScreen() {
    Vector2 mouse = GetMousePosition();
    int w = SCREEN_W;
    int h = SCREEN_H;
    
    DrawRectangle(0, 0, w, h, (Color){20, 30, 40, 255});
    
    DrawText("MATCH HISTORY", w/2 - MeasureText("MATCH HISTORY", 40)/2, 30, 40, GOLD);
    DrawText(TextFormat("Total Matches: %d", saved_count), w/2 - 100, 80, 20, LIGHTGRAY);
    
    // SEARCH BAR 
    DrawText("SEARCH PLAYER:", w/2 - 420, 120, 21, GOLD);
    Rectangle search_box = {w/2 - 230, 115, 400, 30};
    DrawRectangleRec(search_box, Fade(WHITE, 0.2f));
    DrawRectangleLinesEx(search_box, 2, search_typing ? YELLOW : LIGHTGRAY);
    
    if (strlen(search_name) > 0) {
        DrawText(search_name, w/2 - 220, 122, 18, WHITE);
    } else {
        DrawText("", w/2 - 220, 122, 16, (Color){150, 150, 150, 255});
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
        DrawText(display_text, w/2 - 220, 122, 18, YELLOW);
    }
    
    // Clear search button
    Rectangle clear_btn = {w/2 + 180, 115, 80, 30};
    DrawRectangleRec(clear_btn, CheckCollisionPointRec(mouse, clear_btn) ? RED : MAROON);
    DrawText("CLEAR", clear_btn.x + 12, clear_btn.y + 5, 19, WHITE);
    if (CheckCollisionPointRec(mouse, clear_btn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        strcpy(search_name, "");
        search_typing = 0;
        SearchMatches();
    }
    
    // Display matches 
    if (saved_count == 0) {
        DrawText("No matches played yet", w/2 - MeasureText("No matches played yet", 24)/2, 250, 24, LIGHTGRAY);
    } else if (search_result_count == 0 && strlen(search_name) > 0) {
        DrawText("No matches found for that player", w/2 - MeasureText("No matches found for that player", 24)/2, 250, 24, RED);
    } else {
        int y = 180;
        int max_display = (search_result_count > 0) ? search_result_count : saved_count;
        if (max_display > 6) max_display = 6; 
        
        for (int i = 0; i < max_display; i++) {
            int match_index = (search_result_count > 0) ? search_results[i] : i;
            
            Rectangle box = {w/2 - 550, y, 1100, 100};
            DrawRectangleRec(box, CheckCollisionPointRec(mouse, box) ? DARKBLUE : (Color){30, 40, 60, 200});
            DrawRectangleLinesEx(box, 2, WHITE);
            
            // First line: Players and winner
            char line1[200];
            sprintf(line1, "%s  vs  %s     |     WINNER: %s",
                    saved[match_index].name1, saved[match_index].name2,
                    saved[match_index].winner_num == 1 ? saved[match_index].name1 : saved[match_index].name2);
            DrawText(line1, w/2 - 540, y + 10, 28, GOLD);
            
            // Second line: Score and stats
            char line2[300];
            sprintf(line2, "Score: %d-%d     |     Faults: %d-%d     |     Aces: %d-%d     |     Outs: %d-%d",
                    saved[match_index].score1, saved[match_index].score2,
                    saved[match_index].faults1, saved[match_index].faults2,
                    saved[match_index].aces1, saved[match_index].aces2,
                    saved[match_index].outs1, saved[match_index].outs2);
            DrawText(line2, w/2 - 540, y + 45, 25, WHITE);
            
            // Third line: Duration and date
            char line3[200];
            sprintf(line3, "Duration: %.0f seconds     |     Date: %s",
                    saved[match_index].length, saved[match_index].date);
            DrawText(line3, w/2 - 540, y + 75, 20, LIGHTGRAY);
            
            y += 85;
        }
        
        if ((search_result_count > 0 ? search_result_count : saved_count) > 6) {
            DrawText(TextFormat("Showing first 6 of %d matches", (search_result_count > 0 ? search_result_count : saved_count)), 
                     w/2 - 200, y + 10, 18, LIGHTGRAY);
        }
    }
    
    // Back button
    Rectangle back = {w/2 - 100, h - 80, 200, 50};
    DrawRectangleRec(back, CheckCollisionPointRec(mouse, back) ? DARKGRAY : GRAY);
    DrawRectangleLinesEx(back, 2, WHITE);
    DrawText("BACK", back.x + 75, back.y + 15, 20, WHITE);
    if (CheckCollisionPointRec(mouse, back) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        screen = 0;
        search_typing = 0;
        strcpy(search_name, "");
    }
}

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