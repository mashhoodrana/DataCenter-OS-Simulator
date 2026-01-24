// ============================================
// FILE: resource_manager.hpp
// DESCRIPTION: Manages shared resources (CPU, RAM, Disk, Network)
// ============================================

#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <mutex>
#include <condition_variable>
#include <vector>

struct ResourceRequest {
    int cpuCores;
    int ramGB;
    int diskSlots;
    int networkSlots;
    
    ResourceRequest(int cpu = 0, int ram = 0, int disk = 0, int network = 0)
        : cpuCores(cpu), ramGB(ram), diskSlots(disk), networkSlots(network) {}
};

class ResourceManager {
private:
    // Total resources available
    int totalCPU;
    int totalRAM;
    int totalDisk;
    int totalNetwork;
    
    // Currently available resources
    int availableCPU;
    int availableRAM;
    int availableDisk;
    int availableNetwork;
    
    // Synchronization primitives
    std::mutex resourceMutex;
    std::condition_variable resourceCV;
    
    // Track resource allocation per job
    struct Allocation {
        int jobId;
        ResourceRequest allocated;
    };
    std::vector<Allocation> allocations;

public:
    ResourceManager(int cpu, int ram, int disk, int network);
    
    // Resource allocation methods
    bool canAllocate(const ResourceRequest& req);
    bool allocateResources(int jobId, const ResourceRequest& req);
    void releaseResources(int jobId);
    
    // Query methods
    ResourceRequest getAvailableResources();
    ResourceRequest getTotalResources();
    double getCPUUtilization();
    double getRAMUtilization();
    double getDiskUtilization();
    double getNetworkUtilization();
    
    // For Banker's Algorithm
    std::vector<Allocation> getAllocations();
};

#endif 