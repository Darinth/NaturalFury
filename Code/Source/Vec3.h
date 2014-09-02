// Name:
// Vec3.h
// Description:
// Header file for Vec3.
// Notes:
// OS-Unaware
// A vector, of the mathematical type.

#ifndef SPATIALVECTOR_H
#define SPATIALVECTOR_H

#include <exception>
using namespace std;

class InvalidLinearizationException : public exception
{
	virtual const char* what() const throw()
	{
		return "Attempt to linearize coordinates outside of given dimensions";
	}
};


template <class xType, class yType = xType, class zType = yType>
struct Vec3
{

	Vec3(){};
	Vec3(xType x, yType y, zType z) : x(x), y(y), z(z){};

	xType x;
	yType y;
	zType z;

	xType linearize(const Vec3<xType, yType, zType>& dimensions) const
	{
		if (this->x >= dimensions.x || this->y >= dimensions.y || this->z >= dimensions.z ||
			this->x < 0 || this->y < 0 || this->z < 0) throw InvalidLinearizationException();
		return(this->x + this->y*dimensions.x + this->z*dimensions.x*dimensions.y);
	}

	bool operator==(const Vec3<xType, yType, zType>& dimensions) const
	{
		SpatialVector<xType, yType, zType> returnVector = *this;
		if (this->x != dimensions.x || this->y != dimensions.y || this->z != dimensions.z)
			return false;
		return true;
	}

	bool operator!=(const Vec3<xType, yType, zType>& dimensions) const
	{
		Vec3<xType, yType, zType> returnVector = *this;
		if (this->x != dimensions.x || this->y != dimensions.y || this->z != dimensions.z)
			return true;
		return false;
	}

	Vec3<xType, yType, zType> operator+(const Vec3<xType, yType, zType>& dimensions) const
	{
		Vec3<xType, yType, zType> returnVector = *this;
		returnVector.x += dimensions.x;
		returnVector.y += dimensions.y;
		returnVector.z += dimensions.z;
		return returnVector;
	}

	Vec3<xType, yType, zType>& operator+=(const Vec3<xType, yType, zType>& modifier)
	{
		x += modifier.x;
		y += modifier.y;
		z += modifier.z;
		return *this;
	}

	Vec3<xType, yType, zType> operator-(const Vec3<xType, yType, zType>& dimensions) const
	{
		SpatialVector<xType, yType, zType> returnVector = *this;
		returnVector.x -= dimensions.x;
		returnVector.y -= dimensions.y;
		returnVector.z -= dimensions.z;
		return returnVector;
	}

	Vec3<xType, yType, zType>& operator-=(const Vec3<xType, yType, zType>& modifier)
	{
		x -= modifier.x;
		y -= modifier.y;
		z -= modifier.z;
		return *this;
	}

	Vec3<xType, yType, zType> operator/(const Vec3<xType, yType, zType>& dimensions) const
	{
		Vec3<xType, yType, zType> returnVector = *this;
		returnVector.x /= dimensions.x;
		returnVector.y /= dimensions.y;
		returnVector.z /= dimensions.z;
		return returnVector;
	}

	Vec3<xType, yType, zType>& operator/=(const Vec3<xType, yType, zType>& modifier)
	{
		x /= modifier.x;
		y /= modifier.y;
		z /= modifier.z;
		return *this;
	}

	Vec3<xType, yType, zType> operator*(const double modifier) const
	{
		Vec3<xType, yType, zType> returnVector = *this;
		returnVector.x *= (xType)modifier;
		returnVector.y *= (yType)modifier;
		returnVector.z *= (zType)modifier;
		return returnVector;
	}

	Vec3<xType, yType, zType>& operator*=(const double modifier)
	{
		x *= modifier;
		y *= modifier;
		z *= modifier;
		return *this;
	}

	Vec3<xType, yType, zType> operator/(const xType modifier) const
	{
		Vec3<xType, yType, zType> returnVector = *this;
		returnVector.x /= modifier;
		returnVector.y /= modifier;
		returnVector.z /= modifier;
		return returnVector;
	}

	Vec3<xType, yType, zType>& operator/=(const xType modifier)
	{
		x /= modifier;
		y /= modifier;
		z /= modifier;
		return *this;
	}

	Vec3<xType, yType, zType> wrap(const Vec3<xType, yType, zType>& dimensions) const
	{
		Vec3<xType, yType, zType> returnVector = *this;
		if (returnVector.x >= 0)
			returnVector.x %= dimensions.x;
		else
			returnVector.x = (dimensions.x + (returnVector.x + 1) % dimensions.x) - 1;

		if (returnVector.y >= 0)
			returnVector.y %= dimensions.y;
		else
			returnVector.y = (dimensions.y + (returnVector.y + 1) % dimensions.y) - 1;

		if (returnVector.z >= 0)
			returnVector.z %= dimensions.z;
		else
			returnVector.z = (dimensions.z + (returnVector.z + 1) % dimensions.z) - 1;
		return returnVector;
	}

	Vec3<xType, yType, zType>& wrapSelf(const Vec3<xType, yType, zType>& dimensions)
	{
		this->x %= dimensions.x;
		this->y %= dimensions.y;
		this->z %= dimensions.z;
		if (this->x >= 0)
			this->x %= dimensions.x;
		else
			this->x = (dimensions.x + (this->x + 1) % dimensions.x) - 1;

		if (this->y >= 0)
			this->y %= dimensions.y;
		else
			this->y = (dimensions.y + (this->y + 1) % dimensions.y) - 1;

		if (this->z >= 0)
			this->z %= dimensions.z;
		else
			this->z = (dimensions.z + (this->z + 1) % dimensions.z) - 1;
		return *this;
	}

	double distance()
	{
		return sqrt(x*x + y*y + z*z);
	}

	double distanceXY()
	{
		return sqrt(x*x + y*y);
	}

	Vec3<xType, yType, zType> normalize()
	{
		Vec3<xType, yType, zType> returnVector = *this;
		return returnVector / returnVector.distance();
	}

	Vec3<xType, yType, zType> normalizeSelf()
	{
		*this /= distance();
		return *this;
	}

	double dot(Vec3<xType, yType, zType> otherVector)
	{
		return x*otherVector.x + y*otherVector.y + z*otherVector.z;
	}

	Vec3<xType, yType, zType> rotateZ(double radians)
	{
		double xOrig = x;
		double yOrig = y;
		x = xOrig*cos(radians) - yOrig*sin(radians);
		y = xOrig*sin(radians) + yOrig*cos(radians);
		return *this;
	}

	Vec3<xType, yType, zType> rotateY(double radians)
	{
		double xOrig = x;
		double zOrig = z;
		x = xOrig*cos(radians) + zOrig*sin(radians);
		z = -xOrig*sin(radians) + zOrig*cos(radians);
		return *this;
	}

	Vec3<xType, yType, zType> rotateX(double radians)
	{
		double yOrig = y;
		double zOrig = z;
		y = yOrig*cos(radians) - zOrig*sin(radians);
		z = yOrig*sin(radians) + zOrig*cos(radians);
		return *this;
	}

	//Returns a spatialVector that is a unit vector in the direction of the greatest component
	Vec3<xType, yType, zType> getCardinal()
	{
		SpatialVector<xType, yType, zType> returnVector(0.0, 0.0, 0.0);
		xType tempX = x > 0 ? x : -x;
		yType tempY = y > 0 ? y : -y;
		zType tempZ = z > 0 ? z : -z;

		if (tempX >= tempY && tempX >= tempZ)
			returnVector.x = (xType)(x >= 0 ? 1.0 : -1.0);
		else if (tempY >= tempZ)
			returnVector.y = (yType)(y >= 0 ? 1.0 : -1.0);
		else
			returnVector.z = (zType)(z >= 0 ? 1.0 : -1.0);
		return returnVector;
	}
};

#endif