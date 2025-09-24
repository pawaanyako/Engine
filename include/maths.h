#ifndef MATHS_H_
#define MATHS_H_

#include <cmath>
#include <iostream>

template <typename T>
class vec2 {
public:
    T x, y;

    vec2() : x(T()), y(T()) {}

    vec2(T x, T y) : x(x), y(y) {}

    vec2(const vec2& other) : x(other.x), y(other.y) {}

    vec2& operator=(const vec2& other) {
        if (this != &other) {
            x = other.x;
            y = other.y;
        }
        return *this;
    }

    vec2 operator+(const vec2& other) const {
        return vec2(x + other.x, y + other.y);
    }

    vec2 operator-(const vec2& other) const {
        return vec2(x - other.x, y - other.y);
    }

    vec2 operator*(T scalar) const {
        return vec2(x * scalar, y * scalar);
    }

    vec2 operator/(T scalar) const {
        assert(scalar != T(0) && "Division by zero in vec2");
        return vec2(x / scalar, y / scalar);
    }

    T dot(const vec2& other) const {
        return x * other.x + y * other.y;
    }

    T length() const {
        return std::sqrt(x * x + y * y);
    }

    T lengthSquared() const {
        return x * x + y * y;
    }

    vec2 normalized() const {
        T len = length();
        if (len == T(0))
            return vec2(T(0), T(0));
        return *this / len;
    }

    T cross(const vec2& other) const {
        return (x * other.y) - (y * other.x);
    }

    friend std::ostream& operator<<(std::ostream& os, const vec2& v) {
        os << "(" << v.x << ", " << v.y << ")";
        return os;
    }
};

template <typename T>
vec2<T> operator*(T scalar, const vec2<T>& vec) {
    return vec * scalar;
}

template <typename T>
class vec3 {
public:
    T x, y, z;

    vec3() : x(T()), y(T()), z(T()) {}

    vec3(T x, T y, T z) : x(x), y(y), z(z) {}

    vec3(const vec3& other) : x(other.x), y(other.y), z(other.z) {}

    vec3& operator=(const vec3& other) {
        if (this != &other) {
            x = other.x;
            y = other.y;
            z = other.z;
        }
        return *this;
    }

    vec3 operator+(const vec3& other) const {
        return vec3(x + other.x, y + other.y, z + other.z);
    }

    vec3 operator-(const vec3& other) const {
        return vec3(x - other.x, y - other.y, z - other.z);
    }

    vec3 operator*(T scalar) const {
        return vec3(x * scalar, y * scalar, z * scalar);
    }

    vec3 operator/(T scalar) const {
        assert(scalar != T(0) && "Division by zero in vec3");
        return vec3(x / scalar, y / scalar, z / scalar);
    }

    T dot(const vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    T length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    T lengthSquared() const {
        return x * x + y * y + z * z;
    }

    vec3 normalized() const {
        T len = length();
        if (len == T(0)) {
            return vec3(T(0), T(0), T(0));
        }
        return *this / len;
    }

    vec3 cross(const vec3& other) const {
        return vec3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
    }

    friend std::ostream& operator<<(std::ostream& os, const vec3& v) {
        os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return os;
    }
};

template <typename T>
vec3<T> operator*(T scalar, const vec3<T>& vec) {
    return vec * scalar;
}

#endif
