#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

#include <gui/model/Model.hpp>

class ModelListener
{
public:
    ModelListener() : model(0) {}
    
    virtual ~ModelListener() {}

    void bind(Model* m)
    {
        model = m;
    }
    
    // UI Notifications
    virtual void notifyGameOver() {}
    virtual void notifyScoreUpdated(int newScore) {}
    virtual void notifyGridShifted() {}
    virtual void notifyTurnEnd() {}
    
    // Feature 4: Special Events Hook System
    virtual void notifyOnMatch(int matchCount, int combo) {}
    virtual void notifyOnDrop(int dropCount) {}
    virtual void notifyOnShoot() {}
    
protected:
    Model* model;
};

#endif // MODELLISTENER_HPP
