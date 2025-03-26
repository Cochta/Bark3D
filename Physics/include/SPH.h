#pragma once
#include "Utility.h"
namespace SPH {

	inline float SmoothingRadius = 15;
	inline float TargetDensity = 100;
	inline float PressureMultiplier = 25;
	inline float nearPressureMultiplier = 0.5;
	inline float ViscosityStrength = 0.5;
	inline float SpikyPow2ScalingFactor = 15 / (2 * PI * Pow(SmoothingRadius, 5));;
	inline float SpikyPow3ScalingFactor = 15 / (PI * Pow(SmoothingRadius, 6));;
	inline float SpikyPow2DerivativeScalingFactor = 15 / (PI * Pow(SmoothingRadius, 5));
	inline float SpikyPow3DerivativeScalingFactor = 45 / (PI * Pow(SmoothingRadius, 6));
	inline float Poly6ScalingFactor = 315 / (64 * PI * Pow(SmoothingRadius, 9));

	float SmoothingKernelPoly6(float radius, float dst);

	float SpikyKernelPow3(float radius, float dst);
	float SpikyKernelPow2(float radius, float dst);

	float DerivativeSpikyPow3(float radius, float dst);

	float DerivativeSpikyPow2(float radius, float dst);

	float DensityKernel(float radius, float dst);

	float NearDensityKernel(float radius, float dst);

	float DensityDerivative(float radius, float dst);

	float NearDensityDerivative(float radius, float dst);

	float ViscosityKernel(float radius, float dst);
}