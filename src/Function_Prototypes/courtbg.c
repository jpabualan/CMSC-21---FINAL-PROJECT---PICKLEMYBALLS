#include "raylib.h"
#include <math.h>

#include "constants.h"
#include "types.h"
#include "draw_fx_prototypes.h"

void DrawCourt(Appstate *state, Camera3D cam) {

    BeginMode3D(cam); 
    
    //Court dimensions
    float courtWidth = 20.0f;           //width of the court
    float courtLength = 44.0f;          //courtLengthgth of the court
    float net_h = 3.0f;                 //net height posts
    float kitchen = 7.0f;               //courtLengthgth of the non-volley zone

    //helper calculations
    float half_w = courtWidth / 2.0f;   //from center line to sideline
    float half_l = courtLength / 2.0f;  //from the net to baseline
    float y_up = 0.02f;                 //offsets the y-axis from plane to prevent z-fighting
    
    DrawPlane((Vector3){0, 0, 0}, (Vector2){courtLength, courtWidth}, (Color){30, 100, 80, 255});
    
    DrawPlane((Vector3){-kitchen/2, 0.01f, 0}, (Vector2){kitchen, courtWidth}, (Color){255, 100, 100, 80});
    DrawPlane((Vector3){kitchen/2, 0.01f, 0}, (Vector2){kitchen, courtWidth}, (Color){255, 100, 100, 80});
    
    DrawPlane((Vector3){-(half_l + kitchen)/2, 0.01f, -half_w/2}, (Vector2){half_l - kitchen, half_w}, (Color){100, 150, 200, 80});
    DrawPlane((Vector3){-(half_l + kitchen)/2, 0.01f, half_w/2}, (Vector2){half_l - kitchen, half_w}, (Color){100, 150, 200, 80});
    DrawPlane((Vector3){(half_l + kitchen)/2, 0.01f, -half_w/2}, (Vector2){half_l - kitchen, half_w}, (Color){200, 150, 100, 80});
    DrawPlane((Vector3){(half_l + kitchen)/2, 0.01f, half_w/2}, (Vector2){half_l - kitchen, half_w}, (Color){200, 150, 100, 80});
    
    DrawLine3D((Vector3){-kitchen, y_up, -half_w}, (Vector3){-kitchen, y_up, half_w}, YELLOW);
    DrawLine3D((Vector3){kitchen, y_up, -half_w}, (Vector3){kitchen, y_up, half_w}, YELLOW);
    DrawLine3D((Vector3){-half_l, y_up, -half_w}, (Vector3){-kitchen, y_up, -half_w}, WHITE);
    DrawLine3D((Vector3){-half_l, y_up, half_w}, (Vector3){-kitchen, y_up, half_w}, WHITE);
    DrawLine3D((Vector3){-half_l, y_up, -half_w}, (Vector3){-half_l, y_up, half_w}, WHITE);
    DrawLine3D((Vector3){-half_l, y_up, 0}, (Vector3){-kitchen, y_up, 0}, WHITE);
    DrawLine3D((Vector3){kitchen, y_up, -half_w}, (Vector3){half_l, y_up, -half_w}, WHITE);
    DrawLine3D((Vector3){kitchen, y_up, half_w}, (Vector3){half_l, y_up, half_w}, WHITE);
    DrawLine3D((Vector3){half_l, y_up, -half_w}, (Vector3){half_l, y_up, half_w}, WHITE);
    DrawLine3D((Vector3){kitchen, y_up, 0}, (Vector3){half_l, y_up, 0}, WHITE);
    
    DrawCube((Vector3){0, net_h/2, -half_w}, 0.2f, net_h, 0.2f, BROWN);
    DrawCube((Vector3){0, net_h/2, half_w}, 0.2f, net_h, 0.2f, BROWN);
    for (float z = -half_w + 0.3f; z <= half_w; z += 0.4f) {
        DrawLine3D((Vector3){-0.1f, 0.1f, z}, (Vector3){-0.1f, net_h - 0.1f, z}, LIGHTGRAY);
    }
    
    DrawCylinder((Vector3){state->player1_x, 0.5f, state->player1_z}, 0.5f, 0.5f, 1.0f, 8, BLUE);
    DrawSphere((Vector3){state->player1_x, 1.1f, state->player1_z}, 0.35f, BEIGE);
    DrawCube((Vector3){state->player1_x + 0.8f, 0.7f, state->player1_z}, 0.4f, 0.08f, 0.7f, DARKBLUE);
    
    DrawCylinder((Vector3){state->player2_x, 0.5f, state->player2_z}, 0.5f, 0.5f, 1.0f, 8, RED);
    DrawSphere((Vector3){state->player2_x, 1.1f, state->player2_z}, 0.35f, BEIGE);
    DrawCube((Vector3){state->player2_x - 0.8f, 0.7f, state->player2_z}, 0.4f, 0.08f, 0.7f, MAROON);
    
    DrawSphere((Vector3){state->ball_x, state->ball_y, state->ball_z}, 0.14f, YELLOW);
    
    DrawGrid(30, 2.0f);
    EndMode3D();
}