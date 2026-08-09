#pragma once
#include <string>

struct Vector3 {
    float x, y, z;
    static Vector3 Zero() { return { 0.f, 0.f, 0.f }; }
    Vector3 operator+(const Vector3& o) const { return { x + o.x, y + o.y, z + o.z }; }
    Vector3 operator-(const Vector3& o) const { return { x - o.x, y - o.y, z - o.z }; }
    Vector3 operator*(float s) const { return { x * s, y * s, z * s }; }
};

struct Player {
    int id = 0;
    std::string name;
    Vector3 position;
    Vector3 headPosition;
    float health = 100.f;
    bool isVisible = false;
    bool isDown = false;
    int team = 0;
};
