#!/usr/bin/env python3
"""Generate the iconic 'Ray Tracing in One Weekend' final scene as a minirt JSON.

Many small random-material spheres scattered on a ground plane behind three
large hero spheres (glass, diffuse brown, polished metal). Run:

    python3 gen_rtow_final.py > rtow_final.json

NOTE: the engine caps a scene at 256 objects (RT_MAX_OBJECTS), so the small
field is budgeted to ~248 spheres and the heroes are emitted first to guarantee
they survive the cap.
"""
import json
import math
import random

random.seed(7)  # reproducible cover layout

MAX_OBJECTS = 256          # engine hard cap (RT_MAX_OBJECTS)
HERO_R = 1.2               # hero sphere radius -> diameter 2.4
SMALL_R = 0.2


def to255(c):
    return [round(max(0.0, min(1.0, x)) * 255) for x in c]


def rand_color(lo=0.0, hi=1.0):
    return [random.uniform(lo, hi) for _ in range(3)]


objects = []

# --- ground: large-scale subtle checker + faint gloss so the heroes feel
#     grounded and the plane reflects the sky a touch (kills the sterile flat). ---
objects.append({
    "_comment": "ground plane — subtle large checker, faint gloss",
    "type": "plane",
    "point": [0, 0, 0],
    "normal": [0, 1, 0],
    "color": to255([0.70, 0.72, 0.76]),
    "material": {
        "type": "checker",
        "scale": 2.0,
        "color2": to255([0.56, 0.58, 0.62]),
    },
})

# --- three hero spheres FIRST so they always survive the 256-object cap ---
heroes = [(-4.0, HERO_R, 0.0), (0.0, HERO_R, 0.0), (4.0, HERO_R, 0.0)]
HD = round(HERO_R * 2, 3)
objects.append({
    "_comment": "hero — diffuse brown (left)",
    "type": "sphere", "center": [-4, HERO_R, 0], "diameter": HD,
    "color": to255([0.4, 0.2, 0.1]), "material": "lambertian",
})
objects.append({
    "_comment": "hero — glass (centre)",
    "type": "sphere", "center": [0, HERO_R, 0], "diameter": HD,
    "color": [255, 255, 255], "material": {"type": "dielectric", "ior": 1.5},
})
objects.append({
    "_comment": "hero — polished metal (right)",
    "type": "sphere", "center": [4, HERO_R, 0], "diameter": HD,
    "color": to255([0.72, 0.62, 0.50]), "material": {"type": "metal", "fuzz": 0.02},
})

# --- candidate small spheres: a wide field that lives BEHIND the trio so the
#     heroes sit on a clean apron with the colourful carpet receding behind. ---
cands = []
for a in range(-14, 15):
    for b in range(-22, -1):
        cx = a + 0.9 * random.random()
        cz = b + 0.9 * random.random()

        if cz > -1.55:                      # nothing in front of / on the trio
            continue
        too_close = False
        for hx, hy, hz in heroes:
            if math.hypot(cx - hx, cz - hz) < HERO_R + 1.2:
                too_close = True
                break
        if too_close:
            continue
        cands.append((cx, cz))

# Budget: thin the candidate field uniformly so the carpet keeps real DEPTH
# (recedes toward the horizon) instead of collapsing to one dense row, while
# staying under the 256-object cap.
budget = MAX_OBJECTS - len(objects)
random.shuffle(cands)
cands = cands[:budget]

count = 0
for cx, cz in cands:
    choose = random.random()
    if choose < 0.76:
        albedo = [rand_color()[i] * rand_color()[i] for i in range(3)]
        mat, color = "lambertian", to255(albedo)
    elif choose < 0.92:
        fuzz = random.uniform(0.0, 0.5)
        mat = {"type": "metal", "fuzz": round(fuzz, 3)}
        color = to255(rand_color(0.5, 1.0))
    else:
        mat, color = {"type": "dielectric", "ior": 1.5}, [255, 255, 255]

    objects.append({
        "type": "sphere",
        "center": [round(cx, 3), SMALL_R, round(cz, 3)],
        "diameter": 0.4,
        "color": color,
        "material": mat,
    })
    count += 1

scene = {
    "_description": (
        "Ray Tracing in One Weekend — the final cover scene. Three hero spheres "
        "(glass, diffuse brown, polished metal) on a clean apron, backed by %d "
        "small random-material spheres. Soft daytime sky, shallow depth of "
        "field." % count
    ),
    "ambient": {"ratio": 0.45, "color": [205, 222, 255]},
    "render": {"aspect_ratio": 1.7778, "spp": 120, "max_depth": 24},
    "camera": {
        "position": [0.0, 2.7, 12.0],
        "direction": [0.0, -1.4, -12.0],
        "fov": 38,
        "defocus_angle": 0.35,
        "focus_dist": 12.1,
    },
    "lights": [
        {"position": [20, 30, 10], "brightness": 1.0, "color": [255, 248, 235]},
        {"position": [-18, 16, -8], "brightness": 0.45, "color": [200, 218, 255]},
    ],
    "objects": objects,
}

assert len(objects) <= MAX_OBJECTS, len(objects)
print(json.dumps(scene, indent=2))
