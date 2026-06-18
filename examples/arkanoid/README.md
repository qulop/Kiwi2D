# Arkanoid (Kiwi2D example)

A small, complete Arkanoid/Breakout clone built on the Kiwi2D engine. It exercises the
engine end-to-end: the ECS (`Scene` / `Entity` / `Component`), `Transform2D`, the
orthographic `Camera2D`, the OpenGL 4.6 batched `Renderer`, the `InputSubsystem`, and the
Dear ImGui debug/HUD overlay.

## Controls

| Key | Action |
| --- | --- |
| `A` / `Left`  | Move paddle left |
| `D` / `Right` | Move paddle right |
| `Space` / `Up` | Launch the ball |
| `Enter` | Restart after win / game over |
| `Esc` | Quit |

## How it works

- **`Components.hpp`** — gameplay data components: `PaddleComponent`, `BallComponent`,
  `BrickComponent` (all derive from the engine's `Component`).
- **`ArkanoidApp`** — an `Application` subclass that owns the `Scene` and `Camera2D`,
  builds the brick grid, runs the per-frame gameplay tick (paddle movement, ball
  integration, AABB collision against paddle/bricks/walls, lives/score/win state), and
  submits quads to the `Renderer` each frame via `BeginScene` / `SubmitDraw` / `EndScene`.
- **`Main.cpp`** — defines `Kiwi::EntryPoint` (and `WinMain`) and boots `ArkanoidApp`.

The world is a fixed `1280 x 720` logical space mapped 1:1 by the camera
(`Camera2D(0, W, 0, H)`), so positions and sizes are expressed directly in those units.

## Building

The example is built by default with the engine. From the project root:

```bash
cmake -S . -B build
cmake --build build --target Arkanoid
```

To skip building examples, configure with `-DKIWI_BUILD_EXAMPLES=OFF`.

The build copies the engine `resources/` folder next to the executable as a post-build
step.
