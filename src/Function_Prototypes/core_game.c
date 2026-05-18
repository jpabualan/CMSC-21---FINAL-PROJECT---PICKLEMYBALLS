#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "constants.h"
#include "types.h"
#include "game_fx_prototypes.h"

//Game functions
void StartNewGame(Appstate *state) {
    memset(&state->game, 0, sizeof(Match)); //set all blocks to 0
    
    strcpy(state->game.p1.name, state->p1_name);
    strcpy(state->game.p2.name, state->p2_name);
    
    //initializaed at the start of the game
    state->game.server = 1; // player 1 serves first
    state->game.game_over = 0;
    state->game.winner = 0;
    state->game.timer_on = 1;
    state->game_timer = 0;
}

void SaveGameToHistory(Appstate *state) {
    if (state->saved_count < MAX_SAVED) {
        SavedMatch *s = &state->saved[state->saved_count];
        
        strcpy(s->name1, state->game.p1.name);
        strcpy(s->name2, state->game.p2.name);
        s->score1 = state->game.p1.score;
        s->score2 = state->game.p2.score;
        s->faults1 = state->game.p1.faults;
        s->faults2 = state->game.p2.faults;
        s->aces1 = state->game.p1.aces;
        s->aces2 = state->game.p2.aces;
        s->outs1 = state->game.p1.outs;
        s->outs2 = state->game.p2.outs;
        s->length = state->game_timer;
        s->winner_num = state->game.winner;
        
        time_t t = time(NULL);
        struct tm *tm = localtime(&t);
        //gets current time to be permanently stored
        strftime(s->date, sizeof(s->date), "%Y-%m-%d %H:%M", tm);
        
        state->saved_count++;
        
        FILE *f = fopen("matchHistory.txt", "wb");
        if (f) {
            fwrite(&state->saved_count, sizeof(int), 1, f);
            fwrite(state->saved, sizeof(SavedMatch), state->saved_count, f);
            fclose(f);
        }
    }
}

void LoadHistory(Appstate *state) {
    FILE *f = fopen("matchHistory.txt", "rb");
    if (f) {
        //Read the count, check if successful
        if (fread(&state->saved_count, sizeof(int), 1, f) != 1) {
            state->saved_count = 0;
        } else {
            //Clamp the values to be safe
            if (state->saved_count > MAX_SAVED) state->saved_count = MAX_SAVED;
            if (state->saved_count < 0) state->saved_count = 0;
            
            //Read the matches and update the count to how many were ACTUALLY read
            int items_read = fread(state->saved, sizeof(SavedMatch), state->saved_count, f);
            state->saved_count = items_read; 
        }
        fclose(f);
    } else {
        // If file doesn't exist, ensure count is 0
        state->saved_count = 0; 
    }
}

void SearchMatches(Appstate *state) {
    state->search_result_count = 0;
    
    if (strlen(state->search_name) == 0) {
        // Show all matches if search is empty
        for (int i = 0; i < state->saved_count; i++) {
            state->search_results[state->search_result_count++] = i;
        }
        return;
    }
    
    // Search for matches containing the search term (case insensitive)
    for (int i = 0; i < state->saved_count; i++) {
        char name1_lower[MAX_NAME], name2_lower[MAX_NAME], search_lower[MAX_NAME];
        
        strcpy(name1_lower, state->saved[i].name1);
        strcpy(name2_lower, state->saved[i].name2);
        strcpy(search_lower, state->search_name);
        
        // Convert to lowercase for case-insensitive search
        for (int j = 0; j < strlen(name1_lower); j++) name1_lower[j] = tolower(name1_lower[j]);
        for (int j = 0; j < strlen(name2_lower); j++) name2_lower[j] = tolower(name2_lower[j]);
        for (int j = 0; j < strlen(search_lower); j++) search_lower[j] = tolower(search_lower[j]);
        
        if (strstr(name1_lower, search_lower) != NULL || strstr(name2_lower, search_lower) != NULL) {
            state->search_results[state->search_result_count++] = i;
        }
    }
}
