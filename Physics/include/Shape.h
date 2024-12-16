#pragma once

#include <DirectXMath.h>
#include "Utility.h"
#include <vector>

using namespace DirectX;

enum class ShapeType
{
	Circle, Rectangle, None
};

template <typename T>
class Circle
{
public:
	/**
	 * @brief Construct a new Circle object
	 * @param center Center of the circle, also the position of the circle
	 * @param radius Radius of the circle, if negative, it will be converted to its positive value
	 */
	constexpr Circle(XMVECTOR center, T radius) noexcept : _center(center), _radius(Abs(radius)) {}
	/**
	 * @brief Construct a new Circle object with a center in (0, 0)
	 * @param radius Radius of the circle, if negative, it will be converted to its positive value
	 */
	constexpr explicit Circle(T radius) noexcept : _center(XMVECTOR::Zero()), _radius(Abs(radius)) {}

private:
	XMVECTOR _center = XMVectorZero();
	T _radius = 0;

public:
	[[nodiscard]] constexpr XMVECTOR Center() const noexcept { return _center; }
	[[nodiscard]] constexpr T Radius() const noexcept { return _radius; }

	void SetCenter(XMVECTOR center) noexcept { _center = center; }
	void SetRadius(T radius) noexcept { _radius = Abs(radius); }

	/**
	 * @brief Check if the circle contains a point
	 * @param point the point to check
	 * @return true if the point is inside the circle, false otherwise
	 */
	 /*[[nodiscard]] bool Contains(XMVECTOR point) const
	 {
		 return (_center - point).SquareLength() <= _radius * _radius;
	 }*/

	[[nodiscard]] bool Contains(XMVECTOR point) const {
		// Compute the vector difference between the circle's center and the point
		XMVECTOR delta = XMVectorSubtract(_center, point);

		// Calculate the squared distance (squared length of delta)
		float squaredDistance = XMVectorGetX(XMVector3LengthSq(delta));

		// Compare squared distance with squared radius
		return squaredDistance <= (_radius * _radius);
	}

	[[nodiscard]] constexpr Circle<T> operator+(const XMVECTOR& vec) const noexcept
	{
		return Circle<T>(XMVectorAdd(_center, vec), _radius);
	}
};

using CircleF = Circle<float>;
using CircleI = Circle<int>;

template <typename T>
class Rectangle
{
public:
	/**
	 * @brief Construct a new Rectangle object
	 * @param position the position of the rectangle
	 * @param size the size of the rectangle
	 */
	constexpr Rectangle(XMVECTOR minBound, XMVECTOR maxBound) noexcept : _minBound(minBound), _maxBound(maxBound) {}

private:
	XMVECTOR _minBound = XMVECTOR::Zero();
	XMVECTOR _maxBound = XMVECTOR::Zero();

public:
	[[nodiscard]] constexpr XMVECTOR MinBound() const noexcept { return _minBound; }
	[[nodiscard]] constexpr XMVECTOR MaxBound() const noexcept { return _maxBound; }

	void SetMinBound(XMVECTOR minBound) noexcept { _minBound = minBound; }
	void SetMaxBound(XMVECTOR maxBound) noexcept { _maxBound = maxBound; }

	/**
	 * @brief Check if the rectangle contains a point
	 * @param point the point to check
	 * @return true if the point is inside the rectangle, false otherwise
	 */
	[[nodiscard]] bool Contains(XMVECTOR point) const
	{
		if (XMVectorGetX(_maxBound) < XMVectorGetX(point) || XMVectorGetX(_minBound) > XMVectorGetX(point)) return false;
		if (XMVectorGetY(_maxBound) < XMVectorGetY(point) || XMVectorGetY(_minBound) > XMVectorGetY(point)) return false;

		return true;
	}

	[[nodiscard]] constexpr XMVECTOR Center() const noexcept
	{
		return XMVectorScale(XMVectorAdd(_minBound, _maxBound), 0.5f);
	}

	[[nodiscard]] constexpr XMVECTOR Size() const noexcept
	{
		return XMVectorSubtract(_maxBound, _minBound);
	}

	[[nodiscard]] constexpr XMVECTOR HalfSize() const noexcept
	{
		return XMVectorScale(XMVectorSubtract(_maxBound, _minBound), 0.5f);
	}

	[[nodiscard]] constexpr Rectangle<T> operator+(const XMVECTOR& vec) const noexcept
	{
		return Rectangle<T>(XMVectorAdd(_minBound, vec), XMVectorAdd(_maxBound, vec));
	}

	[[nodiscard]] static constexpr Rectangle<T> FromCenter(XMVECTOR center, XMVECTOR halfSize) noexcept
	{
		return Rectangle<T>(XMVectorSubtract(center, halfSize), XMVectorAdd(center, halfSize));
	}
};

using RectangleF = Rectangle<float>;
using RectangleI = Rectangle<int>;

// Intersect functions

template<typename T>
[[nodiscard]] constexpr bool Intersect(const Circle<T> circle1, const Circle<T> circle2) noexcept
{
	const T radiusSum = (circle1.Radius() + circle2.Radius()) * (circle1.Radius() + circle2.Radius());

	const T distanceBetweenCenters = XMVectorGetX(XMVector3LengthSq(XMVectorSubtract(circle1.Center(), circle2.Center())));
	return distanceBetweenCenters <= radiusSum;
}

template<typename T>
[[nodiscard]] constexpr bool Intersect(const Rectangle<T> rectangle1, const Rectangle<T> rectangle2) noexcept
{
	if (XMVectorGetX(rectangle1.MaxBound()) < XMVectorGetX(rectangle2.MinBound()) ||
		XMVectorGetX(rectangle1.MinBound()) > XMVectorGetX(rectangle2.MaxBound()))
		return false;
	if (XMVectorGetY(rectangle1.MaxBound()) < XMVectorGetY(rectangle2.MinBound()) ||
		XMVectorGetY(rectangle1.MinBound()) > XMVectorGetY(rectangle2.MaxBound()))
		return false;

	return true;
}

template <typename T>
[[nodiscard]] constexpr bool Intersect(const Rectangle<T> rectangle, const Circle<T> circle) noexcept
{
	const auto center = circle.Center();

	if (rectangle.Contains(center)) return true;

	const auto minBound = rectangle.MinBound();
	const auto maxBound = rectangle.MaxBound();
	const auto radius = circle.Radius();

	const auto minBoundRect1 = XMVectorSubtract(minBound, XMVectorSet(radius, 0, 0, 0));
	const auto minBoundRect2 = XMVectorSubtract(minBound, XMVectorSet(0, radius, 0, 0));
	const auto maxBoundRect1 = XMVectorAdd(maxBound, XMVectorSet(radius, 0, 0, 0));
	const auto maxBoundRect2 = XMVectorAdd(maxBound, XMVectorSet(0, radius, 0, 0));

	// Added circle radius to rectangle bounds
	const Rectangle<T> rectangle1(minBoundRect1, maxBoundRect1);
	const Rectangle<T> rectangle2(minBoundRect2, maxBoundRect2);

	// Check rectangle bounds
	if (rectangle1.Contains(center)) return true;
	if (rectangle2.Contains(center)) return true;

	// Check circles at rectangle corners
	const auto corners = {
		minBound,
		maxBound,
		XMVectorSet(XMVectorGetX(minBound), XMVectorGetY(maxBound),0,0),
		XMVectorSet(XMVectorGetX(maxBound), XMVectorGetY(minBound),0,0)
	};

	for (const auto& corner : corners)
	{
		if (circle.Contains(corner)) return true;
	}

	return false;
}

template <typename T>
[[nodiscard]] constexpr bool Intersect(const Circle<T> circle, const Rectangle<T> rectangle) noexcept
{
	return Intersect(rectangle, circle);
}