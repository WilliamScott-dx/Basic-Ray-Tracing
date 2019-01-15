/*
COSC363 Assignment 2: Ray Tracer
Author: William Scott (wrs35, 11876177)
Based on example code provided by R. Mukundan, Department of Computer Science and Software Engineering
University of Canterbury, Christchurch, New Zealand.

The cylinder class
*/

#ifndef H_CYLINDER
#define H_CYLINDER

//#include "glm/glm.hpp"
#include <glm/glm.hpp>
#include <math.h>
#include "SceneObject.h"

/**
* Defines a simple Cylinder located at 'center'
* with the specified radius and height
*/
class Cylinder : public SceneObject
{

private:
	glm::vec3 center;
	float radius;
	float height;

public:
	//Default constructor creates a unit cylinder
	Cylinder()
		: center(glm::vec3(0)), radius(1), height(1)
	{
		color = glm::vec3(1);
	};

	Cylinder(glm::vec3 c, float r, float h, glm::vec3 col)
		: center(c), radius(r), height(h)
	{
		color = col;
	};

	float intersect(glm::vec3 posn, glm::vec3 dir);

	glm::vec3 normal(glm::vec3 p);

};

#endif //!H_CYLINDER
