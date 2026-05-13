#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ctype.h>  // for tolower()

#include "constants.h"
#include "types.h"
#include "Function_Prototypes/scoring_fx_prototypes.h"
#include "global_variables.h"

// ======================= SCORING =======================
void AddPoint(int who) {
    if (game.game_over) return;
    
    // Save current state for undo
    game.last_score1 = game.p1.score;
    game.last_score2 = game.p2.score;
    game.last_faults1 = game.p1.faults;
    game.last_faults2 = game.p2.faults;
    game.last_aces1 = game.p1.aces;
    game.last_aces2 = game.p2.aces;
    game.last_outs1 = game.p1.outs;
    game.last_outs2 = game.p2.outs;
    game.last_action = 1;
    game.last_who = who;
    
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
    
    // Save current state for undo
    game.last_score1 = game.p1.score;
    game.last_score2 = game.p2.score;
    game.last_faults1 = game.p1.faults;
    game.last_faults2 = game.p2.faults;
    game.last_aces1 = game.p1.aces;
    game.last_aces2 = game.p2.aces;
    game.last_outs1 = game.p1.outs;
    game.last_outs2 = game.p2.outs;
    game.last_action = 2;
    game.last_who = who;
    
    if (who == 1) game.p1.faults++;
    else game.p2.faults++;
}

void AddAce(int who) {
    if (game.game_over) return;
    
    // Save current state for undo
    game.last_score1 = game.p1.score;
    game.last_score2 = game.p2.score;
    game.last_faults1 = game.p1.faults;
    game.last_faults2 = game.p2.faults;
    game.last_aces1 = game.p1.aces;
    game.last_aces2 = game.p2.aces;
    game.last_outs1 = game.p1.outs;
    game.last_outs2 = game.p2.outs;
    game.last_action = 3;
    game.last_who = who;
    
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
    
    // Save current state for undo
    game.last_score1 = game.p1.score;
    game.last_score2 = game.p2.score;
    game.last_faults1 = game.p1.faults;
    game.last_faults2 = game.p2.faults;
    game.last_aces1 = game.p1.aces;
    game.last_aces2 = game.p2.aces;
    game.last_outs1 = game.p1.outs;
    game.last_outs2 = game.p2.outs;
    game.last_action = 4;
    game.last_who = who;
    
    if (who == 1) game.p1.outs++;
    else game.p2.outs++;
}

void AddIn(int who) {
    if (game.game_over) return;
    game.ball_in = 1;
}

void UndoLastAction() {
    if (game.game_over) return;
    
    game.p1.score = game.last_score1;
    game.p2.score = game.last_score2;
    game.p1.faults = game.last_faults1;
    game.p2.faults = game.last_faults2;
    game.p1.aces = game.last_aces1;
    game.p2.aces = game.last_aces2;
    game.p1.outs = game.last_outs1;
    game.p2.outs = game.last_outs2;
    
    game.last_action = 0;
}

void ResetGame() {
    StartNewGame();
}
