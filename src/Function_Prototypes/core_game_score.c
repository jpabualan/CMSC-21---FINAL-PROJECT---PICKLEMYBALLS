#include "raylib.h"
#include "constants.h"
#include "types.h"
#include "scoring_fx_prototypes.h"
#include "game_fx_prototypes.h"

// ---- helper functions (not included in header files)

//Takes a savestate so UndoLastAction can restore them
static void SaveUndoState(Appstate *state, int action, int who){
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
}


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
static void SwapServer(Appstate *state)
{
    state->game.server = (state->game.server == 1) ? 2 : 1;
}


// ---- Public functions

// Scoring
void addPoint(Appstate *state, int who) {
    if (state->game.game_over) 
        return;
 
    SaveUndoState(state, 1, who);
 
    if (who == 1) state->game.p1.score++;
    else          state->game.p2.score++;
 
    // If the scorer is not the current server, swap serve (side-out).
    if (state->game.server != who) SwapServer(state);
 
    checkWin(state);
}
 
// Fault
void addFault(Appstate *state, int who) {
    if (state->game.game_over) return;
 
    SaveUndoState(state, 2, who);

    if (state->game.server != who) {
        // Receiver error (server gets point and continues serving)
        if (who == 1) state->game.p1.faults++;
        else          state->game.p2.faults++;

        if (state->game.server == 1) state->game.p1.score++;
        else                         state->game.p2.score++;

        checkWin(state);
    } else {
        // Server fault (no point given and service switches)
        if (who == 1) state->game.p1.faults++;
        else          state->game.p2.faults++;

        SwapServer(state);
    }
}

//
void addAce(Appstate *state, int who) {
    if (state->game.game_over) return;
 
    SaveUndoState(state, 3, who);
 
    if (who == 1) { state->game.p1.aces++; state->game.p1.score++; }
    else          { state->game.p2.aces++; state->game.p2.score++; }
 
    // Ace is a point — same server logic as AddPoint.
    if (state->game.server != who) SwapServer(state);
 
    checkWin(state);
}
 
void addOut(Appstate *state, int who) {
    if (state->game.game_over) return;
 
    SaveUndoState(state, 4, who);
 
    if (who == 1) state->game.p1.outs++;
    else          state->game.p2.outs++;
 
    if (state->game.server == who) {
        // Server hit it out — receiver wins rally, side-out
        SwapServer(state);
    } else {
        // Receiver hit it out — server wins the rally, gets a point, keeps serving
        if (state->game.server == 1) state->game.p1.score++;
        else                         state->game.p2.score++;
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
 
    state->game.p1.score  = state->game.last_score1;
    state->game.p2.score  = state->game.last_score2;
    state->game.p1.faults = state->game.last_faults1;
    state->game.p2.faults = state->game.last_faults2;
    state->game.p1.aces   = state->game.last_aces1;
    state->game.p2.aces   = state->game.last_aces2;
    state->game.p1.outs   = state->game.last_outs1;
    state->game.p2.outs   = state->game.last_outs2;
 
    // Restore game state in case this reverses a winning point.
    state->game.game_over = 0;
    state->game.winner    = 0;
    state->game.timer_on  = 1;
 
    // Reverse the serve swap that the action caused.
    SwapServer(state);
 
    state->game.last_action = 0;
}
 
void ResetGame(Appstate *state) {
    StartNewGame(state);
}
