#include "raylib.h"
#include "menu.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MENU_OPTIONS 3

const char *options[MENU_OPTIONS] = { "Jogar", "Como jogar?", "Créditos" };

void MostrarMenu(void) {
    int selectedOption = 0;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Heart!");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_DOWN)) selectedOption = (selectedOption + 1) % MENU_OPTIONS;
        if (IsKeyPressed(KEY_UP)) selectedOption = (selectedOption - 1 + MENU_OPTIONS) % MENU_OPTIONS;

        if (IsKeyPressed(KEY_ENTER)) {
            if (selectedOption == 0) break; // Começar o jogo
            else if (selectedOption == 1) TraceLog(LOG_INFO, "Como jogar");
            else if (selectedOption == 2) TraceLog(LOG_INFO, "Créditos");
        }

        BeginDrawing();
        ClearBackground(DARKBLUE);
        DrawText("Heart!", 330, 80, 50, RED);

        for (int i = 0; i < MENU_OPTIONS; i++) {
            Color color = (i == selectedOption) ? YELLOW : WHITE;
            DrawText(options[i], 350, 200 + i * 50, 30, color);
        }

        EndDrawing();
    }

    CloseWindow();
}
