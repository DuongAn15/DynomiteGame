#include <gui/common/CollisionEngine.hpp>

float CollisionEngine::distanceSquared(float x1, float y1, float x2, float y2) {
    return (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
}

float CollisionEngine::getWallPenalty(float x, float leftWall, float rightWall, float penaltyMin, float penaltyMax, float wallDist) {
    float penaltyX = penaltyMax;
    float distToLeft = x - leftWall;
    float distToRight = rightWall - x;
    float minDistToWall = (distToLeft < distToRight) ? distToLeft : distToRight;
    
    if (minDistToWall < wallDist) {
        if (minDistToWall < 0.0f) minDistToWall = 0.0f;
        penaltyX = penaltyMin + (penaltyMax - penaltyMin) * (minDistToWall / wallDist);
    }
    
    return penaltyX;
}
