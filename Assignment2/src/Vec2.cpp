#include "Vec2.h"
#include <cmath>
#include <limits>

Vec2::Vec2()
{
}

Vec2::Vec2(const float xin, const float yin): x(xin), y(yin)
{
}

bool Vec2::operator ==(const Vec2& rhs) const
{
    return
        std::fabs(x - rhs.x) <= std::numeric_limits<float>::epsilon() &&
        std::fabs(y - rhs.y) <= std::numeric_limits<float>::epsilon();
}

bool Vec2::operator !=(const Vec2& rhs) const
{
    return
        std::fabs(x - rhs.x) > std::numeric_limits<float>::epsilon() ||
        std::fabs(y - rhs.y) > std::numeric_limits<float>::epsilon();
}

Vec2 Vec2::operator +(const Vec2& rhs) const
{
    return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator -(const Vec2& rhs) const
{
    return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator *(const float val) const
{
    return Vec2(x * val, y * val);
}

Vec2 Vec2::operator /(const float val) const
{
    return Vec2(x / val, y / val);
}

void Vec2::operator +=(const Vec2& rhs)
{
    x += rhs.x;
    y += rhs.y;
}

void Vec2::operator -=(const Vec2& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
}

void Vec2::operator *=(const float val)
{
    x *= val;
    y *= val;
}

void Vec2::operator /=(const float val)
{
    x /= val;
    y /= val;
}

float Vec2::distanceSquare(const Vec2& vin) const
{
    return static_cast<float>(std::pow(vin.x - x, 2)) + static_cast<float>(std::pow(vin.y - y, 2));
}
