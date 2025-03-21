#pragma once

#include "Body.h"
#include "refs.h"
#include "Contact.h"
#include "QuadTree.h"
#include "SPH.h"
#include <vector>
#include <unordered_set>
#include <stdexcept>

/**
 * @brief Represents the physics world containing bodies and interactions.
 * @note This class manages the simulation of physics entities.
 */

class World {
private:
	float RestDensity = 0.1f; // Rest density of the fluid
	float GasConstant = 0.2f; // Gas constant for equation of state
	float ViscosityCoefficient = 0.1f; // Viscosity coefficient
	float Gravity = -9.81f; // Gravity force


	std::vector<Body> _bodies; /**< A collection of all the bodies in the world. */
	std::vector<Collider> _colliders; /**< A collection of all the colliders in the world. */

	HeapAllocator _heapAlloc; /**< Allocator used to track memory usage. */
	std::unordered_set<ColliderRefPair, ColliderRefPairHash, std::equal_to<ColliderRefPair>, StandardAllocator<ColliderRefPair>> _colRefPairs{ _heapAlloc }; /**< A set of colliderRef pairs for collision detection. */

	ContactListener* _contactListener = nullptr; /**< A listener for contact events between colliders. */


public:
	std::vector<size_t> BodyGenIndices; /**< Indices of generated bodies. */
	std::vector<size_t> ColliderGenIndices; /**< Indices of generated colliders. */
	OctTree OctTree{ _heapAlloc };/**< OctTree for collision checks */

	World() noexcept = default;

	void SetUp(int initSize = 100) noexcept;

	void TearDown() noexcept;

	void Update(const float deltaTime) noexcept;


	[[nodiscard]] BodyRef CreateBody() noexcept;

	void DestroyBody(const BodyRef bodyRef);

	[[nodiscard]] Body& GetBody(const BodyRef bodyRef);

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

};