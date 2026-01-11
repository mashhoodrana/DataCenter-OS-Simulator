// ============================================
// FILE: scheduler.hpp
// DESCRIPTION: CPU Scheduler with Priority Scheduling
// ============================================

#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include "job.hpp"
#include "resource_manager.hpp"
#include "deadlock.hpp"
#include <queue>
#include <vector>
#include <mutex>
#include <memory>

// Comparator for priority queue (higher priority = lower number)
struct JobComparator {
    bool operator()(Job* a, Job* b) const {
        return static_cast<int>(a->getPriority()) > static_cast<int>(b->getPriority());
    }
};

class Scheduler {
private:
    std::priority_queue<Job*, std::vector<Job*>, JobComparator> readyQueue;
    std::vector<Job*> allJobs;
    std::mutex schedulerMutex;
    
    ResourceManager* resourceManager;
    DeadlockManager* deadlockManager;
    
    // Statistics
    int totalJobs;
    int completedJobs;
    double totalWaitingTime;
    double totalExecutionTime;

public:
    Scheduler(ResourceManager* rm, DeadlockManager* dm);
    ~Scheduler();
    
    // Job management
    void addJob(Job* job);
    void scheduleAll();
    void waitForAllJobs();
    
    // Statistics
    double getAverageWaitingTime() const;
    double getAverageTurnaroundTime() const;
    double getThroughput() const;
    int getTotalJobs() const { return totalJobs; }
    int getCompletedJobs() const { return completedJobs; }
    
    // Report
    std::string getSchedulingReport() const;
};

#endif // SCHEDULER_HPP