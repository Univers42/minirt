/* ============================================================================ */
/*                                                                              */
/*                                 FILE HEADER                                  */
/* ---------------------------------------------------------------------------- */
/*  File:       wavefront.c                                                     */
/*  Author:     dlesieur                                                        */
/*  Email:      dlesieur@student.42.fr                                          */
/*  Created:    2025/12/30 19:58:32                                             */
/*  Updated:    2025/12/30 19:58:32                                             */
/*                                                                              */
/* ============================================================================ */

#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "SDL.h"
#include "SDL_opengl.h"
#include <float.h>
#include <math.h>

#define new(a, n, t) (t *)alloc(a, n, sizeof(t), _Alignof(t))
#define S(s) (Str){s, sizeof(s) - 1}

typedef struct
{
    char *beg;
    char *end;
    jmp_buf *oom;
} Arena;

static void *alloc(Arena *a, ptrdiff_t count, ptrdiff_t size, ptrdiff_t align)
{
    ptrdiff_t pad = (uintptr_t)a->end & (align - 1);
    if (count >= (a->end - a->beg - pad) / size)
    {
        longjmp(*a->oom, 1);
    }
    return memset(a->end -= pad + count * size, 0, count * size);
}

typedef struct
{
    char *data;
    ptrdiff_t len;
} Str;

static Str span(char *beg, char *end)
{
    Str r = {0};
    r.data = beg;
    r.len = beg ? end - beg : 0;
    return r;
}

static _Bool equals(Str a, Str b)
{
    return a.len == b.len && (!a.len || !memcmp(a.data, b.data, a.len));
}

static Str trimleft(Str s)
{
    for (; s.len && *s.data <= ' '; s.data++, s.len--)
    {
    }
    return s;
}

static Str trimright(Str s)
{
    for (; s.len && s.data[s.len - 1] <= ' '; s.len--)
    {
    }
    return s;
}

static Str substring(Str s, ptrdiff_t i)
{
    if (i)
    {
        s.data += i;
        s.len -= i;
    }
    return s;
}

typedef struct
{
    Str head;
    Str tail;
    _Bool ok;
} Cut;

static Cut cut(Str s, char c)
{
    Cut r = {0};
    if (!s.len)
        return r;
    char *beg = s.data;
    char *end = s.data + s.len;
    char *cut = beg;
    for (; cut < end && *cut != c; cut++)
    {
    }
    r.ok = cut < end;
    r.head = span(beg, cut);
    r.tail = span(cut + r.ok, end);
    return r;
}

static int32_t parseint(Str s)
{
    uint32_t r = 0;
    int32_t sign = 1;
    for (ptrdiff_t i = 0; i < s.len; i++)
    {
        switch (s.data[i])
        {
        case '+':
            break;
        case '-':
            sign = -1;
            break;
        default:
            r = 10 * r + s.data[i] - '0';
        }
    }
    return r * sign;
}

static float expt10(int32_t e)
{
    float y = 1.0f;
    float x = e < 0 ? 0.1f : e > 0 ? 10.0f
                                   : 1.0f;
    int32_t n = e < 0 ? e : -e;
    for (; n < -1; n /= 2)
    {
        y *= n % 2 ? x : 1.0f;
        x *= x;
    }
    return x * y;
}

static float parsefloat(Str s)
{
    float r = 0.0f;
    float sign = 1.0f;
    float exp = 0.0f;
    for (ptrdiff_t i = 0; i < s.len; i++)
    {
        switch (s.data[i])
        {
        case '+':
            break;
        case '-':
            sign = -1;
            break;
        case '.':
            exp = 1;
            break;
        case 'E':
        case 'e':
            exp = exp ? exp : 1.0f;
            exp *= expt10(parseint(substring(s, i + 1)));
            i = s.len;
            break;
        default:
            r = 10.0f * r + (s.data[i] - '0');
            exp *= 0.1f;
        }
    }
    return sign * r * (exp ? exp : 1.0f);
}

typedef struct
{
    float v[3];
} Vert;

static Vert parsevert(Str s)
{
    Vert r = {0};
    Cut c = cut(trimleft(s), ' ');
    r.v[0] = parsefloat(c.head);
    c = cut(trimleft(c.tail), ' ');
    r.v[1] = parsefloat(c.head);
    c = cut(trimleft(c.tail), ' ');
    r.v[2] = parsefloat(c.head);
    return r;
}

typedef struct
{
    int32_t v[3];
    int32_t n[3];
} Face;

static Face parseface(Str s, ptrdiff_t nverts, ptrdiff_t nnorms)
{
    Face r = {0};
    Cut fields = {0};
    fields.tail = s;
    for (int i = 0; i < 3; i++)
    {
        fields = cut(trimleft(fields.tail), ' ');
        Cut elem = cut(fields.head, '/');
        r.v[i] = parseint(elem.head);
        elem = cut(elem.tail, '/');
        elem = cut(elem.tail, '/');
        r.n[i] = parseint(elem.head);

        if (r.v[i] < 0)
        {
            r.v[i] = (int32_t)(r.v[i] + 1 + nverts);
        }
        if (r.n[i] < 0)
        {
            r.n[i] = (int32_t)(r.n[i] + 1 + nnorms);
        }
    }
    return r;
}

typedef struct
{
    Vert *verts;
    ptrdiff_t nverts;
    Vert *norms;
    ptrdiff_t nnorms;
    Face *faces;
    ptrdiff_t nfaces;
} Model;

static Model parseobj(Arena *a, Str obj)
{
    Model m = {0};
    Cut lines = {0};

    lines.tail = obj;
    while (lines.tail.len)
    {
        lines = cut(lines.tail, '\n');
        Cut fields = cut(trimright(lines.head), ' ');
        Str kind = fields.head;
        if (equals(S("v"), kind))
        {
            m.nverts++;
        }
        else if (equals(S("vn"), kind))
        {
            m.nnorms++;
        }
        else if (equals(S("f"), kind))
        {
            m.nfaces++;
        }
    }

    m.verts = new(a, m.nverts, Vert);
    m.norms = new(a, m.nnorms, Vert);
    m.faces = new(a, m.nfaces, Face);
    m.nverts = m.nnorms = m.nfaces = 0;

    lines.tail = obj;
    while (lines.tail.len)
    {
        lines = cut(lines.tail, '\n');
        Cut fields = cut(trimright(lines.head), ' ');
        Str kind = fields.head;
        if (equals(S("v"), kind))
        {
            m.verts[m.nverts++] = parsevert(fields.tail);
        }
        else if (equals(S("vn"), kind))
        {
            m.norms[m.nnorms++] = parsevert(fields.tail);
        }
        else if (equals(S("f"), kind))
        {
            m.faces[m.nfaces++] = parseface(fields.tail, m.nverts, m.nnorms);
        }
    }
    return m;
}

typedef struct
{
    Vert n, v;
} N3FV3F[3];

typedef struct
{
    N3FV3F *data;
    ptrdiff_t len;
} N3FV3Fs;

static N3FV3Fs n3fv3fize(Arena *a, Model m)
{
    N3FV3Fs r = {0};
    r.data = new(a, m.nfaces, N3FV3F);
    for (ptrdiff_t f = 0; f < m.nfaces; f++)
    {
        _Bool valid = 1;
        for (int i = 0; i < 3; i++)
        {
            valid &= m.faces[f].v[i] > 0 && m.faces[f].v[i] <= m.nverts;
            valid &= m.faces[f].n[i] > 0 && m.faces[f].n[i] <= m.nnorms;
        }

        if (valid)
        {
            ptrdiff_t t = r.len++;
            for (int i = 0; i < 3; i++)
            {
                r.data[t][i].n = m.norms[m.faces[f].n[i] - 1];
                r.data[t][i].v = m.verts[m.faces[f].v[i] - 1];
            }
        }
    }
    return r;
}

static Str loadfile(Arena *a, SDL_RWops *f)
{
    Str r = {0};
    r.data = a->beg;
    r.len = a->end - a->beg;
    r.len = SDL_RWread(f, r.data, 1, r.len);
    a->beg += r.len;
    return r;
}

static void perspective(double fovy, double aspect, double znear, double zfar)
{
    double h = znear * SDL_tan(fovy / 360 * 3.141592653589793);
    double w = h * aspect;
    glFrustum(-w, +w, -h, +h, znear, zfar);
}

static void xDwmFlush(void) {}

// New: helper to compute radius where light intensity falls below threshold
static float light_radius_for_threshold(float cAtt, float lAtt, float qAtt, float threshold)
{
    if (threshold <= 0.0f)
        return 0.0f;
    float inv = 1.0f / threshold;
    if (qAtt == 0.0f)
    {
        if (lAtt == 0.0f)
            return 1e6f;
        float d = (inv - cAtt) / lAtt;
        return d > 0.0f ? d : 0.0f;
    }
    float a = qAtt;
    float b = lAtt;
    float c0 = cAtt - inv;
    float disc = b * b - 4.0f * a * c0;
    if (disc <= 0.0f)
        return 0.0f;
    float r = (-b + sqrtf(disc)) / (2.0f * a);
    return r > 0.0f ? r : 0.0f;
}

// New: draw a 3D sphere with proper normals for lighting
static void draw_3d_sphere(float radius, int slices, int stacks)
{
    for (int i = 0; i < stacks; i++)
    {
        float lat0 = 3.141592653589793f * (-0.5f + (float)i / stacks);
        float lat1 = 3.141592653589793f * (-0.5f + (float)(i + 1) / stacks);
        float z0 = sinf(lat0);
        float z1 = sinf(lat1);
        float r0 = cosf(lat0);
        float r1 = cosf(lat1);

        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= slices; j++)
        {
            float lng = 2.0f * 3.141592653589793f * (float)j / slices;
            float x = cosf(lng);
            float y = sinf(lng);

            float nx0 = x * r0;
            float ny0 = y * r0;
            float nz0 = z0;
            glNormal3f(nx0, ny0, nz0);
            glVertex3f(radius * nx0, radius * ny0, radius * nz0);

            float nx1 = x * r1;
            float ny1 = y * r1;
            float nz1 = z1;
            glNormal3f(nx1, ny1, nz1);
            glVertex3f(radius * nx1, radius * ny1, radius * nz1);
        }
        glEnd();
    }
}

// Updated: build shadow projection matrix properly
static void build_shadow_matrix(float shadowMat[16], float lx, float ly, float lz,
                                float px, float py, float pz, float groundY)
{
    float dx = lx - px;
    float dy = ly - py;
    float dz = lz - pz;

    if (fabsf(dy) < 1e-6f)
        dy = (dy < 0) ? -1e-6f : 1e-6f;

    float dot = dy * (groundY - py);

    shadowMat[0] = dot - dy * dx;
    shadowMat[1] = -dx * dy;
    shadowMat[2] = -dx * dz;
    shadowMat[3] = -dx * (groundY - py);

    shadowMat[4] = -dy * dx;
    shadowMat[5] = dot - dy * dy;
    shadowMat[6] = -dy * dz;
    shadowMat[7] = -dy * (groundY - py);

    shadowMat[8] = -dz * dx;
    shadowMat[9] = -dz * dy;
    shadowMat[10] = dot - dy * dz;
    shadowMat[11] = -dz * (groundY - py);

    shadowMat[12] = -dx * (groundY - py);
    shadowMat[13] = -dy * (groundY - py);
    shadowMat[14] = -dz * (groundY - py);
    shadowMat[15] = dot - dy * (groundY - py);

    float scale = 1.0f / dot;
    for (int i = 0; i < 16; i++)
        shadowMat[i] *= scale;
}

int main(int argc, char **argv)
{
    jmp_buf oom;
    if (setjmp(oom))
    {
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR,
            "Out of Memory",
            "Not enough memory to display this model.",
            0);
        return 1;
    }

    ptrdiff_t cap = (ptrdiff_t)1 << 30;
    char *mem = SDL_malloc(cap);
    Arena perm = {mem, mem + cap, &oom};

    char *objpath = argc > 1 ? argv[1] : "/dev/stdin";
    SDL_RWops *objfile = SDL_RWFromFile(objpath, "rb");
    if (!objfile)
    {
        SDL_Log("SDL_RWFromFile(): %s", SDL_GetError());
        return 1;
    }
    Str obj = loadfile(&perm, objfile);
    SDL_RWclose(objfile);

    Model m = parseobj(&perm, obj);
    SDL_Log("%d verts, %d norms, %d faces",
            (int)m.nverts, (int)m.nnorms, (int)m.nfaces);

    if (SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init(): %s", SDL_GetError());
        return 1;
    }

    int width = 2000;
    int height = 1300;
    SDL_Window *w = SDL_CreateWindow(
        "OBJ Render - Realistic Dynamic Lighting",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!w)
    {
        SDL_Log("SDL_CreateWindow(): %s", SDL_GetError());
        return 1;
    }

    SDL_GLContext *gl = SDL_GL_CreateContext(w);
    if (!gl)
    {
        SDL_Log("SDL_GL_CreateContext(): %s", SDL_GetError());
        return 1;
    }
    SDL_GL_SetSwapInterval(1);

    void (*DwmFlush)(void) = 0;
    void *dwmapi = SDL_LoadObject("dwmapi.dll");
    if (dwmapi)
    {
        DwmFlush = SDL_LoadFunction(dwmapi, "DwmFlush");
    }
    DwmFlush = DwmFlush ? DwmFlush : xDwmFlush;

    N3FV3Fs triangles = n3fv3fize(&perm, m);

    // Calculate model bounds
    float mid[3] = {0};
    float min[3] = {+FLT_MAX, +FLT_MAX, +FLT_MAX};
    float max[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
    float scale = FLT_MAX;

    for (ptrdiff_t t = 0; t < triangles.len; t++)
    {
        for (int v = 0; v < 3; v++)
        {
            for (int i = 0; i < 3; i++)
            {
                float r = triangles.data[t][v].v.v[i];
                min[i] = min[i] < r ? min[i] : r;
                max[i] = max[i] > r ? max[i] : r;
            }
        }
    }
    for (int i = 0; i < 3; i++)
    {
        float size = max[i] - min[i];
        mid[i] = min[i] + size / 2;
        scale = scale < size ? scale : size;
    }

    SDL_Log("%d triangles, center [%.2f %.2f %.2f], scale %.3f",
            (int)triangles.len, mid[0], mid[1], mid[2], scale);

    // Setup realistic lighting with proper material
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);

    // Neutral brown/gray material (more natural / desaturated)
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (GLfloat[]){0.30f, 0.28f, 0.25f, 1.0f});
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (GLfloat[]){0.60f, 0.55f, 0.50f, 1.0f});
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (GLfloat[]){0.20f, 0.20f, 0.20f, 1.0f});
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    glMatrixMode(GL_PROJECTION);
    perspective(35, (double)width / height, 0.1, 20);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_MODELVIEW);

    float zoom = 1.0f;
    const float ZOOM_STEP = 1.1f;
    const float ZOOM_MIN = 0.1f;
    const float ZOOM_MAX = 10.0f;

    SDL_Log("\nControls:");
    SDL_Log("  Mouse Wheel / +/- : Zoom");
    SDL_Log("  Q : Quit\n");

    for (;;)
    {
        for (SDL_Event e; SDL_PollEvent(&e);)
        {
            switch (e.type)
            {
            case SDL_QUIT:
                return 0;
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym)
                {
                case 'q':
                    return 0;
                case SDLK_MINUS:
                case SDLK_KP_MINUS:
                    zoom /= ZOOM_STEP;
                    if (zoom < ZOOM_MIN)
                        zoom = ZOOM_MIN;
                    break;
                case SDLK_EQUALS:
                case SDLK_PLUS:
                case SDLK_KP_PLUS:
                    zoom *= ZOOM_STEP;
                    if (zoom > ZOOM_MAX)
                        zoom = ZOOM_MAX;
                    break;
                }
                break;
            case SDL_MOUSEWHEEL:
                if (e.wheel.y > 0)
                    zoom *= ZOOM_STEP;
                else if (e.wheel.y < 0)
                    zoom /= ZOOM_STEP;
                if (zoom < ZOOM_MIN)
                    zoom = ZOOM_MIN;
                if (zoom > ZOOM_MAX)
                    zoom = ZOOM_MAX;
                break;
            case SDL_WINDOWEVENT:
                if (e.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    width = e.window.data1;
                    height = e.window.data2;
                    if (height <= 0)
                        height = 1;
                    glViewport(0, 0, width, height);
                    glMatrixMode(GL_PROJECTION);
                    glLoadIdentity();
                    perspective(35, (double)width / height, 0.1, 20);
                    glMatrixMode(GL_MODELVIEW);
                }
                break;
            }
        }

        // neutral / light background to make shadow contrast clear
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, -4.5f * zoom);

        float tsec = (float)SDL_GetTicks64() / 1000.0f;
        float orbit = scale * 2.2f;

        // Three physically-positioned lights orbiting the model
        float lx0 = mid[0] + cosf(tsec * 0.5f) * orbit;
        float ly0 = mid[1] + scale * 0.8f + sinf(tsec * 0.3f) * scale * 0.4f;
        float lz0 = mid[2] + sinf(tsec * 0.5f) * orbit;

        float lx1 = mid[0] + cosf(tsec * 0.7f + 2.0f) * orbit * 0.8f;
        float ly1 = mid[1] + scale * 0.5f + cosf(tsec * 0.4f) * scale * 0.3f;
        float lz1 = mid[2] + sinf(tsec * 0.7f + 2.0f) * orbit * 0.8f;

        float lx2 = mid[0] + cosf(tsec * 0.6f + 4.0f) * orbit * 0.6f;
        float ly2 = mid[1] + scale * 1.2f + sinf(tsec * 0.5f) * scale * 0.5f;
        float lz2 = mid[2] + sinf(tsec * 0.6f + 4.0f) * orbit * 0.6f;

        // Apply model transformations
        glScalef(1.0f / scale, 1.0f / scale, 1.0f / scale);
        glTranslatef(-mid[0], -mid[1], -mid[2]);
        float rotate = (float)(SDL_GetTicks64() % (40 * 360)) / 40;
        glRotatef(rotate, 0.0f, 1.0f, 0.0f);

        // Set light positions (in model space after transformations)
        float light0pos[4] = {lx0, ly0, lz0, 1.0f};
        float light1pos[4] = {lx1, ly1, lz1, 1.0f};
        float light2pos[4] = {lx2, ly2, lz2, 1.0f};

        glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
        glLightfv(GL_LIGHT1, GL_POSITION, light1pos);
        glLightfv(GL_LIGHT2, GL_POSITION, light2pos);

        // Set lights as spotlights with direction towards model center
        float dir0[3] = {mid[0] - lx0, mid[1] - ly0, mid[2] - lz0};
        float dir1[3] = {mid[0] - lx1, mid[1] - ly1, mid[2] - lz1};
        float dir2[3] = {mid[0] - lx2, mid[1] - ly2, mid[2] - lz2};
        // Normalize directions
        float len0 = sqrtf(dir0[0] * dir0[0] + dir0[1] * dir0[1] + dir0[2] * dir0[2]);
        float len1 = sqrtf(dir1[0] * dir1[0] + dir1[1] * dir1[1] + dir1[2] * dir1[2]);
        float len2 = sqrtf(dir2[0] * dir2[0] + dir2[1] * dir2[1] + dir2[2] * dir2[2]);
        if (len0 > 0)
        {
            dir0[0] /= len0;
            dir0[1] /= len0;
            dir0[2] /= len0;
        }
        if (len1 > 0)
        {
            dir1[0] /= len1;
            dir1[1] /= len1;
            dir1[2] /= len1;
        }
        if (len2 > 0)
        {
            dir2[0] /= len2;
            dir2[1] /= len2;
            dir2[2] /= len2;
        }

        glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir0);
        glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 45.0f); // wide beam
        glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 2.0f);

        glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, dir1);
        glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 35.0f); // narrower
        glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 4.0f);

        glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, dir2);
        glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 30.0f); // narrowest
        glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 8.0f);

        // Light 0: Warm white/yellow - key light
        glLightfv(GL_LIGHT0, GL_AMBIENT, (GLfloat[]){0.05f, 0.04f, 0.02f, 1.0f});
        glLightfv(GL_LIGHT0, GL_DIFFUSE, (GLfloat[]){1.0f, 0.95f, 0.8f, 1.0f});
        glLightfv(GL_LIGHT0, GL_SPECULAR, (GLfloat[]){1.0f, 1.0f, 0.95f, 1.0f});
        glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
        glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.05f / scale);
        glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.01f / (scale * scale));

        // Light 1: Cool blue - fill light
        glLightfv(GL_LIGHT1, GL_AMBIENT, (GLfloat[]){0.02f, 0.03f, 0.05f, 1.0f});
        glLightfv(GL_LIGHT1, GL_DIFFUSE, (GLfloat[]){0.6f, 0.7f, 1.0f, 1.0f});
        glLightfv(GL_LIGHT1, GL_SPECULAR, (GLfloat[]){0.8f, 0.9f, 1.0f, 1.0f});
        glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
        glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.07f / scale);
        glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.015f / (scale * scale));

        // Light 2: Subtle green accent - rim light
        glLightfv(GL_LIGHT2, GL_AMBIENT, (GLfloat[]){0.01f, 0.02f, 0.01f, 1.0f});
        glLightfv(GL_LIGHT2, GL_DIFFUSE, (GLfloat[]){0.5f, 0.9f, 0.6f, 1.0f});
        glLightfv(GL_LIGHT2, GL_SPECULAR, (GLfloat[]){0.7f, 1.0f, 0.8f, 1.0f});
        glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1.0f);
        glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.08f / scale);
        glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.02f / (scale * scale));

        // Visualize light spread: draw low-alpha translucent sphere at radius where intensity ~ threshold
        const float VIS_THRESHOLD = 0.25f; // visualize radius where intensity drops below this
        float r0 = light_radius_for_threshold(1.0f, 0.05f / scale, 0.01f / (scale * scale), VIS_THRESHOLD);
        float r1 = light_radius_for_threshold(1.0f, 0.07f / scale, 0.015f / (scale * scale), VIS_THRESHOLD);
        float r2 = light_radius_for_threshold(1.0f, 0.08f / scale, 0.02f / (scale * scale), VIS_THRESHOLD);

        glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);
        // Light 0 translucent volume (warm)
        glColor4f(1.0f, 0.95f, 0.8f, 0.12f);
        glPushMatrix();
        glTranslatef(lx0, ly0, lz0);
        draw_3d_sphere(r0, 20, 10);
        glPopMatrix();
        // Light 1 translucent volume (cool)
        glColor4f(0.45f, 0.55f, 1.0f, 0.10f);
        glPushMatrix();
        glTranslatef(lx1, ly1, lz1);
        draw_3d_sphere(r1, 18, 9);
        glPopMatrix();
        // Light 2 translucent volume (green)
        glColor4f(0.4f, 0.9f, 0.6f, 0.09f);
        glPushMatrix();
        glTranslatef(lx2, ly2, lz2);
        draw_3d_sphere(r2, 16, 8);
        glPopMatrix();
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);
        glPopAttrib();

        // Render shadows from primary light onto ground plane
        float groundY = min[1] - scale * 0.02f;
        float shadowMat[16];
        // Project from model's base center for shadow to spread from origin and deform with angle
        build_shadow_matrix(shadowMat, lx0, ly0, lz0, mid[0], min[1], mid[2], groundY);

        glPushMatrix();
        glMultMatrixf(shadowMat);
        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // stronger black shadow for contrast on white background
        glColor4f(0.0f, 0.0f, 0.0f, 0.85f);
        glDepthMask(GL_FALSE);

        glInterleavedArrays(GL_N3F_V3F, 0, triangles.data);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(triangles.len * 3));

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);
        glPopMatrix();

        // Draw the main model with neutral color (material already set)
        glColor3f(0.85f, 0.85f, 0.85f);
        glInterleavedArrays(GL_N3F_V3F, 0, triangles.data);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(triangles.len * 3));

        // Visualize light directions: draw lines from lights to model center
        glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
        glDisable(GL_LIGHTING);
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        // Light 0 direction (warm)
        glColor3f(1.0f, 0.9f, 0.6f);
        glVertex3f(lx0, ly0, lz0);
        glVertex3f(mid[0], mid[1], mid[2]);
        // Light 1 direction (cool)
        glColor3f(0.4f, 0.5f, 1.0f);
        glVertex3f(lx1, ly1, lz1);
        glVertex3f(mid[0], mid[1], mid[2]);
        // Light 2 direction (green)
        glColor3f(0.3f, 0.9f, 0.4f);
        glVertex3f(lx2, ly2, lz2);
        glVertex3f(mid[0], mid[1], mid[2]);
        glEnd();
        glPopAttrib();

        // Draw light sources as emissive 3D spheres
        float light_radius = scale * 0.06f;

        // Light 0 - Warm yellow sphere
        glPushMatrix();
        glTranslatef(lx0, ly0, lz0);
        glMaterialfv(GL_FRONT, GL_EMISSION, (GLfloat[]){1.0f, 0.95f, 0.7f, 1.0f});
        glMaterialfv(GL_FRONT, GL_AMBIENT, (GLfloat[]){0.2f, 0.19f, 0.14f, 1.0f});
        glMaterialfv(GL_FRONT, GL_DIFFUSE, (GLfloat[]){1.0f, 0.95f, 0.7f, 1.0f});
        glColor3f(1.0f, 0.95f, 0.7f);
        draw_3d_sphere(light_radius, 24, 18);
        glMaterialfv(GL_FRONT, GL_EMISSION, (GLfloat[]){0, 0, 0, 1});
        glPopMatrix();

        // Light 1 - Cool blue sphere
        glPushMatrix();
        glTranslatef(lx1, ly1, lz1);
        glMaterialfv(GL_FRONT, GL_EMISSION, (GLfloat[]){0.5f, 0.6f, 1.0f, 1.0f});
        glMaterialfv(GL_FRONT, GL_AMBIENT, (GLfloat[]){0.1f, 0.12f, 0.2f, 1.0f});
        glMaterialfv(GL_FRONT, GL_DIFFUSE, (GLfloat[]){0.5f, 0.6f, 1.0f, 1.0f});
        glColor3f(0.5f, 0.6f, 1.0f);
        draw_3d_sphere(light_radius * 0.85f, 24, 18);
        glMaterialfv(GL_FRONT, GL_EMISSION, (GLfloat[]){0, 0, 0, 1});
        glPopMatrix();

        // Light 2 - Green sphere
        glPushMatrix();
        glTranslatef(lx2, ly2, lz2);
        glMaterialfv(GL_FRONT, GL_EMISSION, (GLfloat[]){0.4f, 0.9f, 0.5f, 1.0f});
        glMaterialfv(GL_FRONT, GL_AMBIENT, (GLfloat[]){0.08f, 0.18f, 0.1f, 1.0f});
        glMaterialfv(GL_FRONT, GL_DIFFUSE, (GLfloat[]){0.4f, 0.9f, 0.5f, 1.0f});
        glColor3f(0.4f, 0.9f, 0.5f);
        draw_3d_sphere(light_radius * 0.7f, 24, 18);
        glMaterialfv(GL_FRONT, GL_EMISSION, (GLfloat[]){0, 0, 0, 1});
        glPopMatrix();

        SDL_GL_SwapWindow(w);
        DwmFlush();
    }
}
