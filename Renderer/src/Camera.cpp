#include "Camera.h"
#include <corecrt_math.h>

void Camera::Init()
{
	// Set the camera
	gluLookAt(x, 1.0f, z, x + lx, 1.0f, z + lz, 0.0f, 1.0f, 0.0f);
}

void Camera::computePos(float deltaMove)
{

	x += deltaMove * lx * 0.1f;
	z += deltaMove * lz * 0.1f;
}

void Camera::computeDir(float deltaAngle)
{

	angle += deltaAngle;
	lx = sin(angle);
	lz = -cos(angle);
}
