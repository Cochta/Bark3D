#pragma once
#include <GL/glut.h>

class Camera
{
public:

	// angle of rotation for the camera direction
	float angle = 0.0f;
	// actual vector representing the camera's direction
	float lx = 0.0f, lz = -1.0f;
	// XZ position of the camera
	float x = 0.0f, z = 5.0f;
	// the key states. These variables will be zero
	//when no key is being presses
	float deltaAngle = 0.0f;
	float deltaMove = 0;

	void Init();

	void computePos(float deltaMove);

	void computeDir(float deltaAngle);

private:

};