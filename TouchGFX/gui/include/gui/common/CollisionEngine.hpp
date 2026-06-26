#ifndef COLLISIONENGINE_HPP
#define COLLISIONENGINE_HPP

class CollisionEngine {
public:
    static float distanceSquared(float x1, float y1, float x2, float y2);
    static float getWallPenalty(float x, float leftWall, float rightWall, float penaltyMin, float penaltyMax, float wallDist);
};

#endif // COLLISIONENGINE_HPP
