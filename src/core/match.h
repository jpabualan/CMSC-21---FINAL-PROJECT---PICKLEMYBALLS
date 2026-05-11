#ifndef MATCH_H
#define MATCH_H

#define MAX_NAME_LEN 64
// Tournament standard
#define MAX_SETS 3

typedef struct {
    char name[MAX_NAME_LEN];
    int  score;        // current set score
    int  sets_won;
    int  faults;
    int  aces;
    int  rallies;
    int  is_serving;
} Player;

typedef struct {
    Player p1;
    Player p2;

    int  set_scores[2][MAX_SETS]; // [player][set]
    int  current_set;
    int  last_player; // last player scoring/commit error
    int  last_action; // 1 - point/ 2 - error/ 3 - ace

    float timer_seconds;
    int   timer_running;

    int   ball_in;     //1 = in, 0 = out
    int   game_over;
    int   winner;      //1 = p1, 2 = p2
} MatchState;

// Pickleball rules
#define POINTS_TO_WIN   11
#define WIN_BY          2

void match_init(MatchState *m, const char *name1, const char *name2);
void match_add_score(MatchState *m, int player);   // 1 or 2
void match_add_fault(MatchState *m, int player);
void match_add_ace(MatchState *m, int player);
void match_undo(MatchState *m);
void match_reset(MatchState *m);
int  match_check_winner(MatchState *m);

#endif
