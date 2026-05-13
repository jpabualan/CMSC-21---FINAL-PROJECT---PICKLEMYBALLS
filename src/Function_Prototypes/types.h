#ifndef TYPES_H
#define TYPES_H

#include "constants.h"


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

// Master state struct
typedef struct {
    Match game;
    SavedMatch saved[MAX_SAVED];
    int saved_count;
    int screen;

    // player names
    char p1_name[MAX_NAME];
    char p2_name[MAX_NAME];

    int typing;
    int typing_who;
    char typing_buffer[MAX_NAME];
    float game_timer;
    int name_error;

    char search_name[MAX_NAME];
    int searching;
    int search_typing;
    char search_buffer[MAX_NAME];
    int search_results[MAX_SAVED];
    int search_result_count;

    float ball_x, ball_y, ball_z;
    float player1_x, player1_z;
    float player2_x, player2_z;
} Appstate;

#endif