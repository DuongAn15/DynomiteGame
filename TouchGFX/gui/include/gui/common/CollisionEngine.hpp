#ifndef COLLISIONENGINE_HPP
#define COLLISIONENGINE_HPP

#include <stdint.h>

class CollisionEngine {
public:
    static bool computeCollisionAt(
        float x, float y,
        const uint8_t* grid,
        int headRowIndex,
        int gridParityOffset,
        float globalOffsetY);

    static void resolveSnapToGrid(
        float px, float py,
        int& outCol, int& outRow,
        const uint8_t* grid,
        int headRowIndex,
        int gridParityOffset,
        float globalOffsetY);

    static float computeDistanceSquared(float x1, float y1, float x2, float y2);
    static float computeWallPenalty(float x, float leftWall, float rightWall, float penaltyMin, float penaltyMax, float wallDist);
};

#endif // COLLISIONENGINE_HPP
