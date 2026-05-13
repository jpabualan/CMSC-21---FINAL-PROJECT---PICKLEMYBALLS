#ifndef TYPES_H
#define TYPES_H

#include "constants.h"

// DATA STRUCTURES
typedef struct {
    char name[MAX_NAME];
    int score;
    int faults;
    int aces;
    int outs;
    int rallies;
} Player;

typedef struct {
    Player p1;
    Player p2;
    int game_over;
    int winner;
    int set_num;
    float timer;
    int timer_on;
    int ball_in;
    int last_action;
    int last_who;
    int last_score1;  // For undo functionality
    int last_score2;
    int last_faults1;
    int last_faults2;
    int last_aces1;
    int last_aces2;
    int last_outs1;
    int last_outs2;
} Match;

typedef struct {
    char name1[MAX_NAME];
    char name2[MAX_NAME];
    int score1;
    int score2;
    int faults1;
    int faults2;
    int aces1;
    int aces2;
    int outs1;
    int outs2;
    float length;
    char date[30];
    int winner_num;
} SavedMatch;

#endif