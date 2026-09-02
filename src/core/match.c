#include <string.h>

#include "constants.h"
#include "match.h"
#include "player.h"

void MatchInit(Match *match, const char *player1_name, const char *player2_name)
{
    memset(match, 0, sizeof(*match));
    PlayerInit(&match->p1, player1_name);
    PlayerInit(&match->p2, player2_name);
    match->server = 1;
    match->timer_on = 1;
}

void MatchSwapServer(Match *match)
{
    match->server = (match->server == 1) ? 2 : 1;
}

void MatchSaveUndoState(Match *match, int action, int who)
{
    match->last_score1 = match->p1.score;
    match->last_score2 = match->p2.score;
    match->last_faults1 = match->p1.faults;
    match->last_faults2 = match->p2.faults;
    match->last_aces1 = match->p1.aces;
    match->last_aces2 = match->p2.aces;
    match->last_outs1 = match->p1.outs;
    match->last_outs2 = match->p2.outs;
    match->last_action = action;
    match->last_who = who;
}

void MatchRestoreUndoState(Match *match)
{
    match->p1.score = match->last_score1;
    match->p2.score = match->last_score2;
    match->p1.faults = match->last_faults1;
    match->p2.faults = match->last_faults2;
    match->p1.aces = match->last_aces1;
    match->p2.aces = match->last_aces2;
    match->p1.outs = match->last_outs1;
    match->p2.outs = match->last_outs2;
    match->game_over = 0;
    match->winner = 0;
    match->timer_on = 1;
    match->last_action = 0;
}

int MatchHasWinner(const Match *match)
{
    return (match->p1.score >= WIN_POINTS &&
            match->p1.score - match->p2.score >= WIN_BY) ||
           (match->p2.score >= WIN_POINTS &&
            match->p2.score - match->p1.score >= WIN_BY);
}
