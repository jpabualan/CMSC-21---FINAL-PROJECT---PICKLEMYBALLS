#ifndef TIMER_H
#define TIMER_H

#include "match.h"

//updates time based on time_running being true by frame time
void updateMatchTimer(MatchState *m);

//returns a string of Minutes:Seconds for displaye only
const char* getFormattedTime(float seconds);

#endif