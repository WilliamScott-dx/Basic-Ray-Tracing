/*
COSC363 Assignment 2: Ray Tracer
Author: William Scott (wrs35, 11876177)
Based on example code provided by R. Mukundan, Department of Computer Science and Software Engineering
University of Canterbury, Christchurch, New Zealand.

The cylinder class
*/

#include "Cylinder.h"

/*
* Cylinder intersection method
* Returns the unit normal vector at a given point.
* Slide 31 Lecture 9
*/
float Cylinder::intersect(glm::vec3 posn, glm::vec3 dir)
{
	float a = (pow(dir.x , 2) + pow(dir.z, 2));
	float b = 2 * (dir.x * (posn.x - center.x) + dir.z * (posn.z - center.z));
	float c = pow(posn.x - center.x, 2) + pow(posn.z - center.z, 2) - pow(radius, 2);
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

	// Cap Cylinder for no infinite height
	float max = t1;
	float min = t2;
	if (t2 > t1)
	{
		max = t2;
		min = t1;
	}

	float yMin = (posn.y + (dir.y * min)) - center.y;
	float yMax = (posn.y + (dir.y * max)) - center.y;
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
		return -1.f;
	}
}

/**
* Returns the unit normal vector at a given point.
* Slide 31 Lecture 9
*/
glm::vec3 Cylinder::normal(glm::vec3 p)
{
	glm::vec3 n = glm::vec3(p.x - center.x, 0.f, p.z - center.z);
	n = glm::normalize(n);
	return n;
}
