#include "Collider.h"

RectangleF Collider::GetBounds() const noexcept
{
	switch (Shape.index())
	{
	case static_cast<int>(ShapeType::Circle):
	{
		auto circle = std::get<CircleF>(Shape);
		return RectangleF::FromCenter(circle.Center(), { circle.Radius(), circle.Radius() }) + BodyPosition;
	}
	case static_cast<int>(ShapeType::Rectangle):
	{
		return std::get<RectangleF>(Shape) + BodyPosition;
	}
	}
	return { XMVectorZero(), XMVectorZero() };
}

bool ColliderRefPair::operator==(const ColliderRefPair& other) const
{
	return (ColRefA == other.ColRefA && ColRefB == other.ColRefB) ||
		(ColRefA == other.ColRefB && ColRefB == other.ColRefA);
}


std::size_t ColliderRefPairHash::operator()(const ColliderRefPair& pair) const
{
	const std::size_t hashA = std::hash<size_t>{}(pair.ColRefA.Index);
	const std::size_t hashB = std::hash<size_t>{}(pair.ColRefB.Index);

	// XOR for the hash
	return hashA ^ hashB;
}
