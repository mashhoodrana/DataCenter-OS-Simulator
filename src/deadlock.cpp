// ============================================
// FILE: deadlock.cpp
// DESCRIPTION: Implementation of Banker's Algorithm
// ============================================

#include "../include/deadlock.hpp"
#include "../include/logger.hpp"
#include <algorithm>
#include <sstream>

DeadlockManager::DeadlockManager(ResourceManager* rm) 
    : resourceManager(rm), deadlocksDetected(0), deadlocksPrevented(0) {
    globalLogger->logEvent("DeadlockManager initialized with Banker's Algorithm");
}

bool DeadlockManager::requestResources(int jobId, const ResourceRequest& request) {
    // First check if resources are currently available
    if (!resourceManager->canAllocate(request)) {
        globalLogger->logJobEvent(jobId, 
            "Request denied - Insufficient resources available");
        return false;
    }
    
    // Check if allocation would lead to safe state (Banker's Algorithm)
    if (!isSafeState(request, jobId)) {
        deadlocksPrevented++;
        globalLogger->logJobEvent(jobId, 
            "Request denied by Banker's Algorithm - Would lead to UNSAFE state");
        globalLogger->logEvent("⚠️ Deadlock PREVENTED (Total: " + 
            std::to_string(deadlocksPrevented) + ")");
        return false;
    }
    
    // Safe to allocate
    bool allocated = resourceManager->allocateResources(jobId, request);
    
    if (allocated) {
        globalLogger->logJobEvent(jobId, 
            "✅ Banker's Algorithm verified - System remains in SAFE state");
    }
    
    return allocated;
}

bool DeadlockManager::isSafeState(const ResourceRequest& request, int jobId) {
    // Get current system state
    auto available = resourceManager->getAvailableResources();
    auto allocations = resourceManager->getAllocations();
    
    // Simulate allocation
    std::vector<int> availableVec = {
        available.cpuCores - request.cpuCores,
        available.ramGB - request.ramGB,
        available.diskSlots - request.diskSlots,
        available.networkSlots - request.networkSlots
    };
    
    // Check if any resource goes negative
    for (int res : availableVec) {
        if (res < 0) return false;
    }
    
    // Build allocation matrix
    std::vector<std::vector<int>> allocationMatrix;
    std::vector<std::vector<int>> maxNeedMatrix;
    std::vector<int> jobIds;
    
    // Add existing allocations
    for (const auto& alloc : allocations) {
        jobIds.push_back(alloc.jobId);
        allocationMatrix.push_back({
            alloc.allocated.cpuCores,
            alloc.allocated.ramGB,
            alloc.allocated.diskSlots,
            alloc.allocated.networkSlots
        });
        
        // Assume max need is 2x current allocation (simplified)
        maxNeedMatrix.push_back({
            alloc.allocated.cpuCores * 2,
            alloc.allocated.ramGB * 2,
            alloc.allocated.diskSlots * 2,
            alloc.allocated.networkSlots * 2
        });
    }
    
    // Add the new request
    bool jobExists = std::find(jobIds.begin(), jobIds.end(), jobId) != jobIds.end();
    if (!jobExists) {
        jobIds.push_back(jobId);
        allocationMatrix.push_back({
            request.cpuCores,
            request.ramGB,
            request.diskSlots,
            request.networkSlots
        });
        maxNeedMatrix.push_back({
            request.cpuCores * 2,
            request.ramGB * 2,
            request.diskSlots * 2,
            request.networkSlots * 2
        });
    }
    
    // Find safe sequence
    auto safeSeq = findSafeSequence(availableVec, allocationMatrix, maxNeedMatrix, jobIds);
    
    if (!safeSeq.empty()) {
        std::stringstream ss;
        ss << "Safe sequence found: [";
        for (size_t i = 0; i < safeSeq.size(); i++) {
            ss << safeSeq[i];
            if (i < safeSeq.size() - 1) ss << ", ";
        }
        ss << "]";
        globalLogger->logEvent(ss.str());
        return true;
    }
    
    return false;
}

std::vector<int> DeadlockManager::findSafeSequence(
    std::vector<int> available,
    std::vector<std::vector<int>>& allocation,
    std::vector<std::vector<int>>& maxNeed,
    std::vector<int>& jobIds
) {
    int n = jobIds.size();
    std::vector<bool> finished(n, false);
    std::vector<int> safeSequence;
    
    // Calculate need matrix
    std::vector<std::vector<int>> need(n, std::vector<int>(4));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < 4; j++) {
            need[i][j] = maxNeed[i][j] - allocation[i][j];
            if (need[i][j] < 0) need[i][j] = 0; // Prevent negative
        }
    }
    
    // Try to find safe sequence
    int count = 0;
    while (count < n) {
        bool found = false;
        
        for (int i = 0; i < n; i++) {
            if (finished[i]) continue;
            
            // Check if need[i] <= available
            bool canFinish = true;
            for (int j = 0; j < 4; j++) {
                if (need[i][j] > available[j]) {
                    canFinish = false;
                    break;
                }
            }
            
            if (canFinish) {
                // Add allocated resources back to available
                for (int j = 0; j < 4; j++) {
                    available[j] += allocation[i][j];
                }
                
                safeSequence.push_back(jobIds[i]);
                finished[i] = true;
                found = true;
                count++;
            }
        }
        
        if (!found) {
            // No safe sequence exists
            return std::vector<int>();
        }
    }
    
    return safeSequence;
}

std::string DeadlockManager::getSafetyReport() {
    std::stringstream ss;
    ss << "\n========== DEADLOCK MANAGEMENT REPORT ==========\n";
    ss << "Deadlocks Detected: " << deadlocksDetected << "\n";
    ss << "Deadlocks Prevented: " << deadlocksPrevented << "\n";
    ss << "Method: Banker's Algorithm\n";
    ss << "===============================================\n";
    return ss.str();
}