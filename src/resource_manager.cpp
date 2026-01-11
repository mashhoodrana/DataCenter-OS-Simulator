// ============================================
// FILE: resource_manager.cpp
// DESCRIPTION: Implementation of resource management
// ============================================

#include "../include/resource_manager.hpp"
#include "../include/logger.hpp"
#include <algorithm>

ResourceManager::ResourceManager(int cpu, int ram, int disk, int network)
    : totalCPU(cpu), totalRAM(ram), totalDisk(disk), totalNetwork(network),
      availableCPU(cpu), availableRAM(ram), availableDisk(disk), availableNetwork(network) {
    
    globalLogger->logEvent("ResourceManager initialized: CPU=" +
        std::to_string(cpu) + " cores, RAM=" + std::to_string(ram) + 
        "GB, Disk=" + std::to_string(disk) + " slots, Network=" + 
        std::to_string(network) + " slots");
}

bool ResourceManager::canAllocate(const ResourceRequest& req) {
    std::lock_guard<std::mutex> lock(resourceMutex);
    
    return (req.cpuCores <= availableCPU &&
            req.ramGB <= availableRAM &&
            req.diskSlots <= availableDisk &&
            req.networkSlots <= availableNetwork);
}

bool ResourceManager::allocateResources(int jobId, const ResourceRequest& req) {
    std::lock_guard<std::mutex> lock(resourceMutex);
    
    // Check if resources are available
    if (req.cpuCores > availableCPU || req.ramGB > availableRAM ||
        req.diskSlots > availableDisk || req.networkSlots > availableNetwork) {
        return false;
    }
    
    // Allocate resources
    availableCPU -= req.cpuCores;
    availableRAM -= req.ramGB;
    availableDisk -= req.diskSlots;
    availableNetwork -= req.networkSlots;
    
    // Record allocation
    allocations.push_back({jobId, req});
    
    globalLogger->logJobEvent(jobId, 
        "Resources allocated - CPU:" + std::to_string(req.cpuCores) + 
        " RAM:" + std::to_string(req.ramGB) + "GB" +
        " Disk:" + std::to_string(req.diskSlots) +
        " Network:" + std::to_string(req.networkSlots));
    
    return true;
}

void ResourceManager::releaseResources(int jobId) {
    std::lock_guard<std::mutex> lock(resourceMutex);
    
    // Find and release the allocation
    auto it = std::find_if(allocations.begin(), allocations.end(),
        [jobId](const Allocation& a) { return a.jobId == jobId; });
    
    if (it != allocations.end()) {
        availableCPU += it->allocated.cpuCores;
        availableRAM += it->allocated.ramGB;
        availableDisk += it->allocated.diskSlots;
        availableNetwork += it->allocated.networkSlots;
        
        globalLogger->logJobEvent(jobId, "Resources released");
        
        allocations.erase(it);
        
        // Notify waiting threads
        resourceCV.notify_all();
    }
}

ResourceRequest ResourceManager::getAvailableResources() {
    std::lock_guard<std::mutex> lock(resourceMutex);
    return ResourceRequest(availableCPU, availableRAM, availableDisk, availableNetwork);
}

ResourceRequest ResourceManager::getTotalResources() {
    return ResourceRequest(totalCPU, totalRAM, totalDisk, totalNetwork);
}

double ResourceManager::getCPUUtilization() {
    std::lock_guard<std::mutex> lock(resourceMutex);
    return 100.0 * (totalCPU - availableCPU) / totalCPU;
}

double ResourceManager::getRAMUtilization() {
    std::lock_guard<std::mutex> lock(resourceMutex);
    return 100.0 * (totalRAM - availableRAM) / totalRAM;
}

double ResourceManager::getDiskUtilization() {
    std::lock_guard<std::mutex> lock(resourceMutex);
    return 100.0 * (totalDisk - availableDisk) / totalDisk;
}

double ResourceManager::getNetworkUtilization() {
    std::lock_guard<std::mutex> lock(resourceMutex);
    return 100.0 * (totalNetwork - availableNetwork) / totalNetwork;
}

std::vector<ResourceManager::Allocation> ResourceManager::getAllocations() {
    std::lock_guard<std::mutex> lock(resourceMutex);
    return allocations;
}