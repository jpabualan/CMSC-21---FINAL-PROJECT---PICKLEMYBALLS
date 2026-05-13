#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

#include "constants.h"
#include "types.h"

// GLOBAL VARIABLES
extern Match game = {0};
extern SavedMatch saved[MAX_SAVED];
extern int saved_count = 0;
extern int screen = 0; // 0=main menu, 1=game, 2=history
extern char p1_name[MAX_NAME] = "";  // EMPTY, user will type
extern char p2_name[MAX_NAME] = "";  // EMPTY, user will type
extern int typing = 0;
extern int typing_who = 0; // 0=player1, 1=player2
extern char typing_buffer[MAX_NAME] = "";
extern float game_timer = 0;
extern int name_error = 0;  // show error if name is empty

// Search variables
extern char search_name[MAX_NAME] = "";
extern int searching = 0;
extern int search_typing = 0;
extern char search_buffer[MAX_NAME] = "";
extern int search_results[MAX_SAVED];
extern int search_result_count = 0;

// visuals
extern float ball_x = 0, ball_y = 0.25f, ball_z = 0;
extern float player1_x = -22.0f, player1_z = -4.0f;
extern float player2_x = 22.0f, player2_z = 4.0f;

#endif