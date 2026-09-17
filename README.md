# CMSC-21---FINAL-PROJECT---PICKLINGBALLS
A program that tracks all the statistics of a standard pickleball game. This includes but not limited to: score, time, players, and current player serving. This program is for the submission of the final project in CMSC 21.


## Objectives
* **Accurate Tracking:** Replaces manual scoring by tracking points, faults, and aces in real-time.
* **Match History:** Logs previously played matches, including dates, players, and final scores into a local database.
* **Intuitive GUI:** Designed so anyone can officiate a game without prior technical knowledge.

## Core Features
1. **Main Menu:** Easy navigation between "Start Tracking", "Match History", and "Exit".
2. **Match Setup:** Input player names.
3. **Live Dashboard:** Displays the server indicator, match timer, and live score. 
4. **Stat Logging:** Buttons to record faults, aces, and standard points.
5. **Persistent Storage:** Saves match history to a file upon exiting the program so records are never lost.


## Project Structure

The project separates application state, game logic, file storage, and drawing code. `main.c` creates the raylib window, initializes the application state, loads saved history, and routes each frame to the active screen.

| Location | Responsibility |
|----------|----------------|
| `main.c` | Application entry point and main game loop. |
| `src/constants.h` | Shared limits, scoring rules, and screen identifiers. |
| `src/types.h` | Shared `Player`, `Match`, `SavedMatch`, `UIState`, and `Appstate` data structures. |
| `src/core/` | Match lifecycle, scoring, player statistics, and match-history storage. |
| `src/UI/` | Main-menu, game, history, and court rendering code. |
| `include/` and `lib/` | Bundled raylib header and static library used by the Windows build. |

## File Structure

```text
CMSC-21-FINAL-PROJECT-PICKLEMYBALLS
├── .vscode/                         # VS Code build/debug configuration
├── include/
│   └── raylib.h                      # Bundled raylib header
├── lib/
│   └── libraylib.a                   # Bundled raylib static library
├── src/
│   ├── constants.h                   # Shared configuration and limits
│   ├── types.h                       # Application data models and state
│   ├── core/                         # Game logic and persistent storage
│   │   ├── core_game.c/.h            # New-match initialization
│   │   ├── core_game_score.c/.h      # Score/stat updates, undo, and reset
│   │   ├── core_storage.c/.h         # Match-history save, load, and search
│   │   ├── match.c/.h                # Match state and server management
│   │   └── player.c/.h               # Player initialization and statistics
│   └── UI/                           # Rendering and screen interaction
│       ├── courtbg.c                 # 3D pickleball-court rendering
│       ├── ui_draw.h                 # Shared UI drawing declarations
│       ├── ui_game.c                 # Live match dashboard
│       ├── ui_history.c              # Match-history screen and search UI
│       └── ui_menu.c                 # Main menu and player-name entry
├── .gitignore
├── main.c                            # Program entry point
├── Makefile                          # Build rules
├── README.md
├── newcode.c                         # Earlier standalone source version
├── test1.c                           # Earlier standalone source version
└── tracker.exe                       # Windows build output
```

`matchHistory.txt` is created in the project directory at runtime when match data is saved.
---

## Prerequisites

To build and run this project, you will need:

- A C compiler (e.g., GCC, Clang, or MSVC)
- [raylib](https://github.com/raysan5/raylib) installed and configured on your system

---

## How to Build and Run

> **Note:** Adjust the compilation command based on your specific OS and setup.

### Windows / MinGW (GCC)

```bash
 gcc main.c src/UI/*.c src/core/*.c -o tracker.exe -O1 -Wall -std=c99 -Wno-missing-braces -I include/ -I src/ -I src/UI/ -I src/core/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm
```

This runs the app IFF it compiles with no errors.
```bash
make && tracker.exe
```
---

## Usage Guide

1. **Launch the App** — You will be greeted by the Main Menu.
2. **Enter Names** — Click on the **Player 1** or **Player 2** text boxes and type the names of the competitors. Press `ENTER` to save the name.
3. **Start Match** — Once both names are entered, the **START MATCH** button will unlock.
4. **View History** — Click **MATCH HISTORY** to view all saved games.
5. **Search** — In the history screen, click the search bar and type a name to instantly filter the list.

---

## Acknowledgements

Built with [raylib](https://www.raylib.com/)

---

## Authors

 - Carl Mathew Bartolo
 - Nelson John Imanuel Larena
 - Joshua Pabualan
 - Glaiza Nicole Rico
 - Rafael Victoriano
