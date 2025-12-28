#include <stdio.h>
#include <stdlib.h>
#include "principle_brdf.h"
#include "glossy.h"
#include "lambertian.h"
#include "hitinfo.h"
#include "vec3.h"

int main(void)
{
	// Setup a dummy Disney material
	t_disney mat = {
		.baseColor = vec3_init(0.8, 0.2, 0.1),
		.metallic = 0.1,
		.roughness = 0.5,
		.specular = 0.5,
		.specularTint = 0.0,
		.sheen = 0.0,
		.sheenTint = 0.0,
		.clearcoat = 0.0,
		.clearcoatGloss = 0.0,
		.subsurface = 0.0
	};

	// Setup a dummy hitinfo
	t_hit_info hit;
	hit.normal = vec3_init(0, 1, 0);
	hit.point = (t_point3){0, 0, 0};
	hit.direction = vec3_init(0, -1, 0);
	hit.d = 1.0;

	t_vec3 wi = vec3_init(0, 1, 0);
	t_vec3 wo = vec3_init(0, 1, 0);

	// Principled BRDF
	t_principled_brdf pbrdf;
	principled_init(&pbrdf, NULL);

	t_vec3 eval = principled_eval(&mat, &hit, &wi, &wo);
	printf("Principled eval: (%f, %f, %f)\n", eval.x, eval.y, eval.z);

	// Lambertian BRDF
	t_lambertian lam;
	lambertian_init(&lam, 0.7, vec3_init(0.5, 0.5, 0.5), NULL);
	t_vec3 leval = lambertian_eval_impl(&lam, &hit, &wi, &wo);
	printf("Lambertian eval: (%f, %f, %f)\n", leval.x, leval.y, leval.z);

	// Glossy BRDF
	t_glossy gloss;
	glossy_init(&gloss, 0.9, 20.0, vec3_init(1, 1, 1), NULL);
	t_vec3 geval = glossy_eval_impl(&gloss, &hit, &wi, &wo);
	printf("Glossy eval: (%f, %f, %f)\n", geval.x, geval.y, geval.z);

	return 0;
}
