#ifndef SCORING_FPROTOTYPES_H
#define SCORING_FPROTOTYPES_H

void addPoint(Appstate *state,int who);
void addFault(Appstate *state, int who);
void addAce(Appstate *state, int who);
void addOut(Appstate *state, int who);
void addIn(Appstate *state, int who);
void UndoLastAction(Appstate *state);
void ResetGame(Appstate *state);

#endif