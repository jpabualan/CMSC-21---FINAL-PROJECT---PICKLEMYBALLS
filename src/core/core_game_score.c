#include "raylib.h"
#include "constants.h"
#include "types.h"
#include "core_game_score.h"
#include "core_game.h"
#include "core_storage.h"
#include "match.h"
#include "player.h"


//Checks if either player won
static void checkWin(Appstate *state){
    
    //update state of players
    int s1 = state->game.p1.score;
    int s2 = state->game.p2.score;

    if (s1 >= WIN_POINTS && (s1 - s2) >= WIN_BY){
                state->game.game_over = 1;
        state->game.winner    = 1;
        state->game.timer_on  = 0;
        SaveGameToHistory(state);
    } else if (s2 >= WIN_POINTS && (s2 - s1) >= WIN_BY) {
        state->game.game_over = 1;
        state->game.winner    = 2;
        state->game.timer_on  = 0;
        SaveGameToHistory(state);
    }
}

//swap server
// ---- Public functions

// Scoring
void addPoint(Appstate *state, int who) {
    if (state->game.game_over) 
        return;
 
    MatchSaveUndoState(&state->game, 1, who);
 
    if (who == 1) PlayerAddScore(&state->game.p1);
    else          PlayerAddScore(&state->game.p2);
 
    // If the scorer is not the current server, swap serve (side-out).
    if (state->game.server != who) MatchSwapServer(&state->game);
 
    checkWin(state);
}
 
// Fault
void addFault(Appstate *state, int who) {
    if (state->game.game_over) return;
 
    MatchSaveUndoState(&state->game, 2, who);

    if (state->game.server != who) {
        // Receiver error (server gets point and continues serving)
        if (who == 1) PlayerAddFault(&state->game.p1);
        else          PlayerAddFault(&state->game.p2);

        if (state->game.server == 1) PlayerAddScore(&state->game.p1);
        else                         PlayerAddScore(&state->game.p2);

        checkWin(state);
    } else {
        // Server fault (no point given and service switches)
        if (who == 1) PlayerAddFault(&state->game.p1);
        else          PlayerAddFault(&state->game.p2);

        MatchSwapServer(&state->game);
    }
}

//
void addAce(Appstate *state, int who) {
    if (state->game.game_over) return;
 
    MatchSaveUndoState(&state->game, 3, who);
 
    if (who == 1) PlayerAddAce(&state->game.p1);
    else          PlayerAddAce(&state->game.p2);
 
    // Ace is a point — same server logic as AddPoint.
    if (state->game.server != who) MatchSwapServer(&state->game);
 
    checkWin(state);
}
 
void addOut(Appstate *state, int who) {
    if (state->game.game_over) return;
 
    MatchSaveUndoState(&state->game, 4, who);
 
    if (who == 1) PlayerAddOut(&state->game.p1);
    else          PlayerAddOut(&state->game.p2);
 
    if (state->game.server == who) {
        // Server hit it out — receiver wins rally, side-out
        MatchSwapServer(&state->game);
    } else {
        // Receiver hit it out — server wins the rally, gets a point, keeps serving
        if (state->game.server == 1) PlayerAddScore(&state->game.p1);
        else                         PlayerAddScore(&state->game.p2);
        checkWin(state);
    }
}
 
void addIn(Appstate *state, int who) {
    // "In" confirms ball landed in — no stat or serve change.
    // No-op kept so the UI button can be wired up later.
    (void)state;
    (void)who;
}
 
void UndoLastAction(Appstate *state) {
    // Allow undo even on game_over so operator can correct the last point.
    if (state->game.last_action == 0) 
        return;
 
    MatchRestoreUndoState(&state->game);

    // Every supported action can change the serving side once.
    MatchSwapServer(&state->game);
}
 
void ResetGame(Appstate *state) {
    StartNewGame(state);
}
