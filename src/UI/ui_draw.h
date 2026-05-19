#ifndef UIDRAW_H
#define UIDRAW_H

void DrawMainMenu(Appstate *state);
void DrawGameScreen(Appstate *state);
void DrawHistoryScreen(Appstate *state);
void DrawCourt(Appstate *state, Camera3D cam);
bool DrawMenuButton(Rectangle bounds, const char *text, Color baseColor, Vector2 mouse, bool enabled);

#endif