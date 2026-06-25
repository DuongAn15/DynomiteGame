#ifndef MOCK_MODEL_LISTENER_HPP
#define MOCK_MODEL_LISTENER_HPP

#include <gui/model/ModelListener.hpp>

class MockModelListener : public ModelListener {
public:
    bool isGameOverCalled;
    bool isScoreUpdatedCalled;
    bool isGridShiftedCalled;
    int lastScoreUpdated;

    MockModelListener() {
        reset();
    }

    void reset() {
        isGameOverCalled = false;
        isScoreUpdatedCalled = false;
        isGridShiftedCalled = false;
        lastScoreUpdated = -1;
    }

    void notifyGameOver() override {
        isGameOverCalled = true;
    }

    void notifyScoreUpdated(int newScore) override {
        isScoreUpdatedCalled = true;
        lastScoreUpdated = newScore;
    }

    void notifyGridShifted() override {
        isGridShiftedCalled = true;
    }
};

#endif // MOCK_MODEL_LISTENER_HPP
