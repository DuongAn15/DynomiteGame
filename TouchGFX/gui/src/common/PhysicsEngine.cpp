#include <gui/common/PhysicsEngine.hpp>
#include <math.h>

void PhysicsEngine::computeVelocity(float dx, float dy, float speed, float& vx, float& vy) {
    float length = sqrtf(dx * dx + dy * dy);
    vx = (dx / length) * speed;
    vy = (dy / length) * speed;
}

void PhysicsEngine::updatePosition(float& x, float& y, float vx, float vy) {
    x += vx;
    y += vy;
}

void PhysicsEngine::resolveReflection(float& x, float& vx, float leftWall, float rightWall) {
    if (x <= leftWall && vx < 0) {
        x = leftWall + (leftWall - x);
        vx = -vx;
    } else if (x >= rightWall && vx > 0) {
        x = rightWall - (x - rightWall);
        vx = -vx;
    }
}
