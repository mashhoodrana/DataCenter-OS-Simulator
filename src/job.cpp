// ============================================
// FILE: job.cpp
// DESCRIPTION: Implementation of Job class
// ============================================

#include "../include/job.hpp"
#include "../include/logger.hpp"
#include <sstream>
#include <iomanip>
#include <random>

Job::Job(int jobId, JobPriority prio, ResourceRequest needs,
         ResourceManager* rm, DeadlockManager* dm)
    : id(jobId), priority(prio), status(JobStatus::WAITING),
      resourceNeeds(needs), waitingTime(0), executionTime(0),
      jobThread(nullptr), resourceManager(rm), deadlockManager(dm) {
    
    arrivalTime = std::chrono::steady_clock::now();
    
    globalLogger->logJobEvent(id, 
        "Arrived with priority=" + getPriorityString() + 
        " requesting CPU:" + std::to_string(needs.cpuCores) +
        " RAM:" + std::to_string(needs.ramGB) + "GB" +
        " Disk:" + std::to_string(needs.diskSlots) +
        " Network:" + std::to_string(needs.networkSlots));
}

Job::~Job() {
    if (jobThread != nullptr) {
        if (jobThread->joinable()) {
            jobThread->join();
        }
        delete jobThread;
    }
}

void Job::start() {
    jobThread = new std::thread(&Job::run, this);
}

void Job::join() {
    if (jobThread != nullptr && jobThread->joinable()) {
        jobThread->join();
    }
}

void Job::run() {
    // Wait for resources using Banker's Algorithm
    status = JobStatus::WAITING;
    globalLogger->logJobEvent(id, "Waiting for resources...");
    
    bool resourcesAllocated = false;
    while (!resourcesAllocated) {
        resourcesAllocated = deadlockManager->requestResources(id, resourceNeeds);
        
        if (!resourcesAllocated) {
            status = JobStatus::BLOCKED;
            globalLogger->logJobEvent(id, "BLOCKED - waiting for resources");
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
    
    // Resources allocated, calculate waiting time
    startTime = std::chrono::steady_clock::now();
    auto waitDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
        startTime - arrivalTime);
    waitingTime = waitDuration.count() / 1000.0;
    
    // Start execution
    status = JobStatus::RUNNING;
    globalLogger->logJobEvent(id, 
        "Started execution (waited " + std::to_string(waitingTime) + "s)");
    
    // Execute the job
    executeTask();
    
    // Job completed
    endTime = std::chrono::steady_clock::now();
    auto execDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
        endTime - startTime);
    executionTime = execDuration.count() / 1000.0;
    
    status = JobStatus::COMPLETED;
    globalLogger->logJobEvent(id, 
        "Completed (execution time: " + std::to_string(executionTime) + "s)");
    
    // Release resources
    resourceManager->releaseResources(id);
    
    // Create job summary log
    globalLogger->createJobLog(id, getSummary());
}

void Job::executeTask() {
    // Simulate job execution time based on resource usage
    // More resources = potentially longer execution
    int baseTime = 1000; // 1 second base
    int resourceFactor = resourceNeeds.cpuCores * 200 + 
                        resourceNeeds.ramGB * 100 +
                        resourceNeeds.diskSlots * 150 +
                        resourceNeeds.networkSlots * 100;
    
    // Add some randomness
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(-200, 200);
    
    int sleepTime = baseTime + resourceFactor + dis(gen);
    
    // Simulate work with phases
    globalLogger->logJobEvent(id, "Phase 1: CPU computation");
    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime / 3));
    
    globalLogger->logJobEvent(id, "Phase 2: Disk I/O operations");
    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime / 3));
    
    globalLogger->logJobEvent(id, "Phase 3: Network data transfer");
    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime / 3));
}

std::string Job::getPriorityString() const {
    switch (priority) {
        case JobPriority::HIGH: return "HIGH";
        case JobPriority::MEDIUM: return "MEDIUM";
        case JobPriority::LOW: return "LOW";
        default: return "UNKNOWN";
    }
}

std::string Job::getStatusString() const {
    switch (status) {
        case JobStatus::WAITING: return "WAITING";
        case JobStatus::RUNNING: return "RUNNING";
        case JobStatus::COMPLETED: return "COMPLETED";
        case JobStatus::BLOCKED: return "BLOCKED";
        default: return "UNKNOWN";
    }
}

std::string Job::getSummary() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "Job ID: " << id << "\n";
    ss << "Priority: " << getPriorityString() << "\n";
    ss << "Resources Used:\n";
    ss << "  - CPU Cores: " << resourceNeeds.cpuCores << "\n";
    ss << "  - RAM: " << resourceNeeds.ramGB << " GB\n";
    ss << "  - Disk Slots: " << resourceNeeds.diskSlots << "\n";
    ss << "  - Network Slots: " << resourceNeeds.networkSlots << "\n";
    ss << "Timing:\n";
    ss << "  - Waiting Time: " << waitingTime << " seconds\n";
    ss << "  - Execution Time: " << executionTime << " seconds\n";
    ss << "  - Total Time: " << (waitingTime + executionTime) << " seconds\n";
    ss << "Status: " << getStatusString() << "\n";
    return ss.str();
}