#include "World.h"

#ifdef TRACY_ENABLE
#include <Tracy.hpp>
#include <TracyC.h>
#endif 

void World::SetUp(int initSize) noexcept
{
	_bodies.resize(initSize);
	BodyGenIndices.resize(initSize, 0);

	_colliders.resize(initSize);
	ColliderGenIndices.resize(initSize, 0);
}

void World::TearDown() noexcept
{
	_bodies.clear();
	BodyGenIndices.clear();
	_colliders.clear();

	ColliderGenIndices.clear();

	_colRefPairs.clear();
}

void World::Update(const float deltaTime) noexcept
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	UpdateBodies(deltaTime);


	//UpdateGlobalCollisions();
	SetUpQuadTree();

	UpdateQuadTreeCollisions(OctTree.Nodes[0]);

}

[[nodiscard]] BodyRef World::CreateBody() noexcept
{
	const auto it = std::find_if(_bodies.begin(), _bodies.end(), [](const Body& body) {
		return !body.IsEnabled(); // Get first disabled body
	});

	if (it != _bodies.end())
	{
		const std::size_t index = std::distance(_bodies.begin(), it);
		const auto bodyRef = BodyRef{ index, BodyGenIndices[index] };
		GetBody(bodyRef).Enable();
		return bodyRef;
	}

	const std::size_t previousSize = _bodies.size();

	_bodies.resize(previousSize * 2, Body());
	BodyGenIndices.resize(previousSize * 2, 0);

	const BodyRef bodyRef = { previousSize, BodyGenIndices[previousSize] };
	GetBody(bodyRef).Enable();
	return bodyRef;

}

void World::DestroyBody(const BodyRef bodyRef)
{
	if (BodyGenIndices[bodyRef.Index] != bodyRef.GenIndex)
	{
		throw std::runtime_error("No body found !");
	}

	_bodies[bodyRef.Index].Disable();
}

[[nodiscard]] Body& World::GetBody(const BodyRef bodyRef)
{
	if (BodyGenIndices[bodyRef.Index] != bodyRef.GenIndex)
	{
		throw std::runtime_error("No body found !");
	}

	return _bodies[bodyRef.Index];
}

ColliderRef World::CreateCollider(const BodyRef bodyRef) noexcept
{
	const auto it = std::find_if(_colliders.begin(), _colliders.end(), [](const Collider& collider) {
		return !collider.IsAttached; // Get first disabled collider
	});

	if (it != _colliders.end())
	{
		const std::size_t index = std::distance(_colliders.begin(), it);
		const auto colRef = ColliderRef{ index, ColliderGenIndices[index] };
		auto& col = GetCollider(colRef);
		col.IsAttached = true;
		col.BodyRef = bodyRef;

		return colRef;
	}

	const std::size_t previousSize = _colliders.size();

	_colliders.resize(previousSize * 2, Collider());
	ColliderGenIndices.resize(previousSize * 2, 0);

	const ColliderRef colRef = { previousSize, ColliderGenIndices[previousSize] };
	auto& col = GetCollider(colRef);
	col.IsAttached = true;
	col.BodyRef = bodyRef;
	return colRef;
}

Collider& World::GetCollider(const ColliderRef colRef)
{
	if (ColliderGenIndices[colRef.Index] != colRef.GenIndex)
	{
		throw std::runtime_error("No collider found !");
	}

	return _colliders[colRef.Index];
}

void World::DestroyCollider(const ColliderRef colRef)
{
	if (ColliderGenIndices[colRef.Index] != colRef.GenIndex)
	{
		throw std::runtime_error("No collider found !");
	}
	_colliders[colRef.Index].IsAttached = false;
}

void World::UpdateBodies(const float deltaTime) noexcept
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	for (auto& body : _bodies)
	{
		if (!body.IsEnabled())
		{
			continue;
		}
		if (body.Type == BodyType::STATIC)
		{
			continue;
		}
		auto acceleration = XMVectorScale(body.GetForce(), 1 / body.Mass);
		body.Velocity = XMVectorAdd(body.Velocity, XMVectorScale(acceleration, deltaTime));
		body.Position = XMVectorAdd(body.Position, XMVectorScale(body.Velocity, deltaTime));

		body.ResetForce();
	}
}

void World::SetUpQuadTree() noexcept {
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif

#ifdef TRACY_ENABLE
	ZoneNamedN(SetRoodNodeBoundary, "SetRootNodeBounds", true);
	ZoneValue(_colliders.size());
#endif

	XMVECTOR maxBounds = XMVectorSet(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), 0);
	XMVECTOR minBounds = XMVectorSet(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), 0);

	for (auto& collider : _colliders) {
		if (!collider.IsAttached) {
			continue;
		}

		collider.BodyPosition = GetBody(collider.BodyRef).Position;

		const auto bounds = collider.GetBounds();

		minBounds = XMVectorSetX(minBounds, std::min(XMVectorGetX(minBounds), XMVectorGetX(bounds.MinBound())));
		minBounds = XMVectorSetY(minBounds, std::min(XMVectorGetY(minBounds), XMVectorGetY(bounds.MinBound())));
		minBounds = XMVectorSetZ(minBounds, std::min(XMVectorGetZ(minBounds), XMVectorGetZ(bounds.MinBound())));
		maxBounds = XMVectorSetX(maxBounds, std::max(XMVectorGetX(maxBounds), XMVectorGetX(bounds.MaxBound())));
		maxBounds = XMVectorSetY(maxBounds, std::max(XMVectorGetY(maxBounds), XMVectorGetY(bounds.MaxBound())));
		maxBounds = XMVectorSetZ(maxBounds, std::max(XMVectorGetZ(maxBounds), XMVectorGetZ(bounds.MaxBound())));
	}

	OctTree.SetUpRoot(CuboidF(minBounds, maxBounds));

#ifdef TRACY_ENABLE
	ZoneNamedN(Insert, "Insert in OctTree", true);
#endif
	for (std::size_t i = 0; i < _colliders.size(); ++i) {
		if (!_colliders[i].IsAttached) {
			continue;
		}

		OctTree.Insert(OctTree.Nodes[0], { _colliders[i].GetBounds(), { i, ColliderGenIndices[i] } });

	}
}

void World::UpdateQuadTreeCollisions(const BVHNode& node) noexcept
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	if (node.Children[0] == nullptr)
	{
		if (node.ColliderRefAabbs.empty())
		{
			return;
		}
		for (std::size_t i = 0; i < node.ColliderRefAabbs.size() - 1; ++i)
		{
			auto& col1 = GetCollider(node.ColliderRefAabbs[i].ColRef);
			auto& body1 = GetBody(col1.BodyRef);

			for (std::size_t j = i + 1; j < node.ColliderRefAabbs.size(); ++j)
			{
				auto& col2 = GetCollider(node.ColliderRefAabbs[j].ColRef);
				auto& body2 = GetBody(col2.BodyRef);

				/*	if (body1.ParticleData && body2.ParticleData)
					{
						continue;
					}*/

				if (!col2.IsTrigger && !col1.IsTrigger) // Physical collision
				{
					if (Overlap(col1, col2))
					{
						Contact contact;
						contact.CollidingBodies[0] = { &GetBody(col1.BodyRef), &col1 };
						contact.CollidingBodies[1] = { &GetBody(col2.BodyRef), &col2 };
						contact.Resolve();
						if (_contactListener != nullptr)
						{
							_contactListener->OnCollisionEnter(node.ColliderRefAabbs[i].ColRef, node.ColliderRefAabbs[j].ColRef);
						}
					}
					else
					{
						if (_contactListener != nullptr)
						{
							_contactListener->OnCollisionExit(node.ColliderRefAabbs[i].ColRef, node.ColliderRefAabbs[j].ColRef);
						}
					}
					continue;
				}

				if (_contactListener == nullptr)
				{
					continue;
				}


				// Trigger collision
				const ColliderRefPair& colPair = { node.ColliderRefAabbs[i].ColRef, node.ColliderRefAabbs[j].ColRef };

				if (_colRefPairs.find(colPair) != _colRefPairs.end())
				{
					if (!Overlap(col1, col2))
					{
						_contactListener->OnTriggerExit(colPair.ColRefA, colPair.ColRefB);
						_colRefPairs.erase(colPair);
					}
					continue;
				}

				if (Overlap(col1, col2))
				{
					_contactListener->OnTriggerEnter(colPair.ColRefA, colPair.ColRefB);
					_colRefPairs.emplace(colPair);
				}
			}
		}
	}
	else
	{
		for (const auto& child : node.Children)
		{
			UpdateQuadTreeCollisions(*child);
		}
	}
}

[[nodiscard]] bool World::Overlap(const Collider& colA, const Collider& colB) noexcept
{
	const auto ShapeA = static_cast<ShapeType>(colA.Shape.index());
	const auto ShapeB = static_cast<ShapeType>(colB.Shape.index());

#ifdef TRACY_ENABLE
	ZoneScoped;
#endif

	switch (ShapeA)
	{
	case ShapeType::Sphere:
	{
		SphereF sphere = std::get<SphereF>(colA.Shape) + GetBody(colA.BodyRef).Position;
		switch (ShapeB)
		{
		case ShapeType::Sphere:
		return Intersect(sphere, std::get<SphereF>(colB.Shape) + GetBody(colB.BodyRef).Position);
		case ShapeType::Cuboid:
		return Intersect(sphere, std::get<CuboidF>(colB.Shape) + GetBody(colB.BodyRef).Position);
		}
		break;
	}
	case ShapeType::Cuboid:
	{
		CuboidF rect = std::get<CuboidF>(colA.Shape) + GetBody(colA.BodyRef).Position;
		switch (ShapeB)
		{
		case ShapeType::Sphere:
		return Intersect(rect, std::get<SphereF>(colB.Shape) + GetBody(colB.BodyRef).Position);
		case ShapeType::Cuboid:
		return Intersect(rect, std::get<CuboidF>(colB.Shape) + GetBody(colB.BodyRef).Position);
		}
		break;
	}
	}
	return false;
}

void World::UpdateGlobalCollisions() noexcept
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif

	for (std::size_t i = 0; i < _colliders.size() - 1; ++i)
	{
		ColliderRef colRef1{ i, ColliderGenIndices[i] };
		auto& col1 = GetCollider(colRef1);
		auto& body1 = GetBody(col1.BodyRef);
		col1.BodyPosition = body1.Position;

		if (!col1.IsAttached) continue;

		//if (body1.ParticleData)
		//{
		//	// Reset density and forces for SPH calculations
		//	float density = 0.0f;
		//	XMVECTOR pressureForce = XMVectorZero();
		//	XMVECTOR viscosityForce = XMVectorZero();

		//	for (std::size_t j = 0; j < _colliders.size(); ++j)
		//	{
		//		if (i == j) continue;

		//		ColliderRef colRef2{ j, ColliderGenIndices[j] };
		//		auto& col2 = GetCollider(colRef2);
		//		auto& body2 = GetBody(col2.BodyRef);

		//		if (!col2.IsAttached || !body2.ParticleData) continue;

		//		// Compute distance between particles
		//		XMVECTOR diff = XMVectorSubtract(body1.Position, body2.Position);
		//		float distanceSquared = XMVectorGetX(XMVector3LengthSq(diff));
		//		float h = body1.ParticleData->SmoothingLength;
		//		float h2 = h * h;

		//		if (distanceSquared < h2)
		//		{
		//			float distance = sqrt(distanceSquared);
		//			float h_minus_r2 = h2 - distanceSquared;

		//			// Compute density contribution using Poly6 kernel
		//			float densityWeight = (315.0f / (64.0f * XM_PI * pow(h, 9))) * h_minus_r2 * h_minus_r2 * h_minus_r2;
		//			density += body2.Mass * densityWeight;

		//			if (distance > 0)
		//			{
		//				// Compute pressure force using Spiky kernel
		//				float weight = (45.0f / (XM_PI * pow(h, 6))) * (h - distance) * (h - distance);
		//				float pressureTerm = (body1.ParticleData->Pressure + body2.ParticleData->Pressure) / (2 * body2.ParticleData->Density);
		//				XMVECTOR pressureDir = XMVectorScale(diff, 1.0f / distance);
		//				pressureForce = XMVectorSubtract(pressureForce, XMVectorScale(pressureDir, pressureTerm * weight));

		//				// Compute viscosity force
		//				XMVECTOR velocityDiff = XMVectorSubtract(body2.Velocity, body1.Velocity);
		//				viscosityForce = XMVectorAdd(viscosityForce, XMVectorScale(velocityDiff, ViscosityCoefficient * weight / body2.ParticleData->Density));
		//			}
		//		}
		//	}

		//	// Update density and pressure for body1
		//	body1.ParticleData->Density = density;
		//	body1.ParticleData->Pressure = GasConstant * (density - RestDensity);

		//	// Apply forces
		//	XMVECTOR gravityForce = XMVectorSet(0.0f, Gravity * body1.Mass, 0.0f, 0.0f);
		//	body1.ApplyForce(XMVectorAdd(pressureForce, XMVectorAdd(viscosityForce, gravityForce)));
		//	continue;
		//}

		// Handle solid-body collisions
		for (std::size_t j = i + 1; j < _colliders.size(); ++j)
		{
			ColliderRef colRef2{ j, ColliderGenIndices[j] };
			auto& col2 = GetCollider(colRef2);
			auto& body2 = GetBody(col2.BodyRef);

			if (!col2.IsAttached) continue;

			if (!col2.IsTrigger && !col1.IsTrigger) // Physical collision
			{
				if (!Overlap(col1, col2))
				{
					if (_contactListener) _contactListener->OnCollisionExit(colRef1, colRef2);
				}
				else
				{
					Contact contact;
					contact.CollidingBodies[0] = { &body1, &col1 };
					contact.CollidingBodies[1] = { &body2, &col2 };
					contact.Resolve();
					if (_contactListener) _contactListener->OnCollisionEnter(colRef1, colRef2);
				}
				continue;
			}

			if (_contactListener == nullptr)
			{
				return;
			}

			if (_colRefPairs.find({ colRef1, colRef2 }) != _colRefPairs.end())
			{
				if (!Overlap(col1, col2))
				{
					_contactListener->OnTriggerExit(colRef1, colRef2);
					_colRefPairs.erase({ colRef1, colRef2 });
				}
				continue;
			}

			if (Overlap(col1, col2))
			{
				_contactListener->OnTriggerEnter(colRef1, colRef2);
				_colRefPairs.insert({ colRef1, colRef2 });
			}
		}
	}
}