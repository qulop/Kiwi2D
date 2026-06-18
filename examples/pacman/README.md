# Pac-Man (Kiwi2D example)

A grid/maze game built on the Kiwi2D engine, in the spirit of the arcade classic. It
complements the Arkanoid example by exercising the engine with tile-based movement,
multiple AI actors, and a larger static world. It uses the ECS (`Scene` / `Entity` /
`Component`), `Transform2D`, the orthographic `Camera2D`, the OpenGL 4.6 batched
`Renderer`, the `InputSubsystem`, and the Dear ImGui HUD.

## Controls

| Key | Action |
| --- | --- |
| `W` / `Up`    | Move up |
| `S` / `Down`  | Move down |
| `A` / `Left`  | Move left |
| `D` / `Right` | Move right |
| `Enter` | Restart after win / game over |
| `Esc` | Quit |

Turns are buffered: press a direction before reaching a junction and Pac-Man turns as
soon as the way opens.

## How it works

- **`Components.hpp`** — gameplay components: `GridMover` (tile coordinates + smooth
  interpolation between tiles), `PacmanComponent` (player tag + chomp animation), and
  `GhostComponent` (colour, scatter corner, frightened state).
- **`PacmanApp`** — an `Application` subclass that parses a text maze into a tile grid,
  spawns the player and four ghosts as entities, and each frame:
  - moves actors one tile at a time with sub-tile interpolation,
  - eats pellets / power pellets (power pellets frighten the ghosts),
  - runs a lightweight ghost AI (chase Pac-Man, occasionally wander, flee when
    frightened, no instant reversals),
  - resolves Pac-Man/ghost collisions (lose a life, or eat a frightened ghost),
  - tracks score / lives / win / game-over,
  - submits quads to the `Renderer` and draws the ImGui HUD.
- **`Main.cpp`** — defines `Kiwi::EntryPoint` (and `WinMain`) and boots `PacmanApp`.

The maze is laid out as an array of equal-length strings (`#` wall, `.` pellet,
`o` power pellet, `P` player spawn, `G` ghost spawn). The world is sized to the maze
(`cols x rows` tiles of `TileSize` world units) and mapped 1:1 by the camera.

## Building

The example is built by default with the engine. From the project root:

```bash
cmake -S . -B build
cmake --build build --target Pacman
```

To skip building examples, configure with `-DKIWI_BUILD_EXAMPLES=OFF`.

The build copies the engine `resources/` folder next to the executable as a post-build
step.
