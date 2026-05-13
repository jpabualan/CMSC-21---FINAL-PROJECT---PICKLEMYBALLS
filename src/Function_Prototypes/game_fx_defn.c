#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ctype.h>  // for tolower()

#include "constants.h"
#include "types.h"
#include "Function_Prototypes/game_fx_prototypes.h"
#include "global_variables.h"

// ======================= GAME FUNCTIONS =======================
void StartNewGame() {
    // Save current state for undo before resetting
    game.last_score1 = game.p1.score;
    game.last_score2 = game.p2.score;
    game.last_faults1 = game.p1.faults;
    game.last_faults2 = game.p2.faults;
    game.last_aces1 = game.p1.aces;
    game.last_aces2 = game.p2.aces;
    game.last_outs1 = game.p1.outs;
    game.last_outs2 = game.p2.outs;
    
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
