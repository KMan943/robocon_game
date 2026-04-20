#include "Physics.hpp"
#include <cmath>

void Physics::UpdatePhysics(Player& player, const std::vector<Tile*>& floor, float dt) {
    // reset the grounded state at the start 
    player.grounded = false;

    // Apply Constant Gravity
    player.vel.y -= 22.5f * dt;

    // Collision Detection Loop
    for (auto tile : floor) {
        if (tile->isHole) continue; 

        float halfTileWidth = tile->size.x / 2.0f;
        float halfTileDepth = tile->size.z / 2.0f;

        // Check bounds 
        // added margins to the bounds to get rid of the cube phasing through the floor gaps 
        if (player.pos.x < tile->pos.x + halfTileWidth + 0.1f &&
            player.pos.x > tile->pos.x - halfTileWidth - 0.1f &&
            player.pos.z < tile->pos.z + halfTileDepth + 0.1f &&
            player.pos.z > tile->pos.z - halfTileDepth - 0.1f) {
            
            // If robot is roughly at the tile height and falling
            if (player.pos.y <= tile->pos.y + 0.5f && player.pos.y > tile->pos.y - 0.2f) {
                player.pos.y = tile->pos.y + 0.5f;
                player.vel.y = 0;
                player.grounded = true;
            }
        }
    }
}