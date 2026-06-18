# Snake (Kiwi2D example)

The classic Snake, built on the Kiwi2D engine. It rounds out the example set
(Arkanoid, Pac-Man, Snake) with a fixed-timestep, grid-based game. It uses the
orthographic `Camera2D`, the OpenGL 4.6 batched `Renderer`, the `InputSubsystem`, and
the Dear ImGui HUD.

## Controls

| Key | Action |
| --- | --- |
| `W` / `Up`    | Turn up |
| `S` / `Down`  | Turn down |
| `A` / `Left`  | Turn left |
| `D` / `Right` | Turn right |
| `Enter` | Restart after game over |
| `Esc` | Quit |

Any steering key starts the round. You can't reverse straight back into your own neck.

## How it works

- **`SnakeApp`** — an `Application` subclass that owns the board state and a `Camera2D`
  sized to the `GridCols x GridRows` tile grid. The snake body is a `std::deque<Cell>`
  (head at the front). Each frame:
  - input updates a *pending* direction (rejecting direct reversals),
  - a fixed-timestep accumulator advances the snake one cell per `m_stepInterval`,
  - eating food grows the snake (no tail pop), bumps the score, and slightly speeds the
    game up to a floor,
  - wall and self collisions end the round; filling the whole board wins it,
  - the board, food, and snake are submitted to the `Renderer` and an ImGui HUD shows
    score / best / length / state.
- **`Main.cpp`** — defines `Kiwi::EntryPoint` (and `WinMain`) and boots `SnakeApp`.

The world is `cols x rows` tiles of `TileSize` world units, mapped 1:1 by the camera.

## Building

The example is built by default with the engine. From the project root:

```bash
cmake -S . -B build
cmake --build build --target Snake
```

To skip building examples, configure with `-DKIWI_BUILD_EXAMPLES=OFF`.

The build copies the engine `resources/` folder next to the executable as a post-build
step.
