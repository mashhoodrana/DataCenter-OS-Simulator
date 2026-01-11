// ============================================
// FILE: job.hpp
// DESCRIPTION: Job class representing threads
// ============================================

#ifndef JOB_HPP
#define JOB_HPP

#include "resource_manager.hpp"
#include "deadlock.hpp"
#include <thread>
#include <chrono>
#include <string>

enum class JobPriority {
    HIGH = 1,    // System jobs
    MEDIUM = 2,  // Paid users
    LOW = 3      // Free users
};

enum class JobStatus {
    WAITING,
    RUNNING,
    COMPLETED,
    BLOCKED
};

class Job {
private:
    int id;
    JobPriority priority;
    JobStatus status;
    ResourceRequest resourceNeeds;
    
    // Timing information
    std::chrono::steady_clock::time_point arrivalTime;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    
    double waitingTime;  // in seconds
    double executionTime; // in seconds
    
    // Thread
    std::thread* jobThread;
    
    // References to managers
    ResourceManager* resourceManager;
    DeadlockManager* deadlockManager;
    
    // Job execution logic
    void run();
    void executeTask();

public:
    Job(int jobId, JobPriority prio, ResourceRequest needs,
        ResourceManager* rm, DeadlockManager* dm);
    
    ~Job();
    
    // Thread management
    void start();
    void join();
    
    // Getters
    int getId() const { return id; }
    JobPriority getPriority() const { return priority; }
    JobStatus getStatus() const { return status; }
    double getWaitingTime() const { return waitingTime; }
    double getExecutionTime() const { return executionTime; }
    ResourceRequest getResourceNeeds() const { return resourceNeeds; }
    
    // Priority comparison for scheduling
    bool operator<(const Job& other) const {
        return static_cast<int>(priority) > static_cast<int>(other.priority);
    }
    
    // Utility
    std::string getPriorityString() const;
    std::string getStatusString() const;
    std::string getSummary() const;
};

#endif // JOB_HPP