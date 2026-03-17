#pragma once
#include "Entity.hpp"
#include <vector>

class Physics {
public:
    static void UpdatePhysics(Player& player, const std::vector<Tile*>& floor, float dt);
    static bool CheckCoinCollision(const Player& player, const Coin& coin);
};