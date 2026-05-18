# CMSC-21---FINAL-PROJECT---PICKLINGBALLS
A program that tracks all the statistics of a standard pickleball game. This includes but not limited to: score, time, players, and current player serving. This program is for the submission of the final project in CMSC 21.


## Objectives
* **Accurate Tracking:** Replaces manual scoring by tracking points, faults, rallies, and aces in real-time.
* **Match History:** Logs previously played matches, including dates, players, and final scores into a local database.
* **Intuitive GUI:** Designed so anyone can officiate a game without prior technical knowledge.

## Core Features
1. **Main Menu:** Easy navigation between "Start Tracking", "Match History", and "Exit".
2. **Match Setup:** Input player names and utilize a randomizer (coin flip) to determine the first server.
3. **Live Dashboard:** Displays the server indicator, match timer, and live score. 
4. **Stat Logging:** Buttons to record faults, aces, and standard points.
5. **Persistent Storage:** Saves match history to a file upon exiting the program so records are never lost.


## Project Structure

While the project contains several files, the core UI and drawing logic is divided into modular components:

| File | Description |
|------|-------------|
| `drawmenu_fx_defn.c` | Handles the main menu, title screen, and player name text inputs. |
| `drawcourt_fx_defn.c` | Contains the 3D rendering logic (`DrawCourt`) for the court surface, net, players, and boundaries. |
| `drawhistscreen_fx_defn.c` | Manages the Match History screen, search parsing, and dynamic UI animations. |
| `types.h` | Contains the core data structures, including the global `Appstate` which tracks game variables, input buffers, and animation states. |
| `constants.h` | Defines global constants like screen width, height, and maximum string lengths. |

## File Structure

```text
📦 CMSC-21-FINAL-PROJECT-PICKLEMYBALLS
├── .vscode/                  # Visual Studio Code build and debug configurations
├── include/                  
│   └── raylib.h              # Main raylib library header
├── lib/                      
│   └── libraylib.a           # Static raylib library for linking
├── src/                      
│   └── Function_Prototypes/  # Core modular application code
│       ├── constants.h              
│       ├── types.h                  
│       ├── draw_fx_prototypes.h     
│       ├── drawmenu_fx_defn.c       
│       ├── drawcourt_fx_defn.c      
│       ├── drawgamescreen_fx_defn.c 
│       ├── drawhistscreen_fx_defn.c 
│       ├── game_fx_prototypes.h     
│       ├── game_fx_defn.c           
│       ├── scoring_fx_prototypes.h  
│       └── scoring_fx_defn.c        
├── main.c                    
├── Makefile                  # Build instructions for compiling the project
├── matchHistory.txt          # Local database file storing saved matches
├── README.md                 
```
---

## Prerequisites

To build and run this project, you will need:

- A C compiler (e.g., GCC, Clang, or MSVC)
- [raylib](https://github.com/raysan5/raylib) installed and configured on your system

---

## How to Build and Run

> **Note:** Adjust the compilation command based on your specific OS and setup.

### Linux / macOS (GCC)

```bash
gcc *.c -o pickleball_tracker -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./pickleball_tracker
```

### Windows / MinGW (GCC)

```bash
gcc main.c src/Function_Prototypes/*.c -o tracker.exe -I include/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm
```

---

## Usage Guide

1. **Launch the App** — You will be greeted by the Main Menu.
2. **Enter Names** — Click on the **Player 1** or **Player 2** text boxes and type the names of the competitors. Press `ENTER` to save the name.
3. **Start Match** — Once both names are entered, the **START MATCH** button will unlock.
4. **View History** — Click **MATCH HISTORY** to view all saved games.
5. **Search** — In the history screen, click the search bar and type a name to instantly filter the list. Use tags like `winner:john` to filter by specific parameters. Click **CLEAR** to reset the list.

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
