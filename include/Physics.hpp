#pragma once
#include "Entity.hpp"
#include <vector>

class Physics {
public:
    static void UpdatePhysics(Player& player, const std::vector<Tile*>& floor, float dt);
};