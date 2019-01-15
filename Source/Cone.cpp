/*
	COSC363 Assignment 2: Ray Tracer
	Author: William Scott (wrs35, 11876177)
	Based on example code provided by R. Mukundan, Department of Computer Science and Software Engineering
										 			  University of Canterbury, Christchurch, New Zealand.

	Cone Class
*/

#include "Cone.h"

/**
* Cone's intersection method.  The input is a ray (pos, dir).
* Lecture 9 Slide 33
*/
float Cone::intersect(glm::vec3 posn, glm::vec3 dir)
{
	float radiusOverHeightSq = pow((radius / height), 2);
	float a = pow(dir.x, 2) + pow(dir.z, 2) - (radiusOverHeightSq * pow(dir.y, 2));
	float b = 2 * ((posn.x * dir.x - center.x * dir.x) + (posn.z * dir.z - center.z * dir.z) - ((radiusOverHeightSq * posn.y * dir.y) - (radiusOverHeightSq * center.y * dir.y) - (radiusOverHeightSq * height * dir.y)));
	float h2 = height * 2;
	float c = pow((posn.x - center.x), 2) + pow((posn.z - center.z), 2)
														- radiusOverHeightSq
	 													* ((h2 * center.y)
														- (h2 * posn.y)
														- (2 * posn.y * center.y)
														+ pow(posn.y, 2)
														+ pow(center.y, 2)
														+ pow(height, 2));

	float delta = (pow(b, 2) - (4.f * a * c));

	if (fabs(delta) < 0.001) return -1.0;
	if (delta < 0.0) return -1.0;

	float t1 = (-b - sqrt(delta)) / (2 * a);
	float t2 = (-b + sqrt(delta)) / (2 * a);
	if (fabs(t1) < 0.001)
	{
		if (t2 > 0) return t2;
		else t1 = -1.0;
	}
	if (fabs(t2) < 0.001) t2 = -1.0;
	/////////////////////
	float max = t1;
	float min = t2;
	if (t2 > t1)
	{
		max = t2;
		min = t1;
	}

	float yMin = (posn.y + min * dir.y) - center.y;
	float yMax = (posn.y + max * dir.y) - center.y;
	if ((yMin >= 0 && yMin <= height) && min != -1.0)
	{
		return min;
	}
	else if ((yMax >= 0 && yMax <= height) && max != -1.0)
	{
		return max;
	}
	else
	{
		return -1.0;
	}
}

/**
* Returns the unit normal vector at a given point.
* Slide 32 Lecture 9
*/
glm::vec3 Cone::normal(glm::vec3 p)
{
	float alpha = atan((p.x - center.x) / (p.z - center.z));
	float theta = atan(radius / height);
	glm::vec3 n = glm::vec3(sin(alpha) * cos(theta), sin(theta), cos(alpha) * cos(theta));
	n = glm::normalize(n);
	return n;
}
