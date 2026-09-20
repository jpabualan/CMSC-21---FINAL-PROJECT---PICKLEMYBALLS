#include <time.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "raylib.h"
#include "cJSON.h"
#include "types.h"
#include "constants.h"

#include "core_storage.h"

#define HISTORY_HEADER "PICKLEBALL_HISTORY_V1"
#define HISTORY_FILE "matchHistory.json"

static void SaveJsonHistory(const Appstate *state)
{
    cJSON *root = cJSON_CreateObject();
    cJSON *matches = cJSON_AddArrayToObject(root, "matches");
    if (root == NULL || matches == NULL) {
        cJSON_Delete(root);
        return;
    }

    for (int i = 0; i < state->saved_count; i++) {
        const SavedMatch *s = &state->saved[i];
        cJSON *match = cJSON_CreateObject();
        if (match == NULL) continue;

        cJSON_AddItemToArray(matches, match);
        cJSON_AddStringToObject(match, "player1", s->name1);
        cJSON_AddStringToObject(match, "player2", s->name2);
        cJSON_AddNumberToObject(match, "score1", s->score1);
        cJSON_AddNumberToObject(match, "score2", s->score2);
        cJSON_AddNumberToObject(match, "faults1", s->faults1);
        cJSON_AddNumberToObject(match, "faults2", s->faults2);
        cJSON_AddNumberToObject(match, "aces1", s->aces1);
        cJSON_AddNumberToObject(match, "aces2", s->aces2);
        cJSON_AddNumberToObject(match, "outs1", s->outs1);
        cJSON_AddNumberToObject(match, "outs2", s->outs2);
        cJSON_AddNumberToObject(match, "duration", s->length);
        cJSON_AddStringToObject(match, "date", s->date);
        cJSON_AddNumberToObject(match, "winner", s->winner_num);
    }

    char *json_text = cJSON_Print(root);
    FILE *f = fopen(HISTORY_FILE, "w");
    if (json_text != NULL && f != NULL) {
        fputs(json_text, f);
        fclose(f);
    } else if (f != NULL) {
        fclose(f);
    }

    free(json_text);
    cJSON_Delete(root);
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
        SaveJsonHistory(state);
    }
}

static bool ReadJsonString(const cJSON *object, const char *key, char *out, size_t out_size)
{
    const cJSON *value = cJSON_GetObjectItemCaseSensitive(object, key);
    if (!cJSON_IsString(value) || value->valuestring == NULL) return false;

    snprintf(out, out_size, "%s", value->valuestring);
    return true;
}

static bool ReadJsonNumber(const cJSON *object, const char *key, double *out)
{
    const cJSON *value = cJSON_GetObjectItemCaseSensitive(object, key);
    if (!cJSON_IsNumber(value)) return false;

    *out = value->valuedouble;
    return true;
}

static bool ReadJsonMatch(const cJSON *object, SavedMatch *match)
{
    double score1, score2, faults1, faults2, aces1, aces2;
    double outs1, outs2, length, winner;

    if (!cJSON_IsObject(object) ||
        !ReadJsonString(object, "player1", match->name1, sizeof(match->name1)) ||
        !ReadJsonString(object, "player2", match->name2, sizeof(match->name2)) ||
        !ReadJsonString(object, "date", match->date, sizeof(match->date)) ||
        !ReadJsonNumber(object, "score1", &score1) ||
        !ReadJsonNumber(object, "score2", &score2) ||
        !ReadJsonNumber(object, "faults1", &faults1) ||
        !ReadJsonNumber(object, "faults2", &faults2) ||
        !ReadJsonNumber(object, "aces1", &aces1) ||
        !ReadJsonNumber(object, "aces2", &aces2) ||
        !ReadJsonNumber(object, "outs1", &outs1) ||
        !ReadJsonNumber(object, "outs2", &outs2) ||
        !ReadJsonNumber(object, "duration", &length) ||
        !ReadJsonNumber(object, "winner", &winner)) return false;

    match->score1 = (int)score1;
    match->score2 = (int)score2;
    match->faults1 = (int)faults1;
    match->faults2 = (int)faults2;
    match->aces1 = (int)aces1;
    match->aces2 = (int)aces2;
    match->outs1 = (int)outs1;
    match->outs2 = (int)outs2;
    match->length = (float)length;
    match->winner_num = (int)winner;
    return true;
}

static bool LoadJsonHistory(Appstate *state)
{
    char *json_text = LoadFileText(HISTORY_FILE);
    if (json_text == NULL) return false;

    cJSON *root = cJSON_Parse(json_text);
    UnloadFileText(json_text);
    if (root == NULL) return false;

    const cJSON *matches = cJSON_GetObjectItemCaseSensitive(root, "matches");
    if (!cJSON_IsArray(matches)) {
        cJSON_Delete(root);
        return false;
    }

    state->saved_count = 0;
    int count = cJSON_GetArraySize(matches);
    for (int i = 0; i < count && state->saved_count < MAX_SAVED; i++) {
        SavedMatch match = {0};
        if (ReadJsonMatch(cJSON_GetArrayItem(matches, i), &match)) {
            state->saved[state->saved_count++] = match;
        }
    }

    cJSON_Delete(root);
    return true;
}

static void LoadLegacyTextHistory(Appstate *state) {
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
}

void LoadHistory(Appstate *state) {
    state->saved_count = 0;
    if (LoadJsonHistory(state)) return;

    LoadLegacyTextHistory(state);
    if (state->saved_count > 0) SaveJsonHistory(state);
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
