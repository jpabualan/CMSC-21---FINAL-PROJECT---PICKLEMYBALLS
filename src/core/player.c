#include <string.h>

#include "player.h"

void PlayerInit(Player *player, const char *name)
{
    memset(player, 0, sizeof(*player));
    strncpy(player->name, name, MAX_NAME - 1);
    player->name[MAX_NAME - 1] = '\0';
}

void PlayerResetStats(Player *player)
{
    player->score = 0;
    player->faults = 0;
    player->aces = 0;
    player->outs = 0;
}

void PlayerAddScore(Player *player)
{
    player->score++;
}

void PlayerAddFault(Player *player)
{
    player->faults++;
}

void PlayerAddAce(Player *player)
{
    player->aces++;
    player->score++;
}

void PlayerAddOut(Player *player)
{
    player->outs++;
}
