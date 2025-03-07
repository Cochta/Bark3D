#pragma once

#include "Collider.h"
#include "Allocators.h"
#include "UniquePtr.h"

#include <memory>
#include <array>

static constexpr int MAX_COL_NBR = 16; /**< Maximum number of colliders in a quadtree node. */
static constexpr int MAX_DEPTH = 4; /**< Maximum depth of the quadtree. */
static constexpr int SUBDIVIDE_NBR = 8; /**<Number of subdivisions>*/

/**
 * @brief Structure representing a bounding box (AABB) associated with a collider reference.
 */
struct ColliderRefAabb
{
	const CuboidF Aabb;    /**< The bounding box (AABB). */
	const ColliderRef ColRef;       /**< The reference to a collider. */
};

/**
 * @brief Class representing a node in a quadtree data structure for collision detection.
 */
class BVHNode
{
public:
	CustomlyAllocatedVector<ColliderRefAabb> ColliderRefAabbs;  /**< Vector of collider references with AABBs. */
	CuboidF Bounds{ XMVectorZero(), XMVectorZero() }; /**< The bounds of the quadtree node. */
	std::array<BVHNode*, SUBDIVIDE_NBR> Children{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr }; /**< Array of child nodes. */
	int Depth = 0; /**< The depth of the node in the quadtree.*/

	/**
	 * @brief Constructor for BVHNode, allocating memory using a specified allocator.
	 * @param alloc The allocator for memory allocation.
	 */
	BVHNode(Allocator& alloc) noexcept;

	/**
	 * @brief Constructor for BVHNode with a specified bounds, allocating memory using a specified allocator.
	 * @param bounds The bounds of the quadtree node.
	 * @param alloc The allocator for memory allocation.
	 */
	explicit BVHNode(const CuboidF& bounds, Allocator& alloc) noexcept;
};

/**
 * @brief Class representing a quadtree data structure for collision detection.
 */
class OctTree {
public:
	CustomlyAllocatedVector<BVHNode> Nodes; /**< Vector of quadtree nodes. */

private:

	int _nodeIndex = 1; /**< The index of the current node in the quadtree. */
	Allocator& _alloc; /**< The allocator for memory allocation.*/

public:
	/**
	 * @brief Constructor for OctTree, allocating memory using a specified allocator.
	 * @param alloc The allocator for memory allocation.
	 */
	OctTree(Allocator& alloc) noexcept;

	/**
	 * @brief Set up the root of the quadtree with the specified bounds.
	 * @param bounds The bounds of the root node.
	 */
	void SetUpRoot(const CuboidF& bounds) noexcept;

	/**
	 * @brief Insert a collider reference with an associated AABB into the quadtree.
	 * @param node The node to insert into.
	 * @param colliderRefAabb The collider reference with an AABB to insert.
	 */
	void Insert(BVHNode& node, const ColliderRefAabb& colliderRefAabb) noexcept;
private:
	/**
	 * @brief Subdivide a quadtree node into smaller child nodes.
	 * @param node The node to subdivide.
	 */
	void SubdivideNode(BVHNode& node) noexcept;
};