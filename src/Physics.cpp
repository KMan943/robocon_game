#include "Physics.hpp"
#include <cmath>

void Physics::UpdatePhysics(Player& player, const std::vector<Tile*>& floor, float dt) {
    // reset the grounded state at the start 
    player.grounded = false;

    // Apply Constant Gravity
    player.vel.y -= 10.0f * dt;


    // Collision Detection Loop
    for (auto tile : floor) {
        if (tile->isHole) continue; 

        float halfTileWidth = tile->size.x / 2.0f;
        float halfTileDepth = tile->size.z / 2.0f;

        // Check bounds 
        // reduced margins to allow falling into single-tile holes
        if (player.pos.x < tile->pos.x + halfTileWidth + 0.05f &&
            player.pos.x > tile->pos.x - halfTileWidth - 0.05f &&
            player.pos.z < tile->pos.z + halfTileDepth + 0.05f &&
            player.pos.z > tile->pos.z - halfTileDepth - 0.05f) {
            
            // If robot is roughly at the tile height and falling
            if (player.pos.y <= tile->pos.y + 0.55f && player.pos.y > tile->pos.y - 0.5f) {
                player.pos.y = tile->pos.y + 0.55f;
                player.vel.y = 0;
                player.grounded = true;
                player.pos.x += tile->vel.x * dt;
                player.pos.z += tile->vel.z * dt;
            }
        }
    }
}

bool Physics::CheckCoinCollision(const Player& player, const Coin& coin) {
    if (!coin.isActive) return false;

    // Simple AABB collision
    float playerHalfWidth = player.size.x / 2.0f;
    float playerHalfHeight = player.size.y / 2.0f;
    float playerHalfDepth = player.size.z / 2.0f;

    float coinHalfWidth = coin.size.x / 2.0f;
    float coinHalfHeight = coin.size.y / 2.0f;
    float coinHalfDepth = coin.size.z / 2.0f;

    bool collisionX = player.pos.x + playerHalfWidth >= coin.pos.x - coinHalfWidth &&
                      coin.pos.x + coinHalfWidth >= player.pos.x - playerHalfWidth;

    bool collisionY = player.pos.y + playerHalfHeight >= coin.pos.y - coinHalfHeight &&
                      coin.pos.y + coinHalfHeight >= player.pos.y - playerHalfHeight;

    bool collisionZ = player.pos.z + playerHalfDepth >= coin.pos.z - coinHalfDepth &&
                      coin.pos.z + coinHalfDepth >= player.pos.z - playerHalfDepth;

    return collisionX && collisionY && collisionZ;
}

bool Physics::CheckObstacleCollision(const Player& player, const Obstacle& obstacle) {
    float playerHalfWidth = player.size.x / 2.0f;
    float playerHalfHeight = player.size.y / 2.0f;
    float playerHalfDepth = player.size.z / 2.0f;

    float obsHalfWidth = obstacle.size.x / 2.0f;
    float obsHalfHeight = obstacle.size.y / 2.0f;
    float obsHalfDepth = obstacle.size.z / 2.0f;

    bool collisionX = player.pos.x + playerHalfWidth >= obstacle.pos.x - obsHalfWidth &&
                      obstacle.pos.x + obsHalfWidth >= player.pos.x - playerHalfWidth;

    bool collisionY = player.pos.y + playerHalfHeight >= obstacle.pos.y - obsHalfHeight &&
                      obstacle.pos.y + obsHalfHeight >= player.pos.y - playerHalfHeight;

    bool collisionZ = player.pos.z + playerHalfDepth >= obstacle.pos.z - obsHalfDepth &&
                      obstacle.pos.z + obsHalfDepth >= player.pos.z - playerHalfDepth;

    return collisionX && collisionY && collisionZ;
}

bool Physics::CheckGoalCollision(const Player& player, const Goal& goal) {
    float playerHalfWidth = player.size.x / 2.0f;
    float playerHalfHeight = player.size.y / 2.0f;
    float playerHalfDepth = player.size.z / 2.0f;

    float goalHalfWidth = goal.size.x / 2.0f;
    float goalHalfHeight = goal.size.y / 2.0f;
    float goalHalfDepth = goal.size.z / 2.0f;

    bool collisionX = player.pos.x + playerHalfWidth >= goal.pos.x - goalHalfWidth &&
                      goal.pos.x + goalHalfWidth >= player.pos.x - playerHalfWidth;

    bool collisionY = player.pos.y + playerHalfHeight >= goal.pos.y - goalHalfHeight &&
                      goal.pos.y + goalHalfHeight >= player.pos.y - playerHalfHeight;

    bool collisionZ = player.pos.z + playerHalfDepth >= goal.pos.z - goalHalfDepth &&
                      goal.pos.z + goalHalfDepth >= player.pos.z - playerHalfDepth;

    return collisionX && collisionY && collisionZ;
}