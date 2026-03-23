#include "Physics.hpp"
#include <cmath>

void Physics::UpdatePhysics(Player& player, const std::vector<Tile*>& floor, float dt) {
    // reset the grounded state at the start 
    player.grounded = false;

    // Apply Constant Gravity
    player.vel.y -= 15.0f * dt;


    // Collision Detection Loop
    for (auto tile : floor) {
        if (tile->isHole) continue; 

        float halfTileWidth = tile->size.x / 2.0f;
        float halfTileDepth = tile->size.z / 2.0f;

        // Check bounds 
        // added margins to the bounds to get rid of the cube phasing through the floor gaps 
        if (player.pos.x < tile->pos.x + halfTileWidth + 0.2f &&
            player.pos.x > tile->pos.x - halfTileWidth - 0.2f &&
            player.pos.z < tile->pos.z + halfTileDepth + 0.2f &&
            player.pos.z > tile->pos.z - halfTileDepth - 0.2f) {
            
            // If robot is roughly at the tile height and falling
            if (player.pos.y <= tile->pos.y + 0.5f && player.pos.y > tile->pos.y - 0.2f) {
                player.pos.y = tile->pos.y + 0.5f;
                player.vel.y = 0;
                player.grounded = true;
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