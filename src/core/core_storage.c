#include <time.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "raylib.h"
#include "types.h"
#include "constants.h"

#include "core_storage.h"

#define HISTORY_HEADER "PICKLEBALL_HISTORY_V1"

static void SaveTextHistory(const Appstate *state)
{
    FILE *f = fopen("matchHistory.txt", "w");
    if (f == NULL) return;

    fprintf(f, "%s\n", HISTORY_HEADER);
    for (int i = 0; i < state->saved_count; i++) {
        const SavedMatch *s = &state->saved[i];
        fprintf(f, "%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%.3f\t%s\t%d\n",
                s->name1, s->name2,
                s->score1, s->score2,
                s->faults1, s->faults2,
                s->aces1, s->aces2,
                s->outs1, s->outs2,
                s->length, s->date, s->winner_num);
    }
    fclose(f);
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
        SaveTextHistory(state);
    }
}

void LoadHistory(Appstate *state) {
    FILE *f = fopen("matchHistory.txt", "rb");
    char line[512];
    state->saved_count = 0;
    if (f == NULL) return;

    if (fgets(line, sizeof(line), f) == NULL) {
        fclose(f);
        return;
    }

    line[strcspn(line, "\r\n")] = '\0';
    if (strcmp(line, HISTORY_HEADER) == 0) {
        while (state->saved_count < MAX_SAVED &&
               fgets(line, sizeof(line), f) != NULL) {
            SavedMatch *s = &state->saved[state->saved_count];
            int fields = sscanf(line,
                "%49[^\t]\t%49[^\t]\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%f\t%29[^\t\n]\t%d",
                s->name1, s->name2,
                &s->score1, &s->score2,
                &s->faults1, &s->faults2,
                &s->aces1, &s->aces2,
                &s->outs1, &s->outs2,
                &s->length, s->date, &s->winner_num);
            if (fields == 13) state->saved_count++;
        }
        fclose(f);
        return;
    }

    /*
     * Read the old raw-struct format once, so existing users do not lose
     * their history. The next save converts it to the portable text format.
     */
    rewind(f);
    int count = 0;
    if (fread(&count, sizeof(count), 1, f) == 1 &&
        count >= 0 && count <= MAX_SAVED) {
        state->saved_count = (int)fread(
            state->saved, sizeof(SavedMatch), (size_t)count, f);
    }
    fclose(f);
    if (state->saved_count > 0) SaveTextHistory(state);
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
