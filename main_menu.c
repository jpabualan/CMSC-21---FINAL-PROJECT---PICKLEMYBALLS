#include "raylib.h"
#include <string.h>
#include <stdio.h>

// ─── Screen definitions ───────────────────────────────────────────
typedef enum {
    SCREEN_MENU = 0,
    SCREEN_START_GAME,
    SCREEN_EDIT_STATS,
    SCREEN_PLAYER_STATS
} GameScreen;

// ─── Button state ─────────────────────────────────────────────────
typedef struct {
    Rectangle rect;
    const char *label;
    bool hovered;
    float hoverT;   // 0..1 animation progress
} Button;

// ─── Colours ──────────────────────────────────────────────────────
#define COL_BG        (Color){ 10,  12,  20,  255 }
#define COL_PANEL     (Color){ 18,  22,  36,  255 }
#define COL_ACCENT    (Color){ 92, 198, 255,  255 }
#define COL_ACCENT2   (Color){180,  80, 255,  255 }
#define COL_BTN       (Color){ 24,  30,  50,  255 }
#define COL_BTN_HOV   (Color){ 34,  44,  72,  255 }
#define COL_TEXT      (Color){220, 230, 255,  255 }
#define COL_MUTED     (Color){ 90, 100, 130,  255 }
#define COL_WHITE     WHITE
#define COL_BORDER    (Color){ 50,  65, 100,  255 }

// ─── Helpers ──────────────────────────────────────────────────────

// Linear interpolation for colours
static Color ColorLerp(Color a, Color b, float t) {
    return (Color){
        (unsigned char)(a.r + (b.r - a.r) * t),
        (unsigned char)(a.g + (b.g - a.g) * t),
        (unsigned char)(a.b + (b.b - a.b) * t),
        (unsigned char)(a.a + (b.a - a.a) * t),
    };
}

// Draw a rounded-rectangle button; returns true when clicked
static bool DrawButton(Button *btn, float dt) {
    Vector2 mouse = GetMousePosition();
    btn->hovered = CheckCollisionPointRec(mouse, btn->rect);

    // Animate hover progress
    float target = btn->hovered ? 1.0f : 0.0f;
    btn->hoverT += (target - btn->hoverT) * dt * 10.0f;
    float t = btn->hoverT;

    // Fill
    Color fill = ColorLerp(COL_BTN, COL_BTN_HOV, t);
    DrawRectangleRounded(btn->rect, 0.18f, 8, fill);

    // Accent left-edge bar (grows on hover)
    float barH = btn->rect.height * 0.6f * t;
    float barY = btn->rect.y + (btn->rect.height - barH) * 0.5f;
    DrawRectangleRounded(
        (Rectangle){ btn->rect.x, barY, 4, barH },
        1.0f, 4,
        ColorLerp(COL_ACCENT, COL_ACCENT2, t)
    );

    // Border
    Color borderCol = ColorLerp(COL_BORDER, COL_ACCENT, t * 0.7f);
    DrawRectangleRoundedLines(btn->rect, 0.18f, 8, borderCol);

    // Label — shift slightly on hover
    float offX = t * 6.0f;
    int fontSize = 20;
    int textW = MeasureText(btn->label, fontSize);
    float textX = btn->rect.x + (btn->rect.width - textW) * 0.5f + offX;
    float textY = btn->rect.y + (btn->rect.height - fontSize) * 0.5f;
    Color labelCol = ColorLerp(COL_TEXT, COL_WHITE, t);
    DrawText(btn->label, (int)textX, (int)textY, fontSize, labelCol);

    return btn->hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

// Draw a simple placeholder screen with a back button; returns true to go back
static bool DrawPlaceholderScreen(const char *title, const char *subtitle, float *backT) {
    Vector2 mouse = GetMousePosition();
    int sw = GetScreenWidth(), sh = GetScreenHeight();

    // Panel
    int pw = 600, ph = 300;
    Rectangle panel = { (sw - pw) / 2.0f, (sh - ph) / 2.0f, pw, ph };
    DrawRectangleRounded(panel, 0.08f, 8, COL_PANEL);
    DrawRectangleRoundedLines(panel, 0.08f, 8, COL_BORDER);

    // Accent top strip
    DrawRectangleRounded(
        (Rectangle){ panel.x, panel.y, panel.width, 4 },
        0.5f, 4, COL_ACCENT
    );

    // Title
    int titleSz = 32;
    int titleW = MeasureText(title, titleSz);
    DrawText(title, sw / 2 - titleW / 2, (int)panel.y + 30, titleSz, COL_WHITE);

    // Subtitle
    int subSz = 16;
    int subW = MeasureText(subtitle, subSz);
    DrawText(subtitle, sw / 2 - subW / 2, (int)panel.y + 80, subSz, COL_MUTED);

    // ── placeholder content lines ──
    for (int i = 0; i < 3; i++) {
        float lw = (i == 0) ? 340 : (i == 1) ? 260 : 200;
        DrawRectangleRounded(
            (Rectangle){ panel.x + 40, panel.y + 130 + i * 34, lw, 16 },
            0.5f, 4,
            (Color){ 40, 50, 80, 200 }
        );
    }

    // Back button
    Rectangle backBtn = { panel.x + pw / 2.0f - 80, panel.y + ph - 60, 160, 40 };
    bool backHov = CheckCollisionPointRec(mouse, backBtn);
    *backT += (backHov ? 1.0f : -1.0f) * GetFrameTime() * 8.0f;
    if (*backT < 0) *backT = 0;
    if (*backT > 1) *backT = 1;

    Color backFill = ColorLerp(COL_BTN, COL_BTN_HOV, *backT);
    DrawRectangleRounded(backBtn, 0.3f, 8, backFill);
    DrawRectangleRoundedLines(backBtn, 0.3f, 8, ColorLerp(COL_BORDER, COL_ACCENT, *backT * 0.7f));
    const char *backLabel = "< Back";
    int blW = MeasureText(backLabel, 18);
    DrawText(backLabel,
        (int)(backBtn.x + (backBtn.width - blW) * 0.5f),
        (int)(backBtn.y + (backBtn.height - 18) * 0.5f),
        18, ColorLerp(COL_TEXT, COL_WHITE, *backT)
    );

    return backHov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

// ─── Main ─────────────────────────────────────────────────────────
int main(void) {
    const int SW = 900, SH = 600;
    InitWindow(SW, SH, "Game Main Menu");
    SetTargetFPS(60);

    GameScreen current = SCREEN_MENU;

    // Particle system (background stars)
    #define MAX_PARTICLES 120
    Vector2 particles[MAX_PARTICLES];
    float pSpeed[MAX_PARTICLES];
    float pAlpha[MAX_PARTICLES];
    for (int i = 0; i < MAX_PARTICLES; i++) {
        particles[i] = (Vector2){ GetRandomValue(0, SW), GetRandomValue(0, SH) };
        pSpeed[i]    = GetRandomValue(10, 40) * 0.1f;
        pAlpha[i]    = GetRandomValue(30, 100) * 0.01f;
    }

    // Menu slide-in animation
    float menuAlpha = 0.0f;

    // Buttons
    float bw = 340, bh = 56, bx = (SW - bw) * 0.5f;
    Button buttons[3] = {
        { .rect = { bx, 260, bw, bh }, .label = "Start Game"             },
        { .rect = { bx, 334, bw, bh }, .label = "Edit Game Statistics"   },
        { .rect = { bx, 408, bw, bh }, .label = "Check Player Statistics" },
    };

    // Per-screen back button animation state
    float backT[3] = {0};

    // Scan-line time
    float scanTime = 0.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        scanTime += dt;

        // Drift particles upward
        for (int i = 0; i < MAX_PARTICLES; i++) {
            particles[i].y -= pSpeed[i] * dt;
            if (particles[i].y < 0) {
                particles[i].y = SH;
                particles[i].x = GetRandomValue(0, SW);
            }
        }

        // Fade-in on entering menu
        if (current == SCREEN_MENU && menuAlpha < 1.0f)
            menuAlpha += dt * 2.0f;
        if (current != SCREEN_MENU)
            menuAlpha = 0.0f;

        // ── Draw ──────────────────────────────────────────────────
        BeginDrawing();
        ClearBackground(COL_BG);

        // Starfield
        for (int i = 0; i < MAX_PARTICLES; i++) {
            float r = (pSpeed[i] > 2.5f) ? 2.0f : 1.0f;
            DrawCircle((int)particles[i].x, (int)particles[i].y, r,
                       Fade(COL_ACCENT, pAlpha[i]));
        }

        if (current == SCREEN_MENU) {
            // ── MENU ──────────────────────────────────────────────

            // Central panel glow (soft rectangle behind panel)
            DrawRectangleGradientV(SW/2 - 220, 100, 440, 380,
                Fade((Color){20,40,80,100}, menuAlpha),
                Fade(COL_BG, 0));

            // Panel
            Rectangle panel = { SW/2 - 210, 120, 420, 360 };
            DrawRectangleRounded(panel, 0.06f, 8, Fade(COL_PANEL, menuAlpha));
            DrawRectangleRoundedLines(panel, 0.06f, 8, Fade(COL_BORDER, menuAlpha));

            // Top accent bar (gradient strip)
            for (int x = 0; x < 420; x++) {
                float frac = x / 420.0f;
                Color c = ColorLerp(COL_ACCENT, COL_ACCENT2, frac);
                DrawRectangle((int)panel.x + x, (int)panel.y, 1, 3, Fade(c, menuAlpha));
            }

            // Title
            const char *title = "GAME TITLE";
            int tsz = 42;
            int tw = MeasureText(title, tsz);
            DrawText(title, SW/2 - tw/2, 155, tsz, Fade(COL_WHITE, menuAlpha));

            // Subtitle / tagline
            const char *sub = "A thrilling adventure awaits";
            int subSz = 15;
            int subW = MeasureText(sub, subSz);
            DrawText(sub, SW/2 - subW/2, 207, subSz, Fade(COL_MUTED, menuAlpha));

            // Divider
            DrawLineEx(
                (Vector2){ panel.x + 30, 238 },
                (Vector2){ panel.x + 390, 238 },
                1.0f, Fade(COL_BORDER, menuAlpha)
            );

            // Buttons
            for (int i = 0; i < 3; i++) {
                // Temporarily fade by menuAlpha (override colours)
                if (menuAlpha < 1.0f) {
                    // Just draw at reduced alpha by tinting with alpha
                    Color fill = ColorLerp(COL_BTN, COL_BTN_HOV, buttons[i].hoverT);
                    fill.a = (unsigned char)(fill.a * menuAlpha);
                    DrawRectangleRounded(buttons[i].rect, 0.18f, 8, fill);
                } else {
                    if (DrawButton(&buttons[i], dt)) {
                        current = (GameScreen)(i + 1);
                        backT[i] = 0;
                    }
                }
            }

            // Bottom hint
            const char *hint = "Use mouse to navigate";
            int hW = MeasureText(hint, 13);
            DrawText(hint, SW/2 - hW/2, 478, 13, Fade(COL_MUTED, menuAlpha * 0.6f));

        } else if (current == SCREEN_START_GAME) {
            if (DrawPlaceholderScreen(
                    "Start Game",
                    "Select a save slot to begin your adventure.",
                    &backT[0]))
                current = SCREEN_MENU;

        } else if (current == SCREEN_EDIT_STATS) {
            if (DrawPlaceholderScreen(
                    "Edit Game Statistics",
                    "Modify difficulty, rules, and world settings.",
                    &backT[1]))
                current = SCREEN_MENU;

        } else if (current == SCREEN_PLAYER_STATS) {
            if (DrawPlaceholderScreen(
                    "Player Statistics",
                    "View your achievements, scores, and history.",
                    &backT[2]))
                current = SCREEN_MENU;
        }

        // Subtle scanline overlay (every other row at 4% opacity)
        for (int y = 0; y < SH; y += 4) {
            DrawRectangle(0, y, SW, 1, (Color){0, 0, 0, 10});
        }

        // FPS (dev helper – remove for release)
        DrawFPS(12, 12);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
