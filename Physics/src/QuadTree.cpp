#include "QuadTree.h"

BVHNode::BVHNode(Allocator& alloc) noexcept : ColliderRefAabbs(StandardAllocator<ColliderRefPair>{alloc})
{
	ColliderRefAabbs.reserve(16);
}

BVHNode::BVHNode(const CuboidF& bounds, Allocator& alloc) noexcept : ColliderRefAabbs(StandardAllocator<ColliderRefPair>{ alloc }), Bounds(bounds)
{
	ColliderRefAabbs.reserve(16);
}

BVH::BVH(Allocator& alloc) noexcept : _alloc(alloc), Nodes{ StandardAllocator<BVHNode>{alloc} }
{
	std::size_t result = 0;
	for (size_t i = 0; i <= MAX_DEPTH; i++)
	{
		result += Pow(8, i);
	}
	Nodes.resize(result, BVHNode(_alloc));
}

//void BVH::SubdivideNode(BVHNode& node) noexcept
//{
//	const XMVECTOR halfSize = XMVectorDivide(XMVectorSubtract(node.Bounds.MaxBound(), node.Bounds.MinBound()), XMVectorSet(2, 2, 2, 2));
//	const XMVECTOR minBound = node.Bounds.MinBound();
//
//	node.Children[0] = &Nodes[_nodeIndex];
//	node.Children[0]->Bounds = { minBound, XMVectorAdd(minBound , halfSize) };
//
//	node.Children[1] = &Nodes[_nodeIndex + 1];
//	node.Children[1]->Bounds = { {XMVectorGetX(minBound), XMVectorGetY(minBound) + XMVectorGetY(halfSize)},
//		{XMVectorGetX(minBound) + XMVectorGetX(halfSize), XMVectorGetY(minBound) + 2 * XMVectorGetY(halfSize)} };
//
//	node.Children[2] = &Nodes[_nodeIndex + 2];
//	node.Children[2]->Bounds = { {XMVectorGetX(minBound) + XMVectorGetX(halfSize), XMVectorGetY(minBound)},
//		{XMVectorGetX(minBound) + 2 * XMVectorGetX(halfSize), XMVectorGetY(minBound) + XMVectorGetY(halfSize)} };
//
//	node.Children[3] = &Nodes[_nodeIndex + 3];
//	node.Children[3]->Bounds = { {XMVectorGetX(minBound) + XMVectorGetX(halfSize),
//		XMVectorGetY(minBound) + XMVectorGetY(halfSize)}, node.Bounds.MaxBound() };
//
//
//	for (const auto& child : node.Children)
//	{
//		child->Depth = node.Depth + 1;
//	}
//	_nodeIndex += 4;
//}

void BVH::SubdivideNode(BVHNode& node) noexcept
{
	// Compute half the size of the bounding box
	const XMVECTOR halfSize = XMVectorScale(XMVectorSubtract(node.Bounds.MaxBound(), node.Bounds.MinBound()), 0.5f);
	const XMVECTOR minBound = node.Bounds.MinBound();

	// Precompute offsets for all 8 child nodes
	const XMVECTOR offsets[8] = {
		XMVectorSet(0, 0, 0, 0),                         // Bottom-left-front
		XMVectorSet(0, 0, XMVectorGetZ(halfSize), 0),    // Bottom-left-back
		XMVectorSet(0, XMVectorGetY(halfSize), 0, 0),    // Bottom-right-front
		XMVectorSet(0, XMVectorGetY(halfSize), XMVectorGetZ(halfSize), 0), // Bottom-right-back
		XMVectorSet(XMVectorGetX(halfSize), 0, 0, 0),    // Top-left-front
		XMVectorSet(XMVectorGetX(halfSize), 0, XMVectorGetZ(halfSize), 0), // Top-left-back
		XMVectorSet(XMVectorGetX(halfSize), XMVectorGetY(halfSize), 0, 0), // Top-right-front
		XMVectorSet(XMVectorGetX(halfSize), XMVectorGetY(halfSize), XMVectorGetZ(halfSize), 0) // Top-right-back
	};

	// Create 8 child nodes
	for (int i = 0; i < 8; ++i)
	{
		node.Children[i] = &Nodes[_nodeIndex + i];
		const XMVECTOR childMin = XMVectorAdd(minBound, offsets[i]);
		const XMVECTOR childMax = XMVectorAdd(childMin, halfSize);
		node.Children[i]->Bounds = { childMin, childMax };
		node.Children[i]->Depth = node.Depth + 1;
	}

	// Increment node index for the next subdivision
	_nodeIndex += 8;
}

void BVH::Insert(BVHNode& node, const ColliderRefAabb& colliderRefAabb) noexcept
{
	if (node.Children[0] != nullptr)
	{
		for (const auto& child : node.Children)
		{
			if (Intersect(colliderRefAabb.Aabb, child->Bounds))
			{
				Insert(*child, colliderRefAabb);
			}
		}
	}
	else if (node.ColliderRefAabbs.size() >= MAX_COL_NBR && node.Depth < MAX_DEPTH)
	{
		SubdivideNode(node);
		node.ColliderRefAabbs.push_back(colliderRefAabb);
		for (const auto& child : node.Children)
		{
			for (auto& col : node.ColliderRefAabbs)
			{
				if (Intersect(col.Aabb, child->Bounds))
				{
					Insert(*child, col);
				}
			}
		}
		node.ColliderRefAabbs.clear();
	}
	else
	{
		node.ColliderRefAabbs.push_back(colliderRefAabb);
	}
}

void BVH::SetUpRoot(const CuboidF& bounds) noexcept
{
#ifdef TRACY_ENABLE
	ZoneScoped;
#endif
	for (auto& node : Nodes)
	{
		node.ColliderRefAabbs.clear();
		std::fill(node.Children.begin(), node.Children.end(), nullptr);
	}
	Nodes[0].Bounds = bounds;

	_nodeIndex = 1;
}
