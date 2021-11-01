#include "Sampling.h"

void cosine_sample_hemisphere(float r1, float r2, vec3<f32>& p, float& pdf)
{
	// Uniformly sample disk.
	f32 r = sqrtf(r1);
	f32 phi = r2 * 2.0f * F32_PI;
	p.x = r * cosf(phi);
	p.y = r * sinf(phi);

	// Project up to hemisphere.
	p.z = sqrtf(fmaxf(0.0f, 1.0f - p.x * p.x - p.y * p.y)); //cosTheta NoL

	pdf = p.z * F32_1_FRAC_PI;
}
