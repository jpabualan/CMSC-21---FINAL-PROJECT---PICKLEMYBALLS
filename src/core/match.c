#include <stdio.h>
#include <string.h>
#include "match.h"

void match_init(MatchState *m, const char *name1, const char *name2){
    memset(m, 0, sizeof(MatchState)); // Sets all of the bytes in a block of memory to the same value(0)

    strncpy(m->p1.name, name1, MAX_NAME_LEN); //init player 1
    m->p1.is_serving = 1; //player 1 is serving

    strncpy(m->p1.name, name2, MAX_NAME_LEN); //init player 2

    m->current_set = 0;
    m->timer_running = 0;
    m->ball_in = 1;

}


