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


