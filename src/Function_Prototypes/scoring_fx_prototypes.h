#ifndef SCORING_FPROTOTYPES_H
#define SCORING_FPROTOTYPES_H

void AddPoint(int who);
void AddFault(int who);
void AddAce(int who);
void AddOut(int who);
void AddIn(int who);
void UndoLastAction();
void ResetGame();

#endif