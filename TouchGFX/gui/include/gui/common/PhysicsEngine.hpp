#ifndef PHYSICSENGINE_HPP
#define PHYSICSENGINE_HPP

class PhysicsEngine {
public:
    static void computeVelocity(float dx, float dy, float speed, float& vx, float& vy);
    static void updatePosition(float& x, float& y, float vx, float vy);
    static void resolveReflection(float& x, float& vx, float leftWall, float rightWall);
};

#endif // PHYSICSENGINE_HPP
