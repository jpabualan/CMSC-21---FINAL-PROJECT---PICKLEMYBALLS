#include "raylib.h"
#include "match.h" //gets matchstate

void updateMatchTimer(MatchState *m){
    if (m->timer_running && !m->game_over)
    {
        m->timer_seconds += GetFrameTime();
    }
}

const char* getFormattedTime(float seconds)
{   
    //typecasted to int since Min & Sec is only needed not milisec
    int min = (int)seconds / 60;
    int sec = (int)seconds % 60;

    return TextFormat("%02d:%02d", min, sec);
}