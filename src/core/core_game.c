#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "constants.h"
#include "types.h"
#include "core_game.h"
#include "match.h"

//Game functions
void StartNewGame(Appstate *state) {
    MatchInit(&state->game, state->p1_name, state->p2_name);
    state->game_timer = 0;
}

