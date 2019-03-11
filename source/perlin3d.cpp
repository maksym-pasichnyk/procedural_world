/*
Copyright 2019 Maxim Pasichnyk

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
 */

#include "perlin3d.h"

double perlin3d::noise(const glm::vec3 &point) {
	double x = point.x;
	double y = point.y;
	double z = point.z;

	// Find unit grid cell containing point
	int X = fastfloor(x);
	int Y = fastfloor(y);
	int Z = fastfloor(z);

	// Get relative xyz coordinates of point within that cell
	x = x - X;
	y = y - Y;
	z = z - Z;

	// Wrap the integer cells at 255 (smaller integer period can be introduced here)
	X = X & 255;
	Y = Y & 255;
	Z = Z & 255;

	// Calculate a set of eight hashed gradient indices
	int gi000 = perm[X + perm[Y + perm[Z]]] % 12;
	int gi001 = perm[X + perm[Y + perm[Z + 1]]] % 12;
	int gi010 = perm[X + perm[Y + 1 + perm[Z]]] % 12;
	int gi011 = perm[X + perm[Y + 1 + perm[Z + 1]]] % 12;
	int gi100 = perm[X + 1 + perm[Y + perm[Z]]] % 12;
	int gi101 = perm[X + 1 + perm[Y + perm[Z + 1]]] % 12;
	int gi110 = perm[X + 1 + perm[Y + 1 + perm[Z]]] % 12;
	int gi111 = perm[X + 1 + perm[Y + 1 + perm[Z + 1]]] % 12;

	// The gradients of each corner are now:
	// g000 = grad3[gi000];
	// g001 = grad3[gi001];
	// g010 = grad3[gi010];
	// g011 = grad3[gi011];
	// g100 = grad3[gi100];
	// g101 = grad3[gi101];
	// g110 = grad3[gi110];
	// g111 = grad3[gi111];
	// Calculate noise contributions from each of the eight corners
	double n000 = dot(grad3[gi000], x, y, z);
	double n100 = dot(grad3[gi100], x - 1, y, z);
	double n010 = dot(grad3[gi010], x, y - 1, z);
	double n110 = dot(grad3[gi110], x - 1, y - 1, z);
	double n001 = dot(grad3[gi001], x, y, z - 1);
	double n101 = dot(grad3[gi101], x - 1, y, z - 1);
	double n011 = dot(grad3[gi011], x, y - 1, z - 1);
	double n111 = dot(grad3[gi111], x - 1, y - 1, z - 1);
	// Compute the fade curve value for each of x, y, z
	double u = fade(x);
	double v = fade(y);
	double w = fade(z);
	// Interpolate along x the contributions from each of the corners
	double nx00 = mix(n000, n100, u);
	double nx01 = mix(n001, n101, u);
	double nx10 = mix(n010, n110, u);
	double nx11 = mix(n011, n111, u);
	// Interpolate the four results along y
	double nxy0 = mix(nx00, nx10, v);
	double nxy1 = mix(nx01, nx11, v);
	// Interpolate the two last results along z
	double nxyz = mix(nxy0, nxy1, w);

	return nxyz;
}

double perlin3d::noise(const glm::vec3 &point, int octaves, float persistence) {
	double x = point.x;
	double y = point.y;
	double z = point.z;

	double amplitude = 1;
	double max = 0;
	double result = 0;

	while (octaves-- > 0) {
		max += amplitude;
		result += noise({(float)x, (float)y, (float)z}) * amplitude;
		amplitude *= persistence;
		x *= 2;
		y *= 2;
		z *= 2;
	}

	return result / max;
}
