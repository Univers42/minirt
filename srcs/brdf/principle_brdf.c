#include "principle_brdf.h"
#include <stdlib.h>

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif

/* small helpers */
static inline double clampd(double v, double lo, double hi) { if (v < lo) return lo; if (v > hi) return hi; return v; }
static inline t_vec3 vec3_lerp_s(double t, t_vec3 a, t_vec3 b) { return vec3_add(vec3_scale(a, 1.0 - t), vec3_scale(b, t)); }
static inline t_vec3 vec3_mul_scalar_vec(double s, t_vec3 a){ return vec3_scale(a, s); }

/* Fresnel and distribution helpers (ported) */
static inline double fresnel_s(double cosT) {
	double v = clampd(1.0 - cosT, 0.0, 1.0);
	double v2 = v * v;
	return v2 * v2 * v;
}

static inline double GTR1(double cosT, double a) {
	if (a >= 1.0) return 1.0 / M_PI;
	double t = (1.0 + (a * a - 1.0) * cosT * cosT);
	return (a * a - 1.0) / (M_PI * log(a * a) * t);
}

static inline double GTR2(double cosT, double a) {
	double t = (1.0 + (a * a - 1.0) * cosT * cosT);
	return (a * a) / (M_PI * t * t);
}

static inline double GGX(double cosT, double a) {
	double a2 = a * a;
	double b = cosT * cosT;
	return 1.0 / (cosT + sqrt(a2 + b - a2 * b));
}

/* Expanded shared context to hold all intermediates and avoid local clutter.
   Not thread-safe. */
typedef struct s_pb_ctx
{
	/* geometry/frame */
	t_vec3 N, u, v, half;
	/* dots */
	double ndotwi, ndotwo, ndoth, widoth;
	/* material-related temporaries */
	t_vec3 Cdlin, Ctint, tmp, Cspec0, Fsheen;
	double Cdlum;
	/* diffuse/specular scalars */
	double Fd90, Fi, Fo, Fd, Fss90, Fss, ss;
	double a, Ds, Dc, roughg, Gs, Gc, Fh, Fc;
	t_vec3 Fs;
	/* final terms */
	t_vec3 diffuse_term, spec_term, clearcoat_term, sum;
	/* sampling */
	double r1, r2, r, phi, x, y, z, p, ratiodiffuse, rnd1, rnd2, cos_theta, sin_theta, wodoth;
	t_vec3 wi, half_sample;
}	t_pb_ctx;

static t_pb_ctx PB_CTX;

static inline void ctx_build_frame_from_normal(const t_vec3 *N)
{
	PB_CTX.N = *N;
	t_vec3 up = vec3_init(0.0034, 1.0, 0.0071);
	PB_CTX.v = vec3_cross(up, PB_CTX.N);
	PB_CTX.v = vec3_normalize(PB_CTX.v);
	PB_CTX.u = vec3_cross(PB_CTX.v, PB_CTX.N);
}

static inline void ctx_set_half_from_wi_wo(const t_vec3 *wi, const t_vec3 *wo)
{
	PB_CTX.half = vec3_normalize(vec3_add(*wi, *wo));
	PB_CTX.ndoth = fabs(vec3_dot(PB_CTX.half, PB_CTX.N));
	PB_CTX.widoth = vec3_dot(*wi, PB_CTX.half);
	PB_CTX.ndotwi = vec3_dot(PB_CTX.N, *wi);
	PB_CTX.ndotwo = vec3_dot(PB_CTX.N, *wo);
}

static inline void ctx_prepare_material(const t_disney *d)
{
	if (!d) {
		PB_CTX.Cdlin = vec3_init(0.0,0.0,0.0);
		PB_CTX.Cdlum = 0.0;
		PB_CTX.Ctint = vec3_init(1.0,1.0,1.0);
		PB_CTX.Cspec0 = vec3_init(0.0,0.0,0.0);
		return;
	}
	PB_CTX.Cdlin = d->baseColor;
	PB_CTX.Cdlum = 0.3 * PB_CTX.Cdlin.x + 0.6 * PB_CTX.Cdlin.y + 0.1 * PB_CTX.Cdlin.z;
	PB_CTX.Ctint = (PB_CTX.Cdlum > 0.0) ? vec3_scale(PB_CTX.Cdlin, 1.0 / PB_CTX.Cdlum) : vec3_init(1.0,1.0,1.0);
	PB_CTX.tmp = vec3_scale(vec3_init(1.0,1.0,1.0), d->specular * 0.08);
	PB_CTX.Cspec0 = vec3_lerp_s(d->metallic, PB_CTX.tmp, PB_CTX.Cdlin);
	PB_CTX.Fsheen = vec3_mul_scalar_vec(d->sheen, vec3_lerp_s(d->sheenTint, vec3_init(1.0,1.0,1.0), PB_CTX.Ctint));
}

/* init / lifecycle */
void principled_init(t_principled_brdf *p, const t_sampler *sampler)
{
	if (!p) return;
	brdf_init(&p->base, sampler);
	p->base.eval = principled_eval;
	p->base.pdf = principled_pdf;
	p->base.sample_f = principled_sample_f;
	p->base.color = NULL; /* optional */
}

t_principled_brdf *principled_new(const t_sampler *sampler)
{
	t_principled_brdf *p = (t_principled_brdf *)malloc(sizeof(*p));
	if (!p) return NULL;
	principled_init(p, sampler);
	return p;
}

void principled_free(t_principled_brdf *p)
{
	if (!p) return;
	free(p);
}

/* sample implementation: uses PB_CTX to avoid many local variables */
t_vec3 principled_sample_f(void *mat, const t_hit_info *hitinfo, const t_vec3 *wo)
{
	t_disney *d = (t_disney *)mat;
	PB_CTX.ratiodiffuse = 0.5;
	if (d) PB_CTX.ratiodiffuse = (1.0 - d->metallic) * 0.5;

	PB_CTX.p = (double)rand() / (double)RAND_MAX;

	ctx_build_frame_from_normal(&hitinfo->normal);

	if (PB_CTX.p < PB_CTX.ratiodiffuse || !d) {
		PB_CTX.r1 = (double)rand() / RAND_MAX;
		PB_CTX.r2 = (double)rand() / RAND_MAX;
		PB_CTX.r = sqrt(PB_CTX.r1);
		PB_CTX.phi = 2.0 * M_PI * PB_CTX.r2;
		PB_CTX.x = PB_CTX.r * cos(PB_CTX.phi);
		PB_CTX.y = PB_CTX.r * sin(PB_CTX.phi);
		PB_CTX.z = sqrt(fmax(0.0, 1.0 - PB_CTX.r * PB_CTX.r));
		PB_CTX.wi = vec3_add(vec3_add(vec3_scale(PB_CTX.u, PB_CTX.x), vec3_scale(PB_CTX.v, PB_CTX.y)), vec3_scale(PB_CTX.N, PB_CTX.z));
		return vec3_normalize(PB_CTX.wi);
	} else {
		PB_CTX.a = fmax(0.001, d->roughness);
		PB_CTX.rnd1 = (double)rand() / RAND_MAX;
		PB_CTX.rnd2 = (double)rand() / RAND_MAX;
		PB_CTX.cos_theta = pow(PB_CTX.rnd1, 1.0 / (PB_CTX.a * PB_CTX.a + 1.0));
		PB_CTX.sin_theta = sqrt(fmax(0.0, 1.0 - PB_CTX.cos_theta * PB_CTX.cos_theta));
		PB_CTX.phi = 2.0 * M_PI * PB_CTX.rnd2;
		PB_CTX.x = cos(PB_CTX.phi) * PB_CTX.sin_theta;
		PB_CTX.y = sin(PB_CTX.phi) * PB_CTX.sin_theta;
		PB_CTX.z = PB_CTX.cos_theta;
		PB_CTX.half_sample = vec3_add(vec3_add(vec3_scale(PB_CTX.u, PB_CTX.x), vec3_scale(PB_CTX.v, PB_CTX.y)), vec3_scale(PB_CTX.N, PB_CTX.z));
		PB_CTX.half_sample = vec3_normalize(PB_CTX.half_sample);
		PB_CTX.wodoth = vec3_dot(*wo, PB_CTX.half_sample);
		PB_CTX.wi = vec3_sub(vec3_scale(PB_CTX.half_sample, 2.0 * PB_CTX.wodoth), *wo);
		return vec3_normalize(PB_CTX.wi);
	}
}

/* pdf implementation: uses PB_CTX */
double principled_pdf(void *mat, const t_hit_info *hitinfo, const t_vec3 *wi, const t_vec3 *wo)
{
	t_disney *d = (t_disney *)mat;
	if (!hitinfo || !wi || !wo) return 0.0;
	double specularAlpha = d ? fmax(0.001, d->roughness) : 0.1;
	double clearcoatAlpha = d ? (d->clearcoatGloss * 0.1 + 0.001) : 0.1;
	double diffuseRatio = d ? 0.5 * (1.0 - d->metallic) : 0.5;
	double specularRatio = 1.0 - diffuseRatio;

	ctx_build_frame_from_normal(&hitinfo->normal);
	ctx_set_half_from_wi_wo(wi, wo);

	double pdfGTR2 = GTR2(PB_CTX.ndoth, specularAlpha) * PB_CTX.ndoth;
	double pdfGTR1 = GTR1(PB_CTX.ndoth, clearcoatAlpha) * PB_CTX.ndoth;

	double ratio = 1.0 / (1.0 + (d ? d->clearcoat : 0.0));
	double denom = 4.0 * fabs(vec3_dot(*wi, PB_CTX.half));
	double pdfSpec = (denom > 1e-12) ? ( ( (1.0 - ratio) * pdfGTR2 + ratio * pdfGTR1 ) / denom ) : 0.0;
	double pdfDiff = fabs(vec3_dot(PB_CTX.N, *wi)) * (1.0 / M_PI);

	return diffuseRatio * pdfDiff + specularRatio * pdfSpec;
}

/* eval implementation: uses PB_CTX to reuse intermediates */
t_vec3 principled_eval(void *mat, const t_hit_info *hitinfo, const t_vec3 *wi, const t_vec3 *wo)
{
	t_disney *d = (t_disney *)mat;
	if (!d || !hitinfo || !wi || !wo) return vec3_init(0.0,0.0,0.0);

	ctx_build_frame_from_normal(&hitinfo->normal);
	ctx_set_half_from_wi_wo(wi, wo);
	ctx_prepare_material(d);

	if (PB_CTX.ndotwi <= 0.0 || PB_CTX.ndotwo <= 0.0) return vec3_init(0.0,0.0,0.0);

	PB_CTX.Fd90 = 0.5 + 2.0 * PB_CTX.ndoth * PB_CTX.ndoth * d->roughness;
	PB_CTX.Fi = fresnel_s(PB_CTX.ndotwi);
	PB_CTX.Fo = fresnel_s(PB_CTX.ndotwo);
	PB_CTX.Fd = (1.0 + (PB_CTX.Fd90 - 1.0) * PB_CTX.Fi) * (1.0 + (PB_CTX.Fd90 - 1.0) * PB_CTX.Fo);

	PB_CTX.Fss90 = PB_CTX.widoth * PB_CTX.widoth * d->roughness;
	PB_CTX.Fss = (1.0 - PB_CTX.Fi) * (1.0 - PB_CTX.Fo) + PB_CTX.Fi * PB_CTX.Fss90;
	PB_CTX.ss = 1.25 * (PB_CTX.Fss * (1.0 / (PB_CTX.ndotwi + PB_CTX.ndotwo) - 0.5) + 0.5);

	PB_CTX.a = fmax(0.001, d->roughness);
	PB_CTX.Ds = GTR2(PB_CTX.ndoth, PB_CTX.a);
	PB_CTX.Dc = GTR1(PB_CTX.ndoth, clampd(d->clearcoatGloss, 0.001, 1.0));

	PB_CTX.roughg = sqrt(d->roughness * 0.5 + 0.5);
	PB_CTX.Gs = GGX(PB_CTX.ndotwo, PB_CTX.roughg) * GGX(PB_CTX.ndotwi, PB_CTX.roughg);
	PB_CTX.Gc = GGX(PB_CTX.ndotwo, 0.25) * GGX(PB_CTX.ndotwi, 0.25);

	PB_CTX.Fh = fresnel_s(PB_CTX.widoth);
	PB_CTX.Fs = vec3_lerp_s(PB_CTX.Fh, PB_CTX.Cspec0, vec3_init(1.0,1.0,1.0));
	PB_CTX.Fc = clampd(PB_CTX.Fh * 1.0, 0.04, 1.0);

	PB_CTX.Fsheen = vec3_mul_scalar_vec(d->sheen * PB_CTX.Fh, vec3_lerp_s(d->sheenTint, vec3_init(1.0,1.0,1.0), PB_CTX.Ctint));

	PB_CTX.diffuse_term = vec3_scale(PB_CTX.Cdlin, (1.0 / M_PI) * ((1.0 - d->metallic) * ( (1.0 - d->subsurface) * PB_CTX.Fd + d->subsurface * PB_CTX.ss )));
	PB_CTX.spec_term = vec3_scale(PB_CTX.Fs, PB_CTX.Gs * PB_CTX.Ds);
	PB_CTX.clearcoat_term = vec3_scale(vec3_init(1.0,1.0,1.0), 0.25 * d->clearcoat * PB_CTX.Gc * PB_CTX.Fc * PB_CTX.Dc);

	PB_CTX.sum = vec3_add(PB_CTX.diffuse_term, vec3_add(PB_CTX.Fsheen, vec3_add(PB_CTX.spec_term, PB_CTX.clearcoat_term)));
	return PB_CTX.sum;
}
