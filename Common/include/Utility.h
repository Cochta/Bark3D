#pragma once

constexpr double PI = 3.141592653589793238462643383279502884197;

template<typename T>
[[nodiscard]] constexpr T Clamp(T x, T min, T max) noexcept
{
	if (min > max)
	{
		T tmp = min;
		min = max;
		max = tmp;
	}

	if (x < min)
	{
		x = min;
	}
	else if (x > max)
	{
		x = max;
	}

	return x;
}

template<typename T>
[[nodiscard]] constexpr T Abs(T nbr) noexcept
{
	return nbr < 0 ? -nbr : nbr;
}

template<typename T>
[[nodiscard]] constexpr T Min(const T x, const T y) noexcept
{
	return x < y ? x : y;
}

template<typename T>
[[nodiscard]] constexpr T Max(const T x, const T y) noexcept
{
	return x > y ? x : y;
}
template<typename T>
[[nodiscard]] constexpr T Fact(const T n) noexcept
{
	T result = 1;

	for (int i = 2; i <= Abs(n); i++)
	{
		result *= i;
	}

	if (n < 0)
	{
		result = -result;
	}

	return result;
}

template<typename T>
[[nodiscard]] constexpr T Pow(const T x, const unsigned int power)
{
	if (x == 0)
	{
		return 0;
	}

	T result = 1;

	for (int i = 1; i <= power; i++)
	{
		result *= x;
	}

	return result;
}


template<typename T>
[[nodiscard]] T Poly6Kernel(T r, T h)
{
	if (r > h) return 0.0f;
	T factor = (315.0f / (64.0f * PI * pow(h, 9)));
	return factor * pow((h * h - r * r), 3);
}
template<typename T>
[[nodiscard]] T SpikyGradient(T r, T h)
{
	if (r > h || r == 0) return 0.0f;
	float factor = (-45.0f / (PI * pow(h, 6)));
	return factor * pow((h - r), 2);
}
template<typename T>
[[nodiscard]] T ViscosityLaplacian(T r, T h)
{
	if (r > h) return 0.0f;
	T factor = (45.0f / (PI * pow(h, 6)));
	return factor * (h - r);
}
template<typename T>
[[nodiscard]] T ComputePressure(T density)
{
	const T restDensity = 1000.0f; // Water density
	const T stiffness = 2000.0f; // Tuning parameter
	return stiffness * (density - restDensity);
}