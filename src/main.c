
#include "raylib.h"
#include "screens/screenMenu.h"
#include "screens/screenTracking.h"
#include "screens/screenHist.h"


typedef enum{
    SCREEN_MENU,
    SCREEN_TRACKING,
    SCREEN_HISTORY,
    SCREEN_EXIT
} Screen;

int main(){

    //initialize window
    InitWindow(1280, 720, "PicklingBalls");
    //set fps
    SetTargetFPS(60);

    Screen current = SCREEN_MENU;

    (!WindowShouldClose() && current != SCREEN_EXIT){
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch(current){
            case SCREEN_MENU:
                current = UpdateDrawMenu(); // di pa na define
                break;
            case SCREEN_TRACKING:
                current = UpdateDrawTracking(); // di pa na define
                break;
            case SCREEN_HISTORY:
                current = UpdateDrawHistory(); // di pa na define
                break;
            default: break;
        }

        EndDrawing();

    }

    CloseWindow();
    return 0;

}

