#ifndef PLAYER_H
#define PLAYER_H

#include "types.h"

void PlayerInit(Player *player, const char *name);
void PlayerResetStats(Player *player);
void PlayerAddScore(Player *player);
void PlayerAddFault(Player *player);
void PlayerAddAce(Player *player);
void PlayerAddOut(Player *player);

#endif
