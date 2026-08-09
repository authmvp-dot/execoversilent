#pragma once
#include <cstdint>
#include <string>

struct Vector3;

struct GroundLootEntry {
    uint32_t id = 0;
    std::string name;
    float x = 0.f, y = 0.f, z = 0.f;
};

enum class LootRarity {
    Common = 0,
    Uncommon,
    Rare,
    Epic,
    Legendary,
    Mythic
};
