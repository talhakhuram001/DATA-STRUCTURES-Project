Enchanted Labyrinth Explorer - Data Structures Project

Overview
--------
This repository contains a C++ desktop project that implements a simple 2D maze game using SFML plus a separate AVL tree data-structure implementation. The game demonstrates basic data-structure usage (linked lists, simple object lists), file I/O (scoreboard), and 2D rendering and audio using SFML.

Project layout
--------------
- `test.sln` and `test/test.vcxproj` - Visual Studio solution and project files (x64 Debug configured).
- `Source.cpp` - Main program: `main()` shows the menu and launches the game. Also contains `Menu` and `Game` classes and run loop.
- `maze.h` - Core game world and entity definitions:
  - `Maze` - grid, drawing and collision data.
  - `Player` - player position, movement, and player-fired projectiles.
  - `Enemy`, `Enemylist` - enemy entities, movement and firing.
  - `Treasure`, `Treeasurelist` - collectible items and rewards.
  - `Fire` - projectile logic and drawing.
- `highscore.h` - `ScoreList` linked-list implementation storing `score_body` nodes and reading/writing `scores.txt`.
- `AVL.h` - Generic templated AVL tree implementation (`node<T>`, `AVL<T>`) and a `values_here` test helper. This file is a standalone data-structures assignment/example.
- `scores.txt` - Sample scoreboard file. Each line uses the format: `Name Score` (whitespace separated).
- Asset files (images, fonts, audio) in the same folder (png, jpg, ogg, ttf). The game loads these at runtime.

Dependencies
------------
- C++ compiler: MSVC (Visual Studio 2019/2022 recommended) targeting x64.
- SFML (Graphics, Window, Audio, System). A tested version is SFML 2.5.1.

Build and run (Windows / Visual Studio)
-------------------------------------
1. Install Visual Studio with Desktop development for C++ and the MSVC x64 toolset.
2. Install SFML (choose one):
   - Download precompiled SFML for MSVC from http://www.sfml-dev.org and follow the "Getting started" guide for Visual Studio.
   - Or install via vcpkg and integrate with Visual Studio:
     - `vcpkg install sfml:x64-windows`
     - `vcpkg integrate install`
3. Open `test.sln` in Visual Studio.
4. Configure project properties to link to SFML libraries and copy DLLs next to the executable if using dynamic linking.
5. Build the solution (x64, Debug or Release).
6. Run from Visual Studio or run the built executable at `x64/Debug/test.exe`.

Notes for command-line builds
----------------------------
- Building from the command line is possible with the MSVC Developer Command Prompt; you must supply include and lib paths for SFML and link required libraries (`sfml-graphics`, `sfml-window`, `sfml-system`, `sfml-audio`). The provided Visual Studio project is the simplest route.

Runtime
-------
- The executable expects the asset files (images, fonts, audio) to be in the same working directory as the binary. If running from Visual Studio the working directory is configured in the project and already points to the project folder.
- Score file: `scores.txt` is used by `ScoreList::readFromFile()` and expects lines like `Alice 200`.

Key design notes
----------------
- The game uses raw pointers and manual dynamic allocation for lists and entity storage (e.g., `Enemylist`, `Treeasurelist`, `Fire` lists). Consider migrating to `std::vector` and smart pointers to reduce memory leaks.
- `AVL.h` is a self-contained templated AVL tree used as a data-structures example and not integrated with the game runtime.

Known issues and suggestions
----------------------------
- `values_here::operator!=` implementation in `AVL.h` currently returns `data >= other.data` which is incorrect. It should return `data != other.data`.
- Many classes allocate memory with `new` without matching `delete` in all paths; consider auditing lifetime management and using RAII (smart pointers).
- Error handling for loading assets is sometimes minimal. If an asset fails to load the program may continue and later crash; adding clearer error messages and early exits is recommended.

Possible next steps
-------------------
- Fix `values_here::operator!=` in `AVL.h` and run a quick compile.
- Convert manual linked lists to `std::vector` or `std::list` where appropriate and introduce smart pointers.
- Add a top-level `README.md` (this file), and optionally a CONTRIBUTING.md with build details.
- Add unit tests for `AVL.h` using a lightweight test framework if desired.

License
-------
No license file is included in the repository. Add a LICENSE if you plan to publish the code publicly.

Contact / Author
----------------
This repository appears to be a student project. Replace this section with your name and contact details if desired.
