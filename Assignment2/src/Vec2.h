﻿#pragma once

class Vec2
{
public:
    float x = 0, y = 0;

    Vec2();
    Vec2(float xin, float yin);

    bool operator ==(const Vec2& rhs) const;
    bool operator !=(const Vec2& rhs) const;

    Vec2 operator +(const Vec2& rhs) const;
    Vec2 operator -(const Vec2& rhs) const;
    Vec2 operator *(float val) const;
    Vec2 operator /(float val) const;

    void operator +=(const Vec2& rhs);
    void operator -=(const Vec2& rhs);
    void operator *=(float val);
    void operator /=(float val);

    float distanceSquare(const Vec2& vin) const;
};
