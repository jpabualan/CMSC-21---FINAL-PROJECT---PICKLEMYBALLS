#include "raylib.h"
#include "constants.h"
#include "types.h"
#include "scoring_fx_prototypes.h"

//Scoring
void AddPoint(Appstate *state, int who) {
    if (state->game.game_over) 
        return;
    
    // Save current state for undo
    state->game.last_score1 = state->game.p1.score;
    state->game.last_score2 = state->game.p2.score;
    state->game.last_faults1 = state->game.p1.faults;
    state->game.last_faults2 = state->game.p2.faults;
    state->game.last_aces1 = state->game.p1.aces;
    state->game.last_aces2 = state->game.p2.aces;
    state->game.last_outs1 = state->game.p1.outs;
    state->game.last_outs2 = state->game.p2.outs;
    state->game.last_action = 1;
    state->game.last_who = who;
    
    if (who == 1) {
        state->game.p1.score++;
    } 
    else {
        state->game.p2.score++;
    }
    
    if (state->game.p1.score >= WIN_POINTS && (state->game.p1.score - state->game.p2.score) >= WIN_BY) {
        state->game.game_over = 1;
        state->game.winner = 1;
        state->game.timer_on = 0;
        SaveGameToHistory(state);
    } else if (state->game.p2.score >= WIN_POINTS && (state->game.p2.score - state->game.p1.score) >= WIN_BY) {
        state->game.game_over = 1;
        state->game.winner = 2;
        state->game.timer_on = 0;
        SaveGameToHistory(state);
    }
}

void AddFault(Appstate *state, int who) {
    if (state->game.game_over) 
        return;
    
    // Save current state for undo
    state->game.last_score1 = state->game.p1.score;
    state->game.last_score2 = state->game.p2.score;
    state->game.last_faults1 = state->game.p1.faults;
    state->game.last_faults2 = state->game.p2.faults;
    state->game.last_aces1 = state->game.p1.aces;
    state->game.last_aces2 = state->game.p2.aces;
    state->game.last_outs1 = state->game.p1.outs;
    state->game.last_outs2 = state->game.p2.outs;
    state->game.last_action = 2;
    state->game.last_who = who;
    
    if (who == 1)   state->game.p1.faults++;
    else            state->game.p2.faults++;
}

void AddAce(Appstate *state, int who) {
    if (state->game.game_over) 
        return;
    
    // Save current state for undo
    state->game.last_score1 = state->game.p1.score;
    state->game.last_score2 = state->game.p2.score;
    state->game.last_faults1 = state->game.p1.faults;
    state->game.last_faults2 = state->game.p2.faults;
    state->game.last_aces1 = state->game.p1.aces;
    state->game.last_aces2 = state->game.p2.aces;
    state->game.last_outs1 = state->game.p1.outs;
    state->game.last_outs2 = state->game.p2.outs;
    state->game.last_action = 3;
    state->game.last_who = who;
    
    if (who == 1) {
        state->game.p1.aces++;
        state->game.p1.score++;
    } else {
        state->game.p2.aces++;
        state->game.p2.score++;
    }
    
    if (state->game.p1.score >= WIN_POINTS && (state->game.p1.score - state->game.p2.score) >= WIN_BY) {
        state->game.game_over = 1;
        state->game.winner = 1;
        state->game.timer_on = 0;
        SaveGameToHistory(state);
    } else if (state->game.p2.score >= WIN_POINTS && (state->game.p2.score - state->game.p1.score) >= WIN_BY) {
        state->game.game_over = 1;
        state->game.winner = 2;
        state->game.timer_on = 0;
        SaveGameToHistory(state);
    }
}

void AddOut(Appstate *state,int who) {
    if (state->game.game_over) return;
    
    // Save current state for undo
    state->game.last_score1 = state->game.p1.score;
    state->game.last_score2 = state->game.p2.score;
    state->game.last_faults1 = state->game.p1.faults;
    state->game.last_faults2 = state->game.p2.faults;
    state->game.last_aces1 = state->game.p1.aces;
    state->game.last_aces2 = state->game.p2.aces;
    state->game.last_outs1 = state->game.p1.outs;
    state->game.last_outs2 = state->game.p2.outs;
    state->game.last_action = 4;
    state->game.last_who = who;
    
    if (who == 1)   state->game.p1.outs++;
    else            state->game.p2.outs++;
}

void AddIn(Appstate *state,int who) {
    if (state->game.game_over) 
        return;

    state->game.ball_in = 1;
}

void UndoLastAction(Appstate *state) {
    if (state->game.game_over) 
        return;
    
    state->game.p1.score = state->game.last_score1;
    state->game.p2.score = state->game.last_score2;
    state->game.p1.faults = state->game.last_faults1;
    state->game.p2.faults = state->game.last_faults2;
    state->game.p1.aces = state->game.last_aces1;
    state->game.p2.aces = state->game.last_aces2;
    state->game.p1.outs = state->game.last_outs1;
    state->game.p2.outs = state->game.last_outs2;
    
    state->game.last_action = 0;
}

void ResetGame(Appstate *state) {
    StartNewGame(state);
}
