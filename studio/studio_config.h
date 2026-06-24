/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   studio_config.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 17:40:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/11 00:50:39 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STUDIO_CONFIG_H
# define STUDIO_CONFIG_H

/* ================================================================== */
/*  miniRT Studio Configuration                                       */
/*                                                                    */
/*  Master control header — tweak any define below to change the      */
/*  default behaviour of the renderer, camera, OBJ pipeline, lights   */
/*  and colour processing.  Every value is #ifndef-guarded so it can  */
/*  be overridden from the compiler command line (-D flag) or from    */
/*  a JSON scene file at runtime.                                     */
/*                                                                    */
/*  Quick-start presets (uncomment ONE block or pass -DRT_PRESET=n):  */
/*    PREVIEW   : 400px,   16 spp, depth 10   — instant   (~1 s)     */
/*    DRAFT     : 800px,   64 spp, depth 25   — fast      (~10 s)    */
/*    STANDARD  : 800px,  400 spp, depth 50   — balanced  (~1 min)   */
/*    HIGH      :1200px,  800 spp, depth 80   — slow      (~5 min)   */
/*    ULTRA     :1920px, 2048 spp, depth 100  — very slow            */
/*                                                                    */
/*  All values are clamped at runtime — setting an absurd number      */
/*  will never crash the program, only saturate to a safe max.        */
/* ================================================================== */

/* ================================================================== */
/*  §1  RENDER QUALITY                                                */
/* ================================================================== */

/* ---- Image dimensions -------------------------------------------- */

/* Output width in pixels.  Height is derived from aspect ratio       */
/* unless RT_IMAGE_HEIGHT is explicitly set.                           */
/* Range: 64 – 7680.  Clamped at runtime.                             */
/* DEFAULT: 800 — good speed/quality balance for iteration.           */
# ifndef RT_IMAGE_WIDTH
#  define RT_IMAGE_WIDTH	800
# endif

/* Explicit output height in pixels.  When set, aspect ratio is       */
/* computed from width/height automatically.  When 0 (default),       */
/* height is derived from RT_ASPECT_RATIO.                            */
/* Range: 0 (auto) or 64 – 4320.  Clamped at runtime.                */
# ifndef RT_IMAGE_HEIGHT
#  define RT_IMAGE_HEIGHT		0
# endif

/* Aspect ratio as a decimal (16:9 ≈ 1.7778, 4:3 ≈ 1.3333, 1:1=1.0) */
/* Only used when RT_IMAGE_HEIGHT == 0.                               */
/* Range: 0.25 – 4.0.  Clamped at runtime.                           */
# ifndef RT_ASPECT_RATIO
#  define RT_ASPECT_RATIO		1.7778
# endif

/* ---- Sampling ---------------------------------------------------- */

/* Samples per pixel — controls noise.  More = cleaner, slower.       */
/* Range: 1 – 16384.  Clamped at runtime.                             */
/* DEFAULT: 100 — fast enough for iteration; use 400-800 for finals.  */
# ifndef RT_SAMPLES_PER_PIXEL
#  define RT_SAMPLES_PER_PIXEL 100
# endif

/* Maximum ray-bounce depth before a path is terminated.              */
/* Range: 1 – 500.  Clamped at runtime.                               */
# ifndef RT_MAX_DEPTH
#  define RT_MAX_DEPTH			50
# endif

/* ---- Russian roulette -------------------------------------------- */

/* Depth at which Russian roulette path termination begins.           */
/* After this many bounces, low-contribution paths are terminated     */
/* probabilistically for an unbiased speedup.                         */
/* 0 = disabled.  Typical: 3 – 10.  Range: 0 – 100.                  */
# ifndef RT_RR_START_DEPTH
#  define RT_RR_START_DEPTH		5
# endif

/* ---- Tile-based rendering ---------------------------------------- */

/* Tile side in pixels for cache-friendly rendering.                  */
/* Pixels within a tile share similar rays → better L1/L2 hits.       */
/* 0 = row-based (legacy).  Typical: 16 – 64.  Range: 0, 8 – 256.    */
# ifndef RT_TILE_SIZE
#  define RT_TILE_SIZE			32
# endif

/* ---- Threading --------------------------------------------------- */

/* Number of OpenMP threads used for rendering.                       */
/*   0 = auto-detect (use all available cores — default, fastest)     */
/*   N = use exactly N threads (e.g. 4, 8, 16)                       */
/* Reduce to leave CPU headroom for other tasks.                      */
/* Range: 0 (auto) or 1 – 256.                                       */
# ifndef RT_NUM_THREADS
#  define RT_NUM_THREADS		0
# endif

/* ---- Deterministic (direct) engine ------------------------------- */

/* Recursion cap for the default direct engine.  Bounds mirror/glass     */
/* bounce depth (glass traces 2 rays per hit, so keep this modest).      */
/* Range: 1 - 32.                                                        */
# ifndef RT_FAST_MAX_DEPTH
#  define RT_FAST_MAX_DEPTH		8
# endif

/* Use the flattened SAH/median BVH (iterative, float slab test) for the      */
/* deterministic engine's hot path instead of the recursive wrapper BVH.      */
/*   1 = flat BVH (default, fastest)                                          */
/*   0 = legacy wrapper BVH (for A/B correctness comparison)                  */
/* Has no effect on the cinematic path tracer, which always uses the wrapper. */
# ifndef RT_FAST_BVH
#  define RT_FAST_BVH			1
# endif

/* Blinn-Phong specular strength ("shine effect").  0 = matte.           */
/* Range: 0.0 - 1.0.                                                     */
# ifndef RT_SPECULAR_KS
#  define RT_SPECULAR_KS		0.35
# endif

/* Specular exponent — higher = tighter, glossier highlight.             */
/* Range: 1 - 1024.                                                      */
# ifndef RT_SHININESS
#  define RT_SHININESS			64.0
# endif

/* ---- Adaptive edge anti-aliasing (direct engine only) ------------ */

/* Anti-aliasing mode for the deterministic (direct) engine.          */
/*   0 = off  (byte-reproduces the legacy 1-centred-ray output)       */
/*   1 = adaptive: supersample only silhouette/edge pixels.           */
/* Has no effect on the cinematic path tracer.                        */
# ifndef RT_AA_MODE
#  define RT_AA_MODE			1
# endif

/* Sub-samples averaged on a flagged edge pixel (fixed rotated grid). */
/* Implemented as a 2x2 deterministic grid.  Range: 1 - 16.           */
# ifndef RT_AA_SAMPLES
#  define RT_AA_SAMPLES			4
# endif

/* Luma delta (0..1, BT.709) versus the right/down neighbour above    */
/* which a pixel is treated as an edge and supersampled.              */
/* Lower = more pixels refined (slower); higher = fewer.              */
# ifndef RT_AA_EDGE_THRESH
#  define RT_AA_EDGE_THRESH		0.08
# endif

/* ================================================================== */
/*  §2  CAMERA DEFAULTS                                               */
/* ================================================================== */

/* Vertical field-of-view for .rt / .json scenes (degrees).           */
/* Range: 1 – 179.  Clamped at runtime.                               */
# ifndef RT_DEFAULT_FOV
#  define RT_DEFAULT_FOV		70.0
# endif

/* Depth-of-field: defocus (aperture) angle in degrees.               */
/* 0.0 = pinhole camera (everything in focus).                        */
/* Typical bokeh: 0.3 – 2.0.  Range: 0 – 30.  Clamped at runtime.   */
# ifndef RT_DEFAULT_DEFOCUS
#  define RT_DEFAULT_DEFOCUS	0.0
# endif

/* Camera "up" vector — used to derive the camera basis.              */
/* Almost always (0,1,0).  Change for Dutch-angle effects.            */
# ifndef RT_DEFAULT_VUP_X
#  define RT_DEFAULT_VUP_X		0.0
# endif
# ifndef RT_DEFAULT_VUP_Y
#  define RT_DEFAULT_VUP_Y		1.0
# endif
# ifndef RT_DEFAULT_VUP_Z
#  define RT_DEFAULT_VUP_Z		0.0
# endif

/* ================================================================== */
/*  §3  OBJ MODEL DEFAULTS                                           */
/* ================================================================== */

/* Target bounding-sphere diameter after mesh normalisation.          */
/* Range: 0.1 – 1000.  Clamped at runtime.                           */
# ifndef RT_OBJ_TARGET_SIZE
#  define RT_OBJ_TARGET_SIZE	4.0f
# endif

/* Default camera distance from origin for OBJ-only renders.         */
/* Range: 0.1 – 10000.  Clamped at runtime.                          */
# ifndef RT_OBJ_CAM_DIST
#  define RT_OBJ_CAM_DIST		8.0
# endif

/* Default camera vertical FOV for OBJ-only renders (degrees).       */
/* Range: 1 – 179.  Clamped at runtime.                              */
# ifndef RT_OBJ_CAM_FOV
#  define RT_OBJ_CAM_FOV		40.0
# endif

/* Default camera Y offset for OBJ-only renders.                     */
/* Positive looks slightly down at the model.                         */
# ifndef RT_OBJ_CAM_HEIGHT
#  define RT_OBJ_CAM_HEIGHT		2.0
# endif

/* Default OBJ material colour (RGB 0–1).                             */
/* Overridden if a .mtl file or JSON material is present.             */
# ifndef RT_OBJ_COLOR_R
#  define RT_OBJ_COLOR_R		0.85
# endif
# ifndef RT_OBJ_COLOR_G
#  define RT_OBJ_COLOR_G		0.15
# endif
# ifndef RT_OBJ_COLOR_B
#  define RT_OBJ_COLOR_B		0.15
# endif

/* Enable smooth (per-vertex) normal interpolation for OBJ meshes.   */
/* 1 = smooth shading (if normals present), 0 = flat shading.        */
# ifndef RT_OBJ_SMOOTH_NORMALS
#  define RT_OBJ_SMOOTH_NORMALS	1
# endif

/* Default OBJ material type.                                         */
/* 0=lambertian, 1=metal, 2=glossy, 3=dielectric (glass),            */
/* 4=tinted_glass, 5=diffuse_light.   Fuzz/roughness below.          */
# ifndef RT_OBJ_MAT_TYPE
#  define RT_OBJ_MAT_TYPE		3
# endif

/* Metal fuzz for OBJ default material (only if MAT_TYPE=1).         */
/* Range: 0.0 – 1.0.  Clamped at runtime.                            */
# ifndef RT_OBJ_MAT_FUZZ
#  define RT_OBJ_MAT_FUZZ		0.05
# endif

/* Glossy roughness for OBJ default material (only if MAT_TYPE=2).   */
/* Range: 0.0 – 1.0.  Clamped at runtime.                            */
# ifndef RT_OBJ_MAT_ROUGHNESS
#  define RT_OBJ_MAT_ROUGHNESS	0.1
# endif

/* Refraction index for dielectric/tinted_glass (MAT_TYPE=3 or 4).   */
/* Range: 1.0 – 3.0.  Glass ≈ 1.5, water ≈ 1.33, diamond ≈ 2.42.    */
# ifndef RT_OBJ_MAT_REFRACTION
#  define RT_OBJ_MAT_REFRACTION	1.5
# endif

/* Tint colour for tinted_glass material (MAT_TYPE=4) — RGB 0–1.     */
# ifndef RT_OBJ_MAT_TINT_R
#  define RT_OBJ_MAT_TINT_R		0.8
# endif
# ifndef RT_OBJ_MAT_TINT_G
#  define RT_OBJ_MAT_TINT_G		0.2
# endif
# ifndef RT_OBJ_MAT_TINT_B
#  define RT_OBJ_MAT_TINT_B		0.2
# endif

/* Emission intensity for diffuse_light material (MAT_TYPE=5).       */
/* Range: 0.1 – 1000.  Clamped at runtime.                           */
# ifndef RT_OBJ_MAT_LIGHT_INTENSITY
#  define RT_OBJ_MAT_LIGHT_INTENSITY	15.0
# endif

/* ---- OBJ texture override ---------------------------------------- */

/* Texture type applied to the OBJ material before creation.         */
/* 0 = solid colour (default), 1 = checker pattern.                  */
# ifndef RT_OBJ_TEXTURE_TYPE
#  define RT_OBJ_TEXTURE_TYPE	0
# endif

/* Inverse scale of the checker pattern (higher = finer checks).     */
/* Only used when RT_OBJ_TEXTURE_TYPE == 1.                          */
# ifndef RT_OBJ_CHECKER_SCALE
#  define RT_OBJ_CHECKER_SCALE	0.5
# endif

/* ================================================================== */
/*  §4  LIGHTING                                                      */
/* ================================================================== */

/* Brightness multiplier applied to all point lights when converted   */
/* to emissive spheres.  Range: 0.1 – 10000.  Clamped at runtime.    */
/* DEFAULT: 15.0 — calibrated for linear falloff (RT_LIGHT_FALLOFF=0) */
/*          with typical .rt scene distances (10–30 units).           */
/*   Example: L at distance 15, brightness 0.8 → emission/dist ≈ 0.7 */
/*   which is a natural lit surface.  Use 20+ for harsher sun.        */
# ifndef RT_LIGHT_SCALE
#  define RT_LIGHT_SCALE		15.0
# endif

/* Radius of emissive spheres representing point lights.              */
/* Larger = softer shadows, smaller = sharper.                        */
/* Range: 0.01 – 100.  Clamped at runtime.                           */
/* DEFAULT: 0.5 — gives clean, sharp-ish shadows without being a     */
/*          perfect delta light. Use 1.5+ for area-light softness.    */
# ifndef RT_LIGHT_RADIUS
#  define RT_LIGHT_RADIUS		0.5
# endif

/* Default ambient light ratio when none is specified.                */
/* Range: 0.0 – 1.0.  Clamped at runtime.                            */
# ifndef RT_DEFAULT_AMBIENT
#  define RT_DEFAULT_AMBIENT	0.1
# endif

/* ---- Shadow sampling --------------------------------------------- */

/* Number of shadow-ray samples per light.  More = softer, smoother   */
/* penumbra at the cost of speed.                                     */
/*   1  = hard shadows (single ray to light centre)                   */
/*   4  = slightly soft (fast, good for previews)                     */
/*   8  = visibly soft penumbra                                       */
/*  16  = high-quality soft shadows                                   */
/*  64  = reference quality                                           */
/* Range: 1 – 64.  Clamped at runtime.                                */
/* DEFAULT: 8 — good quality shadows with reasonable cost.           */
# ifndef RT_SHADOW_SAMPLES
#  define RT_SHADOW_SAMPLES		8
# endif

/* Number of DETERMINISTIC stratified shadow rays per light cast by   */
/* the default direct engine to compute a soft-shadow visibility      */
/* fraction across the light's spherical surface (g_lights[].radius). */
/*   1  = hard shadows (single ray to light centre, reproduces old)   */
/*   8  = visibly soft penumbra (default)                             */
/* A small fixed offset pattern is used (no RNG) so renders stay      */
/* bit-for-bit reproducible.  Range: 1 - 16.                          */
# ifndef RT_SOFT_SHADOW_SAMPLES
#  define RT_SOFT_SHADOW_SAMPLES	8
# endif

/* ---- Ambient occlusion (direct engine only) ---------------------- */

/* Number of FIXED (deterministic) hemisphere rays cast at a matte hit */
/* to estimate how exposed the point is.  The unoccluded fraction      */
/* multiplies ONLY the ambient/fill term, darkening creases and        */
/* contact points.  Does NOT touch the direct-light (shadow) term, and */
/* has no effect on the cinematic path tracer.                         */
/*   0  = off (no AO)                                                  */
/*   6  = subtle contact darkening (default)                          */
/*  16  = stronger, smoother occlusion                                */
/* A precomputed cosine-ish pattern is used (no RNG) so renders stay   */
/* bit-for-bit reproducible.  Range: 0 - 16.                           */
# ifndef RT_AO_SAMPLES
#  define RT_AO_SAMPLES			6
# endif

/* World-space length of the AO probe rays.  Only occluders within     */
/* this distance darken a point, keeping AO a local contact effect      */
/* (and bounded for speed).  Larger = broader, softer darkening.       */
/* Range: 0.01 - 100.  Tune to typical scene scale (units between      */
/* touching objects).  DEFAULT: 1.5.                                   */
# ifndef RT_AO_RADIUS
#  define RT_AO_RADIUS			1.5
# endif

/* Light distance attenuation model.                                  */
/*  0 = artistic / linear  (NdotL / distance) — smoother falloff      */
/*  1 = physically correct (NdotL / distance²) — realistic inverse    */
/*      square law; may need very high RT_LIGHT_SCALE to compensate.  */
/* DEFAULT: 0 — linear falloff matches .rt scene conventions where    */
/*          light distances are typically 10–30 units.  With inverse-  */
/*          square, these distances make lights far too dim.           */
# ifndef RT_LIGHT_FALLOFF
#  define RT_LIGHT_FALLOFF		0
# endif

/* ================================================================== */
/*  §5  GEOMETRY                                                      */
/* ================================================================== */

/* Half-size of the large quad used to approximate infinite planes.   */
/* Range: 100 – 1000000.  Clamped at runtime.                        */
# ifndef RT_PLANE_SIZE
#  define RT_PLANE_SIZE			200.0
# endif

/* ================================================================== */
/*  §6  COLOUR PROCESSING                                             */
/* ================================================================== */

/* ---- Exposure & tone mapping -------------------------------------- */

/* Exposure multiplier applied in HDR linear space before tone map.   */
/* 1.0 = neutral.  Lower = darker, higher = brighter.                 */
/* Range: 0.1 – 10.0.  Clamped at runtime.                           */
# ifndef RT_EXPOSURE
#  define RT_EXPOSURE			1.0
# endif

/* Tone-mapping operator applied after exposure, before gamma.        */
/*  0 = none  (clamp-only, legacy)                                    */
/*  1 = ACES filmic  (Narkowicz 2015 approximation)                   */
/*                                                                    */
/* ACES compresses highlights gracefully while preserving shadow       */
/* detail — the standard for photo-realistic CG rendering.            */
/* Range: 0 or 1.                                                     */
# ifndef RT_TONE_MAP
#  define RT_TONE_MAP			1
# endif

/* ---- Gamma -------------------------------------------------------- */
/*                                                                    */
/*  RT_COLOR_CIE     (0) – CIE gamma 2.2 power law.  Default.        */
/*                         Best contrast and deepest blacks.          */
/*                         Physically accurate for most displays.     */
/*                                                                    */
/*  RT_COLOR_SRGB    (1) – IEC 61966-2-1 piecewise sRGB curve.       */
/*                         Official standard for web imagery.         */
/*                         Brighter shadows (linear toe segment).     */
/*                                                                    */
/*  RT_COLOR_GAMMA20 (2) – Simple sqrt (gamma 2.0).                  */
/*                         Matches the book implementation exactly.   */
/*                                                                    */
/*  Override: compile with  -DRT_COLOR_PROFILE=RT_COLOR_SRGB  etc.   */
/*  The constants are defined in settings.h.                          */
# ifndef RT_COLOR_PROFILE
#  define RT_COLOR_PROFILE	RT_COLOR_CIE
# endif

/* S-curve contrast applied after gamma correction.                   */
/* 1.0 = neutral (no change), >1 = more punch.  Range: 0.5 – 3.0.    */
/* DEFAULT: 1.0 — neutral. The original 1.3 was making bright areas   */
/*          clip to white too easily. Increase for punchy art renders.*/
# ifndef RT_CONTRAST
#  define RT_CONTRAST			1.0
# endif

/* Saturation multiplier applied after contrast.                      */
/* 1.0 = neutral (no change), >1 = more vivid.  Range: 0.0 – 4.0.    */
/* DEFAULT: 1.0 — neutral. The original 1.5 was pushing whites to    */
/*          pure white and burning colour detail. Use 1.1-1.2 for     */
/*          gentle life, keep at 1.0 for photorealism.                */
# ifndef RT_SATURATION
#  define RT_SATURATION			1.0
# endif

/* Enable direct light sampling (shadow rays) for crisp shadows.      */
/* 1 = enabled (default), 0 = pure path tracing (legacy behaviour).   */
# ifndef RT_DIRECT_LIGHT_ENABLED
#  define RT_DIRECT_LIGHT_ENABLED	1
# endif

/* ================================================================== */
/*  §7  QUALITY PRESETS                                               */
/*                                                                    */
/*  Uncomment ONE block below — or compile with -DRT_PRESET=N — to   */
/*  override multiple settings at once.  Individual #define's above   */
/*  take priority over presets (they are #ifndef-guarded).            */
/*                                                                    */
/*  Preset    Width  SPP   Depth Shadows  Tile  RR  Exposure ToneMap  */
/*  -------   -----  ----  ----- -------  ----  --  -------- -------  */
/*  PREVIEW     400    16     10       1    16   0     1.0    ACES    */
/*  DRAFT       800    64     25       4    32   3     1.0    ACES    */
/*  STANDARD   1200   800     50       8    32   5     1.0    ACES    */
/*  HIGH       1920  2048     80      16    32   5     1.0    ACES    */
/*  ULTRA      3840  4096    100      16    64   5     1.0    ACES    */
/*                                                                    */
/*  Example usage:                                                    */
/*    make CFLAGS_EXTRA="-DRT_PRESET=4K"                              */
/*                                                                    */
/*  Or simply #define RT_PRESET before including this header.         */
/* ================================================================== */

/* Uncomment the desired preset: */
/* # define RT_PRESET_PREVIEW   */
/* # define RT_PRESET_DRAFT     */
/* # define RT_PRESET_STANDARD  */
/* # define RT_PRESET_HIGH      */
/* # define RT_PRESET_ULTRA     */

/* ================================================================== */
/*  §8  RUNTIME CLAMP HELPERS                                         */
/*                                                                    */
/*  Use rt_clamp_*() in setup code to silently cap user values to     */
/*  safe ranges.  Returns the clamped value.                          */
/* ================================================================== */

# include <math.h>

static inline double	rt_clampd(double v, double lo, double hi)
{
	if (v < lo)
		return (lo);
	if (v > hi)
		return (hi);
	return (v);
}

static inline int	rt_clampi(int v, int lo, int hi)
{
	if (v < lo)
		return (lo);
	if (v > hi)
		return (hi);
	return (v);
}

#endif /* STUDIO_CONFIG_H */
