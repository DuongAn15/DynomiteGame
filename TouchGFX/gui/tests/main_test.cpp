#include "test_utils.hpp"
#include <iostream>

extern void run_GridMath_Tests();
extern void run_Collision_Tests();
extern void run_MatchLogic_Tests();
extern void run_FloatingEggs_Tests();
extern void run_GameOver_Tests();
extern void run_PredictiveRaycast_Tests();
extern void run_InfiniteMode_Tests();

int g_test_passed = 0;
int g_test_failed = 0;

int main() {
    std::cout << "--- RUNNING TDD TESTS ---" << std::endl;
    run_GridMath_Tests();
    run_Collision_Tests();
    run_MatchLogic_Tests();
    run_FloatingEggs_Tests();
    run_GameOver_Tests();
    run_PredictiveRaycast_Tests();
    run_InfiniteMode_Tests();
    
    std::cout << "\nResults:" << std::endl;
    std::cout << "Passed: " << g_test_passed << std::endl;
    std::cout << "Failed: " << g_test_failed << std::endl;
    
    return (g_test_failed > 0) ? 1 : 0;
}
