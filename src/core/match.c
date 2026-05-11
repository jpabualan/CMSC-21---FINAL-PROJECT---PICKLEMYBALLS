#include <stdio.h>
#include <string.h>
#include "match.h"

void match_init(MatchState *m, const char *name1, const char *name2){
    memset(m, 0, sizeof(MatchState)); // Sets all of the bytes in a block of memory to the same value(0)

    strncpy(m->p1.name, name1, MAX_NAME_LEN); //init player 1
    strncpy(m->p2.name, name2, MAX_NAME_LEN); //init player 2

    m->p1.score = 0;
    m->p2.score = 0;

    m->p1.faults = 0;
    m->p2.faults = 0;

    m->p1.aces = 0;
    m->p2.aces = 0;

    m->game_over = 0;
    m->winner = 0;
    m->current_set = 0;

    m->timer_running = 0;
    m->ball_in = 1;
}

void match_add_score(MatchState *m, int player){
    if (player == 1){
        m->p1.score++;
        m->p1.rallies++;
   } else {
        m->p2.score++;
        m->p2.rallies++;
    }   
        m->last_player = player;
        m->last_action = 1;
}

void match_add_fault(MatchState *m, int player){

    if (player == 1) {
        m->p1.faults++;
    } 
    else {
         m->p2.faults++;
    }
        m->last_player = player;
        m->last_action = 2;
}

void match_add_ace(MatchState *m, int player){

    if (player == 1) {
        m->p1.aces++;
        m->p1.score++;
     } 
    else {
        m->p2.aces++;
        m->p2.score++;
    }
        m->last_player = player;
        m->last_action = 3;
    }

void match_undo(MatchState *m){
     if (m->last_action == 0) {
            printf("Nothing to undo\n");
            return;
    }

     if (m->last_action == 1) { // score
        if (m->last_player == 1){
             m->p1.score--;
        } else{
             m->p2.score--;
     }

     else if (m->last_action == 2) { // fault
         if (m->last_player == 1){
              m->p1.faults--;
        }else
         m->p2.faults--;
    }

    else if (m->last_action == 3) { // ace
         if (m->last_player == 1) {
               m->p1.aces--;
              m->p1.score--;
         } else{
               m->p2.aces--;
               m->p2.score--;
         }
    }
        m->last_action = 0; // after undo reset last_action
}

void match_reset(MatchState *m) {

    memset(m, 0, sizeof(MatchState));

    m->p1.score = 0;
    m->p2.score = 0;

    m->p1.faults = 0;
    m->p2.faults = 0;

    m->p1.aces = 0;
    m->p2.aces = 0;

    m->p1.rallies = 0;
    m->p2.rallies = 0;

    m->current_set = 0;
    m->timer_seconds = 0;
    m->timer_running = 0;
    m->ball_in = 1;

    m->game_over = 0;
    m->winner = 0;

    m->last_action = 0;
    m->last_player = 0;  
}

int match_check_winner(MatchState *m) {

     // Player 1 wins
     if (m->p1.score >= POINTS_TO_WIN &&
         (m->p1.score - m->p2.score) >= WIN_BY) {

         m->game_over = 1;
         m->winner = 1;
         return 1;
    }

     // Player 2 wins
     if (m->p2.score >= POINTS_TO_WIN &&
        (m->p2.score - m->p1.score) >= WIN_BY) {

        m->game_over = 1;
        m->winner = 2;
        return 1;
    }
    return 0;
}







