#include "WaterBathSample.h"

std::string WaterBathSample::GetName() noexcept {
	return "Water Bath";
}

std::string WaterBathSample::GetDescription() noexcept {
	return "CONTROLS: Left click to create a sphere, right click to create a "
		"cuboid.\n\nFloor has 1 x bounciness, spheres and cuboids have "
		"0 x bounciness";
}

void WaterBathSample::OnCollisionEnter(ColliderRef col1,
	ColliderRef col2) noexcept {
}

void WaterBathSample::OnCollisionExit(ColliderRef col1,
	ColliderRef col2) noexcept {
}

void WaterBathSample::SampleSetUp() noexcept {
	_world.SetContactListener(this);



	// Create static cuboid for ground
	const auto groundRef = _world.CreateBody();
	_bodyRefs.push_back(groundRef);
	auto& groundBody = _world.GetBody(groundRef);
	groundBody.Type = BodyType::STATIC;
	groundBody.Mass = 1;

	groundBody.Position = { 0,-WALLDIST - WALLSIZE ,0 };

	const auto groundColRef = _world.CreateCollider(groundRef);
	_colRefs.push_back(groundColRef);
	auto& groundCol = _world.GetCollider(groundColRef);
	groundCol.Shape = CuboidF({ -WALLDIST, -WALLSIZE, -WALLDIST },
		{ WALLDIST, WALLSIZE, WALLDIST });
	groundCol.BodyPosition = groundBody.Position;
	groundCol.Restitution = 0.f;

	GraphicsData gd;
	gd.Color = { 160,160,160 };

	AllGraphicsData.emplace_back(gd);

	// Create static cuboids for walls
	// Wall 1
	const auto wall1Ref = _world.CreateBody();
	_bodyRefs.push_back(wall1Ref);
	auto& wall1Body = _world.GetBody(wall1Ref);
	wall1Body.Type = BodyType::STATIC;
	wall1Body.Mass = 1;

	wall1Body.Position = { -WALLDIST - WALLSIZE,0,0 };

	const auto wall1ColRef = _world.CreateCollider(wall1Ref);
	_colRefs.push_back(wall1ColRef);
	auto& wall1Col = _world.GetCollider(wall1ColRef);
	wall1Col.Shape = CuboidF({ -WALLSIZE, -WALLDIST, -WALLDIST },
		{ WALLSIZE, WALLDIST, WALLDIST });
	wall1Col.BodyPosition = wall1Body.Position;
	wall1Col.Restitution = 0.f;

	gd.Filled = false;

	AllGraphicsData.emplace_back(gd);

	// Wall 2
	const auto wall2Ref = _world.CreateBody();
	_bodyRefs.push_back(wall2Ref);
	auto& wall2Body = _world.GetBody(wall2Ref);
	wall2Body.Type = BodyType::STATIC;
	wall2Body.Mass = 1;

	wall2Body.Position = { WALLDIST + WALLSIZE,0,0 };

	const auto wall2ColRef = _world.CreateCollider(wall2Ref);
	_colRefs.push_back(wall2ColRef);
	auto& wall2Col = _world.GetCollider(wall2ColRef);
	wall2Col.Shape = CuboidF({ -WALLSIZE, -WALLDIST, -WALLDIST },
		{ WALLSIZE, WALLDIST, WALLDIST });
	wall2Col.BodyPosition = wall2Body.Position;
	wall2Col.Restitution = 0.f;

	AllGraphicsData.emplace_back(gd);

	// Wall 3
	const auto wall3Ref = _world.CreateBody();
	_bodyRefs.push_back(wall3Ref);
	auto& wall3Body = _world.GetBody(wall3Ref);
	wall3Body.Type = BodyType::STATIC;
	wall3Body.Mass = 1;

	wall3Body.Position = { 0,0,-WALLDIST - WALLSIZE };

	const auto wall3ColRef = _world.CreateCollider(wall3Ref);
	_colRefs.push_back(wall3ColRef);
	auto& wall3Col = _world.GetCollider(wall3ColRef);
	wall3Col.Shape = CuboidF({ -WALLDIST, -WALLDIST, -WALLSIZE },
		{ WALLDIST, WALLDIST, WALLSIZE });
	wall3Col.BodyPosition = wall3Body.Position;
	wall3Col.Restitution = 0.f;

	AllGraphicsData.emplace_back(gd);

	// Wall 4
	const auto wall4Ref = _world.CreateBody();
	_bodyRefs.push_back(wall4Ref);
	auto& wall4Body = _world.GetBody(wall4Ref);
	wall4Body.Type = BodyType::STATIC;
	wall4Body.Mass = 1;

	wall4Body.Position = { 0,0,WALLDIST + WALLSIZE };

	const auto wall4ColRef = _world.CreateCollider(wall4Ref);
	_colRefs.push_back(wall4ColRef);
	auto& wall4Col = _world.GetCollider(wall4ColRef);
	wall4Col.Shape = CuboidF({ -WALLDIST, -WALLDIST, -WALLSIZE },
		{ WALLDIST,WALLDIST, WALLSIZE });
	wall4Col.BodyPosition = wall4Body.Position;
	wall4Col.Restitution = 0.f;

	AllGraphicsData.emplace_back(gd);

	// Create static cuboid for ceiling
	const auto roofRef = _world.CreateBody();
	_bodyRefs.push_back(roofRef);
	auto& roofBody = _world.GetBody(roofRef);
	roofBody.Type = BodyType::STATIC;
	roofBody.Mass = 1;

	roofBody.Position = { 0,WALLDIST + WALLSIZE ,0 };

	const auto roofColRef = _world.CreateCollider(roofRef);
	_colRefs.push_back(roofColRef);
	auto& roofCol = _world.GetCollider(roofColRef);
	roofCol.Shape = CuboidF({ -WALLDIST, -WALLSIZE, -WALLDIST },
		{ WALLDIST, WALLSIZE, WALLDIST });
	roofCol.BodyPosition = roofBody.Position;
	roofCol.Restitution = 0.f;

	AllGraphicsData.emplace_back(gd);

	//density radius representation
	//const auto sphereBodyRef = _world.CreateBody(BodyType::STATIC);
	//_bodyRefs.push_back(sphereBodyRef);
	//auto& sphereBody = _world.GetBody(sphereBodyRef);

	//sphereBody.Mass = 1;

	//sphereBody.Position = { 0,0,0 };

	//const auto sphereColRef = _world.CreateCollider(sphereBodyRef);
	//_colRefs.push_back(sphereColRef);
	//auto& sphereCol = _world.GetCollider(sphereColRef);
	//sphereCol.Shape = Sphere(XMVectorZero(), 15.f);
	//sphereCol.BodyPosition = sphereBody.Position;
	//sphereCol.Restitution = 0.f;
	//sphereCol.IsTrigger = false;


	//gd.Color = { 170,0,0 };

	//AllGraphicsData.emplace_back(gd);

	for (size_t i = 0; i < NBPARTICLES; i++) {
		CreateBall({ Random::Range(-WALLDIST * 0.8f, WALLDIST * 0.8f),
					 Random::Range(-WALLDIST * 0.8f, WALLDIST * 0.8f),
					 Random::Range(-WALLDIST * 0.8f, WALLDIST * 0.8f) });
	}

}
void WaterBathSample::SampleUpdate() noexcept {
	//if (NBPARTICLES + 5 < AllGraphicsData.size()) {
	//	AllGraphicsData.erase(AllGraphicsData.begin() + NBPARTICLES + 5,
	//						  AllGraphicsData.end());
	//}

	for (std::size_t i = 0; i < _colRefs.size(); ++i) {
		const auto& col = _world.GetCollider(_colRefs[i]);

		const auto& shape = _world.GetCollider(_colRefs[i]).Shape;

		switch (shape.index()) {
		case static_cast<int>(ShapeType::Sphere):

			if (XMVectorGetY(col.BodyPosition) <= -500)//fix to reduce quadtree size
			{
				_world.GetBody(col.BodyRef).Position = XMVectorZero();
			}

			//_world.GetBody(col.BodyRef).ApplyForce({ 0, GRAV,0 });
			AllGraphicsData[i].Shape = std::get<SphereF>(shape) + col.BodyPosition;

			break;
		case static_cast<int>(ShapeType::Cuboid):
			AllGraphicsData[i].Shape = std::get<CuboidF>(shape) + col.BodyPosition;
			break;
		default:
			break;
		}
	}

	//_quadTreeGraphicsData.clear();
	//DrawQuadtree(_world.OctTree.Nodes[0]);
	//AllGraphicsData.insert(AllGraphicsData.end(), _quadTreeGraphicsData.begin(),
	//					   _quadTreeGraphicsData.end());
}

void WaterBathSample::SampleTearDown() noexcept {}

void WaterBathSample::CreateBall(XMVECTOR position) noexcept {
	const auto sphereBodyRef = _world.CreateBody(BodyType::FLUID);
	_bodyRefs.push_back(sphereBodyRef);
	auto& sphereBody = _world.GetBody(sphereBodyRef);

	sphereBody.Mass = 1;

	sphereBody.Position = position;

	const auto sphereColRef = _world.CreateCollider(sphereBodyRef);
	_colRefs.push_back(sphereColRef);
	auto& sphereCol = _world.GetCollider(sphereColRef);
	sphereCol.Shape = Sphere(XMVectorZero(), PARTICLESIZE);
	sphereCol.BodyPosition = sphereBody.Position;
	sphereCol.Restitution = 0.f;
	sphereCol.IsTrigger = false;

	GraphicsData gd;

	gd.Color = { 170,213,219 };

	AllGraphicsData.emplace_back(gd);
}

void WaterBathSample::DrawQuadtree(const BVHNode& node) noexcept {
	if (node.Children[0] == nullptr) {
		_quadTreeGraphicsData.push_back({ CuboidF(node.Bounds), false });
	}
	else {
		for (int i = 0; i < 8; i++) {
			DrawQuadtree(*node.Children[i]);
		}
	}
}