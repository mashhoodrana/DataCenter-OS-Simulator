// ============================================
// FILE: deadlock.hpp
// DESCRIPTION: Banker's Algorithm for deadlock prevention
// ============================================

#ifndef DEADLOCK_HPP
#define DEADLOCK_HPP

#include "resource_manager.hpp"
#include <vector>
#include <string>

class DeadlockManager {
private:
    ResourceManager* resourceManager;
    
    // Statistics
    int deadlocksDetected;
    int deadlocksPrevented;
    
    // Helper method to check if system is in safe state
    bool isSafeState(const ResourceRequest& request, int jobId);
    
    // Helper to find safe sequence
    std::vector<int> findSafeSequence(
        std::vector<int> available,
        std::vector<std::vector<int>>& allocation,
        std::vector<std::vector<int>>& maxNeed,
        std::vector<int>& jobIds
    );

public:
    DeadlockManager(ResourceManager* rm);
    
    // Main Banker's Algorithm method
    bool requestResources(int jobId, const ResourceRequest& request);
    
    // Statistics methods
    int getDeadlocksDetected() const { return deadlocksDetected; }
    int getDeadlocksPrevented() const { return deadlocksPrevented; }
    
    // Utility
    std::string getSafetyReport();
};

#endif 