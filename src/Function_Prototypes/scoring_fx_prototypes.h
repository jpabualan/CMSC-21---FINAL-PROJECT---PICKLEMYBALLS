#ifndef SCORING_FPROTOTYPES_H
#define SCORING_FPROTOTYPES_H

void AddPoint(Appstate *state,int who);
void AddFault(Appstate *state, int who);
void AddAce(Appstate *state, int who);
void AddOut(Appstate *state, int who);
void AddIn(Appstate *state, int who);
void UndoLastAction(Appstate *state);
void ResetGame(Appstate *state);

#endif