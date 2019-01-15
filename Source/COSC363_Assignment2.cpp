/*
	COSC363 Assignment 2: Ray Tracer
	Author: William Scott (wrs35, 11876177)
	Based on example code provided by R. Mukundan, Department of Computer Science and Software Engineering
	University of Canterbury, Christchurch, New Zealand.

	Generates a scene using ray tracing.
*/

#include <stdio.h>
#include <cmath>
#include <math.h>
#include <glm/glm.hpp>
#include <GL/glut.h>
#include <iterator>
#include <iostream>
#include <vector>
#include "SceneObject.h"
#include "Sphere.h"
#include "Ray.h"
#include "Cylinder.h"
#include "Cone.h"
#include "Plane.h"
#include "TextureBMP.h"

using namespace std;

const float WIDTH = 30.0;
const float HEIGHT = 30.0;
const float EDIST = 40.0;
const int NUMDIV = 1000;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX = WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX = HEIGHT * 0.5;

// Eyeball Texture for sphere
TextureBMP texture1;
//A global list containing pointers to objects in the scene
vector<SceneObject*> sceneObjects;


/* Computes the colour value obtained by tracing a ray and finding its
closest point of intersection with objects in the scene. */
glm::vec3 trace(Ray ray, int step)
{
	const glm::vec3 backgroundCol(0.2f, 0.2f, 0.8f);
	const glm::vec3 light(10, 40, -3);
	const float ambientTerm = 0.2f;

	//Compute the closest point of intersetion of objects with the ray
	ray.closestPt(sceneObjects);

	//If there is no intersection return background colour
	if (ray.xindex == -1)
	{
		return backgroundCol;
	}

	// Object's colour
	glm::vec3 color = sceneObjects[ray.xindex]->getColor();
	// Normal vector on the sphere at the point of intersection
	glm::vec3 normalVector = sceneObjects[ray.xindex]->normal(ray.xpt);

	// Chequered Floor
	if (ray.xindex == 0)
	{
		if ((((int(ray.xpt.x) - 1000) / 4) + ((int(ray.xpt.z) - 1000) / 4)) % 2 == 0)
		{
			color = glm::vec3(0.6f, 0.2f, 0.2f); // Darker color
		}
		else
		{
			color = glm::vec3(0.8f, 0.8f, 1.f); // Lighter color
		}
	}

	// Textured Sphere
	if (ray.xindex == 2)
	{
		float pi = 3.14159;
		float u = (atan2(normalVector.x, normalVector.z) / (pi)) + 0.5;
		float v = 0.5 - asin(normalVector.y) / pi;
		color = texture1.getColorAt(u, v);
	}

	// Procedural Pattern
	if (ray.xindex == 3)
	{
		if (((int)ray.xpt.z) % 3 == 0)
		{
			color = glm::vec3(0.2f, 0.6f, 0.2f); // Darker color
		}
		else if (((int)ray.xpt.y) % 3 == 0)
		{
			color = glm::vec3(0.2f, 0.2f, 0.6f); // Darker color
		}
		else
		{
			color = glm::vec3(0.8f, 0.8f, 1.f); // Lighter color
		}
	}

	// The light vector (vector from the point of intersection towards the light source)
	glm::vec3 lightVector = light - ray.xpt;
	lightVector = glm::normalize(lightVector);

	// Check if point in shadow
	Ray shadow(ray.xpt, lightVector);
	shadow.closestPt(sceneObjects);
	bool bInShadow = (shadow.xindex > -1 && shadow.xindex != 4) && (shadow.xdist < glm::distance(ray.xpt, light));

	glm::vec3 reflVector = glm::reflect(-lightVector, normalVector);
	float rDotV = glm::dot(reflVector, -ray.dir);
	// If rDotV is greater than 0, add specular
	if (rDotV > 0)
	{
		color = color + pow(rDotV, 10);
	}

	/* If value of lightDotNormal is negative return ambient color times ambient term
	   Otherwise return the sum of the ambient and diffuse colors (0.2 * col + lightDotNormal * col)    */
	float lightDotNormal = glm::dot(lightVector, normalVector);
	if (lightDotNormal <= 0 || bInShadow)
	{
		return color * ambientTerm;
	}
	else // Add color
	{
		glm::vec3 colorSum;
		// Ray.xindex should be for all objects to be reflective
		if (ray.xindex == 1 && step < MAX_STEPS)
		{
			glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVector);
			Ray reflectedRay(ray.xpt, reflectedDir);
			glm::vec3 reflectedCol = trace(reflectedRay, step + 1); //Recursion!
			colorSum = colorSum + (0.8f * reflectedCol);
		}

		// Transparency
		if (ray.xindex == 4 && step < 10)
		{
			Ray transparentRay(ray.xpt, ray.dir);
			glm::vec3 transparentCol = trace(transparentRay, step + 1);
			colorSum = colorSum + (0.2f * transparentCol);
		}

		return  ((ambientTerm * color) + (lightDotNormal * color) + colorSum);
	}
}

/* In a ray tracing application, it just displays the ray traced image by drawing
	each cell as a quad. */
void display()
{
	float xp, yp;  //grid point
	float cellX = (XMAX - XMIN) / NUMDIV;  //cell width
	float cellY = (YMAX - YMIN) / NUMDIV;  //cell height

	glm::vec3 eye(0.f, -3.f, 30.f);  //The eye position (source of primary rays) is the origin

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBegin(GL_QUADS);  //Each cell is a quad.

	for (int i = 0; i < NUMDIV; i++)  	//For each grid point xp, yp
	{
		xp = XMIN + i*cellX;
		for (int j = 0; j < NUMDIV; j++)
		{
			yp = YMIN + j*cellY;

			glm::vec3 dir(xp + 0.5*cellX, yp + 0.5*cellY, -EDIST);	//direction of the primary ray

			Ray ray = Ray(eye, dir);		//Create a ray originating from the camera in the direction 'dir'
			ray.normalize();				//Normalize the direction of the ray to a unit vector
			glm::vec3 col = trace(ray, 1); //Trace the primary ray and get the colour value

			glColor3f(col.r, col.g, col.b);
			glVertex2f(xp, yp);				//Draw each cell with its color value
			glVertex2f(xp + cellX, yp);
			glVertex2f(xp + cellX, yp + cellY);
			glVertex2f(xp, yp + cellY);
		}
	}

	glEnd();
	glFlush();
}

void drawTetrahedron(void)
{
	glm::vec3 tetrahedronOffset(-15, 0, -30);

	glm::vec3 point1 = glm::vec3(0.f, 5.f, 0.f) + tetrahedronOffset;
	glm::vec3 point2 = glm::vec3(2.5f, 5.f, 5.f) + tetrahedronOffset;
	glm::vec3 point3 = glm::vec3(5.f, 5.f, 5.f) + tetrahedronOffset;
	glm::vec3 point4 = glm::vec3(2.5f, 10.f, 2.5f) + tetrahedronOffset; // Top Point

	Plane *tetraSide1 = new Plane(point3, point2, point1, point3, glm::vec3(1.f, 0.f, 0.f));
	Plane *tetraSide2 = new Plane(point4, point2, point1, point1, glm::vec3(0.f, 1.f, 0.f));
	Plane *tetraSide3 = new Plane(point1, point3, point4, point4, glm::vec3(0.f, 0.f, 1.f));
	Plane *tetraSide4 = new Plane(point2, point3, point4, point4, glm::vec3(0.6f, 0.3f, 0.3f));

	sceneObjects.push_back(tetraSide1);
	sceneObjects.push_back(tetraSide2);
	sceneObjects.push_back(tetraSide3);
	sceneObjects.push_back(tetraSide4);
}

void drawCube(void)
{
	glm::vec3 cubeOffset(10, 5, -30);

	glm::vec3 point1 = glm::vec3(0.f, 0.f, 2.5) + cubeOffset;
	glm::vec3 point3 = glm::vec3(2.5, 0, 5) + cubeOffset;
	glm::vec3 point6 = glm::vec3(5, 0, 2.5) + cubeOffset;
	glm::vec3 point8 = glm::vec3(2.5, 0, 0) + cubeOffset;

	glm::vec3 point2 = glm::vec3(0.f, 2.5, 2.5) + cubeOffset;
	glm::vec3 point4 = glm::vec3(2.5, 2.5, 5) + cubeOffset;
	glm::vec3 point5 = glm::vec3(5, 2.5, 2.5) + cubeOffset;
	glm::vec3 point7 = glm::vec3(2.5, 2.5, 0) + cubeOffset;

	//Plane *cubeSide1 = new Plane(point3, point1, point8, point6, glm::vec3(1.f, 0.f, 0.f));
	Plane *cubeSide1 = new Plane(point1, point3, point6, point8, glm::vec3(1.f, 0.f, 0.f));
	Plane *cubeSide2 = new Plane(point1, point2, point7, point8, glm::vec3(1.f, 0.f, 0.f));
	Plane *cubeSide3 = new Plane(point1, point2, point4, point3, glm::vec3(1.f, 0.3f, 0.3f));
	Plane *cubeSide4 = new Plane(point6, point3, point4, point5, glm::vec3(1.f, 1.f, 0.2f));
	Plane *cubeSide5 = new Plane(point6, point8, point7, point5, glm::vec3(1.f, 0.f, 1.f));
	Plane *cubeSide6 = new Plane(point2, point4, point5, point7, glm::vec3(1.f, 1.f, 0.f));

	sceneObjects.push_back(cubeSide1);
	sceneObjects.push_back(cubeSide2);
	sceneObjects.push_back(cubeSide3);
	sceneObjects.push_back(cubeSide4);
	sceneObjects.push_back(cubeSide5);
	sceneObjects.push_back(cubeSide6);
}

void drawCylinder(void)
{
	Cylinder *cylinder1 = new Cylinder(glm::vec3(-15.0, -19.0, -50.0), 3.0, 5.f, glm::vec3(0.2f, 0.5f, 1.f));
	sceneObjects.push_back(cylinder1);
}

//---This function initializes the scene -------------------------------------------
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
	glClearColor(0, 0, 0, 1);
	texture1 = TextureBMP((char*)"eye.bmp");

	//-- Create pointers to spheres for scene
	Sphere *sphere1 = new Sphere(glm::vec3(-5.0, -5.0, -90.0), 15.0, glm::vec3(0, 0, 1));
	Sphere *sphere2 = new Sphere(glm::vec3(3.0, 3.0, -70.0), 3.0, glm::vec3(1, 0, 0));
	Sphere *sphere3 = new Sphere(glm::vec3(5.0, -15.0, -60.0), 5.0, glm::vec3(0, 1, 0));

	// Create a pointer to a plane
	Plane *plane = new Plane(glm::vec3(-20., -20, -40), //Point A
		glm::vec3(20., -20, -40), //Point B
		glm::vec3(20., -20, -200), //Point C
		glm::vec3(-20., -20, -200), //Point D
		glm::vec3(0.5, 0.5, 0)); //Colour

	Plane *window = new Plane(glm::vec3(-2., -2, -20), //Point A
			glm::vec3(2., -2, -20), //Point B
			glm::vec3(2., 2, -20), //Point C
			glm::vec3(-2., 2, -20), //Point D
			glm::vec3(0.3, 0.3, 0.3)); //Colour

	// Cone
	Cone *cone1 = new Cone(glm::vec3(5.0, -5.0, -60.0), 5.0, 7.f, glm::vec3(0, 0, 1));


	//--Add the above to the list of scene objects.
	sceneObjects.push_back(plane); // Checkered Plane 0
	sceneObjects.push_back(sphere1);  // Reflective 1
	sceneObjects.push_back(sphere2); // Textured 2
	sceneObjects.push_back(sphere3);  // Procedural Pattern 3
	sceneObjects.push_back(window); // Transparency? 4
	drawCylinder();
	sceneObjects.push_back(cone1);

	drawCube();
	drawTetrahedron();

}

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(20, 20);
	glutCreateWindow("Raytracer - William Scott 11876177");

	glutDisplayFunc(display);
	initialize();

	glutMainLoop();
	return 0;
}
