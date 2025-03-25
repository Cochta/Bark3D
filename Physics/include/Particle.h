#pragma once

#include "Body.h"

struct ParticleData
{
	float Density = 1.0f;
	float Pressure = 0.0f;
	float SmoothingLength = 1.0f;
	float Viscosity = 0.1f;
	//XMVECTOR Acceleration = XMVectorZero();
};