#include "Physics.hpp"
#include <cmath>

// Notice the Physics:: prefix here
void Physics::UpdatePhysics(Player& player, const std::vector<Tile*>& floor, float dt) {
    // 1. Apply Constant Gravity
    player.vel.y -= 15.0f * dt;
    player.pos += player.vel * dt;

    // 2. Collision Detection Loop
    for (auto tile : floor) {
        if (tile->isHole) continue; 

        float halfTileWidth = tile->size.x / 2.0f;
        float halfTileDepth = tile->size.z / 2.0f;

        // Check bounds
        if (player.pos.x < tile->pos.x + halfTileWidth &&
            player.pos.x > tile->pos.x - halfTileWidth &&
            player.pos.z < tile->pos.z + halfTileDepth &&
            player.pos.z > tile->pos.z - halfTileDepth) {
            
            // Floor collision logic
            if (player.pos.y <= tile->pos.y + 0.5f && player.pos.y > tile->pos.y) {
                player.pos.y = tile->pos.y + 0.5f;
                player.vel.y = 0;
            }
        }
    }
}