#include <stdio.h>
#include <time.h>
#include "match.h"

//adds the final results to the txt file for permanent storage
void SaveMatchToHistory(MatchState *m)
{

    // Only save if the game actually finished
    if (!m->game_over) 
        return;

    // Open the text file
    FILE *file = fopen("src/data/matchHistory.txt", "a"); //append mode
    if (file == NULL) 
        return;

    // Get the current date
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char dateBuffer[11];
    strftime(dateBuffer, 11, "%Y-%m-%d", tm_info);

    // Write: Date, Player1, Player2, Score1, Score2, Winner
    fprintf(file, "%s | %s vs %s | Final: %d-%d | Winner: Player %d | Time: %.0fs\n",
            dateBuffer,
            m->p1.name, m->p2.name,
            m->p1.score, m->p2.score,
            m->winner, m->timer_seconds);

    fclose(file); //close file to prevent errors upon reopen of file
}