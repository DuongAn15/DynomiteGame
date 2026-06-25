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
    
protected:
    Model* model;
};

#endif // MODELLISTENER_HPP
