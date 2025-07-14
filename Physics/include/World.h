#pragma once

#include "Body.h"
#include "Particle.h"
#include "refs.h"
#include "Contact.h"
#include "QuadTree.h"
#include "SPH.h"
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <stdexcept>
#include <iostream>

/**
 * @brief Represents the physics world containing bodies and interactions.
 * @note This class manages the simulation of physics entities.
 */

class World {
private:
	std::vector<Body> _bodies; /**< A collection of all the bodies in the world. */
	std::vector<Collider> _colliders; /**< A collection of all the colliders in the world. */

	HeapAllocator _heapAlloc; /**< Allocator used to track memory usage. */
	std::unordered_set<ColliderRefPair, ColliderRefPairHash, std::equal_to<ColliderRefPair>, StandardAllocator<ColliderRefPair>> _colRefPairs{ _heapAlloc }; /**< A set of colliderRef pairs for collision detection. */

	ContactListener* _contactListener = nullptr; /**< A listener for contact events between colliders. */

	std::unordered_map<BodyRef, ParticleData, BodyRefHash, std::equal_to<BodyRef>, StandardAllocator<std::pair<const BodyRef, ParticleData>>> _particlesData{ _heapAlloc }; /**< A map of particle data associated with bodies. */

	SpatialHashGrid grid;
	std::vector<BodyRef> neighbors;
public:
	float Gravity = 500;

	std::vector<size_t> BodyGenIndices; /**< Indices of generated bodies. */
	std::vector<size_t> ColliderGenIndices; /**< Indices of generated colliders. */

	OctTree OctTree{ _heapAlloc };/**< OctTree for collision checks */

	World() noexcept = default;

	void SetUp(int initSize = 100) noexcept;

	void TearDown() noexcept;

	void Update(const float deltaTime) noexcept;

	//bodies
	[[nodiscard]] BodyRef CreateBody(BodyType type = BodyType::DYNAMIC) noexcept;
	void DestroyBody(const BodyRef bodyRef);
	[[nodiscard]] Body& GetBody(const BodyRef bodyRef);

	//coliders
	[[nodiscard]] ColliderRef CreateCollider(const BodyRef bodyRef) noexcept;
	[[nodiscard]] Collider& GetCollider(const ColliderRef colRef);
	void DestroyCollider(const ColliderRef colRef);

	void SetContactListener(ContactListener* listener) {
		_contactListener = listener;
	}

private:

	void UpdateForces(const float deltaTime) noexcept;

	void SetUpQuadTree() noexcept;

	void UpdateOctTreeCollisions(const BVHNode& node) noexcept;

	[[nodiscard]] bool Overlap(const Collider& colA, const Collider& colB) noexcept;

	void UpdateGlobalCollisions() noexcept; //old code unused

	float SmoothingKernel(float radius, float distance);
	float SmoothingKernelDerivative(float radius, float distance);
	float ViscosityKernelLaplacian(float h, float r)
	{
		if (r >= h) return 0.0f;
		return 45.0f / (XM_PI * pow(h, 6)) * (h - r); // this is the standard Laplacian of viscosity kernel
	}


	XMVECTOR ProcessDensity(BodyRef bodyref);
	float ConvertDensityToPressure(float density);
	float ConvertNearDensityToPressure(float nearDensity);
	XMVECTOR ProcessPressureForce(BodyRef bodyref);
	float CalculateSharedPressure(float density1, float density2);
	float CalculateSharedNearPressure(float density1, float density2);
	XMVECTOR ProcessViscosityForce(BodyRef bodyref);

	void updateGrid() {
#ifdef TRACY_ENABLE
		ZoneScoped;
#endif
		grid.clear();
		for (auto& [ref, data] : _particlesData) {
			grid.insertParticle(ref, data.PredictedPosition = GetBody(ref).PredictedPosition);
		}
	}

	void computeNeighborsDensity() {
#ifdef TRACY_ENABLE
		ZoneScoped;
#endif
		for (auto& [ref, data] : _particlesData) {
			std::vector<BodyRef> neighbors = grid.findNeighbors(data.PredictedPosition);

			float density = 0;

			for (auto& otherRef : neighbors)
			{
				float distance = XMVectorGetX(XMVector3Length(XMVectorSubtract(GetBody(ref).Position, GetBody(otherRef).Position)));
				float influence = SmoothingKernel(SPH::SmoothingRadius, distance);
				density += influence;
			}
			data.Density = density;
		}
	}
	void computeNeighborsPressure() {
#ifdef TRACY_ENABLE
		ZoneScoped;
#endif
		for (auto& [ref, data] : _particlesData) {
			std::vector<BodyRef> neighbors = grid.findNeighbors(data.PredictedPosition);

			XMVECTOR pressureForce = XMVectorZero();

			for (auto& otherRef : neighbors)
			{
				if (ref == otherRef) continue;
				auto& otherParticle = GetBody(otherRef);
				XMVECTOR offset = XMVectorSubtract(GetBody(ref).Position, otherParticle.Position);
				float distance = XMVectorGetX(XMVector3Length(offset));
				XMVECTOR direction = distance == 0 ? g_XMIdentityR1 : offset / distance;
				float slope = SmoothingKernelDerivative(SPH::SmoothingRadius, distance);
				float density = _particlesData.at(otherRef).Density;
				float sharedPressure = CalculateSharedPressure(density, data.Density);
				pressureForce += sharedPressure * direction * slope * otherParticle.Mass / density;
			}
			GetBody(ref).ApplyForce(pressureForce / data.Density);
		}
	}
	void computeNeighborsViscosity() {
#ifdef TRACY_ENABLE
		ZoneScoped;
#endif
		for (auto& [ref, data] : _particlesData) {
			std::vector<BodyRef> neighbors = grid.findNeighbors(data.PredictedPosition);

			auto& thisParticle = GetBody(ref);
			XMVECTOR viscosityForce = XMVectorZero();

			for (auto& otherRef : neighbors)
			{
				if (ref == otherRef) continue;
				auto& otherParticle = GetBody(otherRef);

				XMVECTOR r = thisParticle.Position - otherParticle.Position;
				float distance = XMVectorGetX(XMVector3Length(r));

				if (distance > SPH::SmoothingRadius) continue;

				float influence = SmoothingKernel(SPH::SmoothingRadius, distance);
				viscosityForce += (otherParticle.Velocity - thisParticle.Velocity) * influence * thisParticle.Mass;

			}
			GetBody(ref).ApplyForce(viscosityForce * SPH::ViscosityStrength);
		}
	}
	void computeNeighborsVorticity() {
#ifdef TRACY_ENABLE
		ZoneScoped;
#endif
		std::unordered_map<BodyRef, XMVECTOR, BodyRefHash, std::equal_to<BodyRef>> vorticityMap;

		// 1. Compute vorticity ω = ∇ × velocity
		for (auto& [ref, data] : _particlesData) {
			XMVECTOR omega = XMVectorZero();
			std::vector<BodyRef> neighbors = grid.findNeighbors(data.PredictedPosition);
			auto& thisParticle = GetBody(ref);

			for (auto& otherRef : neighbors) {
				if (ref == otherRef) continue;
				auto& otherParticle = GetBody(otherRef);

				XMVECTOR r = thisParticle.Position - otherParticle.Position;
				float distance = XMVectorGetX(XMVector3Length(r));
				if (distance > SPH::SmoothingRadius || distance == 0) continue;

				XMVECTOR velDiff = otherParticle.Velocity - thisParticle.Velocity;
				XMVECTOR gradKernel = r * SmoothingKernelDerivative(SPH::SmoothingRadius, distance) / distance;

				omega += XMVector3Cross(velDiff, gradKernel);
			}

			vorticityMap[ref] = omega ;
		}

		// 2. Compute vorticity force f_conf = ε * (N × ω)
		for (auto& [ref, data] : _particlesData) {
			std::vector<BodyRef> neighbors = grid.findNeighbors(data.PredictedPosition);
			auto& thisParticle = GetBody(ref);

			// Compute ∇|ω| for this particle
			XMVECTOR gradMagOmega = XMVectorZero();
			float magOmegaThis = XMVectorGetX(XMVector3Length(vorticityMap[ref]));

			for (auto& otherRef : neighbors) {
				if (ref == otherRef) continue;

				XMVECTOR r = GetBody(ref).Position - GetBody(otherRef).Position;
				float distance = XMVectorGetX(XMVector3Length(r));
				if (distance > SPH::SmoothingRadius || distance == 0) continue;

				float magOmegaOther = XMVectorGetX(XMVector3Length(vorticityMap[otherRef]));
				float diff = magOmegaOther - magOmegaThis;
				XMVECTOR dir = r / distance;
				float slope = SmoothingKernelDerivative(SPH::SmoothingRadius, distance);
				gradMagOmega += dir * diff * slope;
			}

			// Normalize ∇|ω| to get N
			XMVECTOR N = XMVectorGetX(XMVector3LengthSq(gradMagOmega)) > 0.0001f ? XMVector3Normalize(gradMagOmega) : XMVectorZero();

			// f_conf = ε * (N × ω)
			constexpr float epsilon = 10.0f; // tunable parameter
			XMVECTOR force = epsilon * XMVector3Cross(N, vorticityMap[ref]);
			//printf("VorticityForce: %f %f %f\n", XMVectorGetX(force), XMVectorGetY(force), XMVectorGetZ(force));
			thisParticle.ApplyForce(force);
		}
	}
};

