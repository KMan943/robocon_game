#pragma once
#include "Entity.hpp"
#include <vector>

class Physics {
public:
    static void UpdatePhysics(Player& player, const std::vector<Tile*>& floor, float dt);
    static bool CheckCoinCollision(const Player& player, const Coin& coin);
    static bool CheckObstacleCollision(const Player& player, const Obstacle& obstacle);
    static bool CheckGoalCollision(const Player& player, const Goal& goal);
};