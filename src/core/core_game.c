#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "constants.h"
#include "types.h"
#include "core_game.h"

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

void SearchMatches(Appstate *state) {
    state->ui.search_result_count = 0;
    
    if (strlen(state->ui.search_name) == 0) {
        // Show all matches if search is empty
        for (int i = 0; i < state->saved_count; i++) {
            state->ui.search_results[state->ui.search_result_count++] = i;
        }
        return;
    }
    
    // Search for matches containing the search term (case insensitive)
    for (int i = 0; i < state->saved_count; i++) {
        char name1_lower[MAX_NAME], name2_lower[MAX_NAME], search_lower[MAX_NAME];
        
        strcpy(name1_lower, state->saved[i].name1);
        strcpy(name2_lower, state->saved[i].name2);
        strcpy(search_lower, state->ui.search_name);
        
        // Convert to lowercase for case-insensitive search
        for (int j = 0; j < strlen(name1_lower); j++) name1_lower[j] = tolower(name1_lower[j]);
        for (int j = 0; j < strlen(name2_lower); j++) name2_lower[j] = tolower(name2_lower[j]);
        for (int j = 0; j < strlen(search_lower); j++) search_lower[j] = tolower(search_lower[j]);
        
        if (strstr(name1_lower, search_lower) != NULL || strstr(name2_lower, search_lower) != NULL) {
            state->ui.search_results[state->ui.search_result_count++] = i;
        }
    }
}
