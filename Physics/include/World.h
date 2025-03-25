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


public:
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

	void UpdateBodies(const float deltaTime) noexcept;

	void SetUpQuadTree() noexcept;

	void UpdateQuadTreeCollisions(const BVHNode& node) noexcept;

	[[nodiscard]] bool Overlap(const Collider& colA, const Collider& colB) noexcept;

	void UpdateGlobalCollisions() noexcept; //old code unused

	float SmoothingKernel(float radius, float distance);
	float SmoothingKernelDerivative(float radius, float distance);

	float ProcessDensity(XMVECTOR point);
	float ProcessPropery(XMVECTOR point);
	float ConvertDensityToPressure(float density);
	XMVECTOR processPressureForce(BodyRef bodyref);
	float CalculateSharedPressure(float density1, float density2);

	float smoothigRadius = 15;
	float targetDensity = 1000;
	float pressureMultiplier = 0.5;
};