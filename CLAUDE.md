# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

A 42 `miniRT` ray tracer in C, extended far past the base subject into a CPU path tracer:
BVH acceleration, PDF importance sampling, multiple material/texture models, Perlin noise,
OBJ mesh loading, CIE-spectral color, ACES tone mapping, and OpenMP multithreading. The
renderer is built as a reusable static library (`librt.a`); `miniRT` and every standalone
test program under `studio/` are thin clients of it.

## Build & run

```sh
make                 # build build/bin/miniRT (also symlinked as ./miniRT)
make BONUS=1         # bonus build (adds -DBONUS)
make libs            # vendor libs + librt.a only
make studio          # all standalone test binaries (== make test / make tests)
make ppms|libx|sdl   # one test group (build/bin/{ppms,libx,sdl}/)
make build/bin/ppms/final_test   # build a single test binary by its output path
make cie             # regenerate CIE tables (already committed; needs vendor/scripts)
make clean|fclean|re

./miniRT scene.rt           # parse + render, open an MLX/X11 window
./miniRT --ppm scene.rt     # render headless, write ./render.ppm instead
./miniRT scene.json         # extended JSON scene format
./miniRT scene.obj          # load a Wavefront mesh into a default scene
```

Input is dispatched by file extension in `srcs/app/main.c`. Note the asymmetry: a plain `.rt`
goes through `rt_run()` (full interactive path); `.rt --ppm`, `.json`, and `.obj` all go
through `display_scene()`. Most `studio/tests/ppms/*` binaries write a PPM/PNG when run.

## ⚠️ Vendor libraries are missing from the working tree

`vendor/{libft, minilibx-linux, png_writer, scripts, evals42}` are recorded as **gitlinks**
(bare commit pointers) but are **not** registered in `.gitmodules` and are **empty as checked
out**. `git submodule update --init` will NOT populate them. The build needs them present:
`libft` (42 libc), `minilibx-linux` (X11 windowing), `png_writer` (lodepng image IO). Only
`vendor/wavefront` (the OBJ loader) is committed normally; `example_rt` is a real submodule
(a third-party reference tracer, not part of the build). **`make` fails at the `[vendor]`
stage until those directories are populated** — clone/copy them in before building.

## Architecture

Three compile targets, layered as a library + clients (see the root `Makefile`):

- **`librt.a`** ← `srcs/core/**` + `srcs/engine/**` + `srcs/app/settings.c`. The renderer.
  - `core/` — `types/`, `math/` (vector, matrix, interval, color, spectrum, point),
    `geometry/` (hittable, hittable_list, sphere, cylinder, quad, triangle, ray),
    `io/` (gnl, error, lodepng, ft_atof), `utils/` (random).
  - `engine/` — `accelerators/` (aabb, bvh), `camera/` (camera + the path-tracing renderer),
    `materials/`, `sampling/` (pdf), `textures/` (texture, perlin, noise).
- **`miniRT`** binary ← `srcs/app/**` (main + `parser/{rt,json,csv,obj}`), links `librt.a`.
- **studio test programs** ← `studio/tests/{ppms,libx,sdl}/*.c`, one binary per file, each
  links `librt.a` directly (they bypass `main.c` and exercise the library in isolation).

### Polymorphism via function pointers (read these together — the core idea)

C "objects" are fat structs of function pointers. Three families:

- **`t_hittable_wrapper`** (`core/geometry/hittable.h`) — `{ void *object; bool owned;
  set_current; hit_noobj; t_aabb bbox }`. Intersection is a **two-step call**: `set_current(obj)`
  stashes the concrete object in a file-static, then `hit_noobj(ray, interval, rec)` tests
  against that stashed object. Every geometry plugs in this way — sphere, quad, cylinder,
  triangle, BVH node, list, and the `translate`/`rotate_y` transform wrappers. A
  `t_hittable_list` is a growable array of wrappers **and is itself wrappable**, so lists nest
  and a BVH wraps the whole world.
- **`t_material`** (`engine/materials/material.h`) — `{ data; emitted; scatter; scattering_pdf;
  destroy }`. Concrete kinds: lambertian, metal, dielectric, glossy, tinted_glass,
  diffuse_light, isotropic. Allocate with `*_create()`, free through `mat->destroy`.
- **`t_texture`** (`engine/textures/`) — same shape (solid color, checker, image, Perlin noise).

### Scene pipeline

`parse → build_scene_objects → bvh_node_create → render_to_buffer → window or render.ppm`.

Parsing fills a `t_scene` (`srcs/app/parser/rt/rt_parser.h`): raw parsed primitives live in a
fixed `t_rt_object[]` array (a tagged union by `t_obj_type`), **separate from** engine geometry.
`build_scene_objects()` converts those into `t_hittable_wrapper`s in `scene.world`;
`add_scene_lights()` turns point lights into emissive spheres. Then a BVH is built over the
world and `render_to_buffer()` produces an RGB byte buffer.

The renderer (`engine/camera/` + `srcs/app/parser/rt/render.c`) is an OpenMP-parallel, tiled,
stratified path tracer with Russian-roulette termination, BVH traversal, PDF importance
sampling, defocus blur, and an ACES + CIE/sRGB/gamma post-process.

### Input formats

- `.rt` — 42 format. Line dispatch table in `parser/rt/parse.c`: `A` ambient, `C` camera,
  `L` light, `sp` sphere, `pl` plane, `cy` cylinder, `co` cone, `tr` triangle.
- `.json` — richer scene format (per-object `material` blocks, a `render` quality block →
  `t_render_opts`). See `studio/assets/jsons/*.json` and `*.json` under `studio/assets/rt_files/`.
- `.obj` — Wavefront mesh via `vendor/wavefront`, wrapped in a default camera/light scene.

## Configuration

- **`studio/studio_config.h`** — the master knob header. Every `RT_*` macro (image size,
  samples-per-pixel, max depth, thread count, tile size, camera FOV/defocus, OBJ-pipeline
  defaults, lighting, exposure/tone-map/gamma) is `#ifndef`-guarded, so override at build time
  with `-D…` or per-scene via the JSON `render` block. Values are runtime-clamped, never crash.
- **`srcs/app/settings.h`** — `IMAGE_WIDTH/HEIGHT` (256, used by studio tests),
  `RT_COLOR_PROFILE`, `PI`/`INFINITY`, and the `X/Y/Z/W` coordinate-index constants.

## Conventions

- **42 Norm.** Strict flags, warnings are errors: `-std=c99 -O3 -march=native -flto -fopenmp
  -Wall -Wextra -Werror`. Archiver is `gcc-ar` (required for `-flto`). Every file carries the
  42 header banner.
- **Numbered file split** (`vector2-5.c`, `material2-7.c`, `camera2-4.c`, `interval2-3.c`): the
  Norm caps 5 functions/file and 25 lines/function, so a module's functions overflow into
  numbered siblings. Add a new function to the lowest-numbered sibling with room, or a new one.
- **Flat includes.** The Makefile adds one `-I` per source subdirectory, so headers are
  included by bare name (`#include "vector.h"`, `"studio_config.h"`) regardless of location. A
  new source directory must have its `-I` added to the `INC_*` sets in the Makefile.
- **Numeric type.** `real_t` is `double` (→ `float` with `-DFLOAT_TYPE`), defined in
  `core/types/types.h`; `t_vec3`, `t_color`, and `t_point3` are the same underlying struct.
