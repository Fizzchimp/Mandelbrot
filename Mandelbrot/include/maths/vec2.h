#pragma once

template<typename T>
class vec2
{
private:
	T v[2];


public:
	T& x = v[0];
	T& y = v[1];

	// Constructors
	vec2(T x = 0, T y = 0)
	{
		v[0] = x, v[1] = y;
	}

	// Type casting
	operator const T* () const
	{
		return v;
	}
};