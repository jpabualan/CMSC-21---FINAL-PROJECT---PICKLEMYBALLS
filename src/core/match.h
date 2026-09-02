#ifndef MATCH_H
#define MATCH_H

#include "types.h"

void MatchInit(Match *match, const char *player1_name, const char *player2_name);
void MatchSwapServer(Match *match);
void MatchSaveUndoState(Match *match, int action, int who);
void MatchRestoreUndoState(Match *match);
int MatchHasWinner(const Match *match);

#endif
