#include <time.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "raylib.h"
#include "types.h"
#include "constants.h"

#include "core_storage.h"

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
