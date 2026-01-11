// ============================================
// FILE: scheduler.cpp
// DESCRIPTION: Implementation of CPU Scheduler
// ============================================

#include "../include/scheduler.hpp"
#include "../include/logger.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>

Scheduler::Scheduler(ResourceManager* rm, DeadlockManager* dm)
    : resourceManager(rm), deadlockManager(dm),
      totalJobs(0), completedJobs(0), totalWaitingTime(0), totalExecutionTime(0) {
    
    globalLogger->logEvent("Scheduler initialized with Priority Scheduling");
}

Scheduler::~Scheduler() {
    // Clean up all jobs
    for (Job* job : allJobs) {
        delete job;
    }
}

void Scheduler::addJob(Job* job) {
    std::lock_guard<std::mutex> lock(schedulerMutex);
    
    readyQueue.push(job);
    allJobs.push_back(job);
    totalJobs++;
    
    globalLogger->logJobEvent(job->getId(), 
        "Added to scheduler ready queue (Priority: " + job->getPriorityString() + ")");
}

void Scheduler::scheduleAll() {
    globalLogger->logEvent("Starting job scheduling...");
    
    // Start all jobs based on priority order
    std::vector<Job*> jobsToStart;
    
    {
        std::lock_guard<std::mutex> lock(schedulerMutex);
        
        while (!readyQueue.empty()) {
            Job* job = readyQueue.top();
            readyQueue.pop();
            jobsToStart.push_back(job);
        }
    }
    
    // Start jobs (they will wait for resources internally)
    for (Job* job : jobsToStart) {
        job->start();
        globalLogger->logJobEvent(job->getId(), 
            "Scheduled for execution (Priority: " + job->getPriorityString() + ")");
        
        // Small delay to simulate arrival time differences
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    globalLogger->logEvent("All jobs scheduled - waiting for completion...");
}

void Scheduler::waitForAllJobs() {
    globalLogger->logEvent("Waiting for all jobs to complete...");
    
    // Wait for all job threads to complete
    for (Job* job : allJobs) {
        job->join();
        
        // Collect statistics
        totalWaitingTime += job->getWaitingTime();
        totalExecutionTime += job->getExecutionTime();
        completedJobs++;
    }
    
    globalLogger->logEvent("All jobs completed!");
}

double Scheduler::getAverageWaitingTime() const {
    if (completedJobs == 0) return 0.0;
    return totalWaitingTime / completedJobs;
}

double Scheduler::getAverageTurnaroundTime() const {
    if (completedJobs == 0) return 0.0;
    return (totalWaitingTime + totalExecutionTime) / completedJobs;
}

double Scheduler::getThroughput() const {
    if (totalExecutionTime == 0) return 0.0;
    return completedJobs / totalExecutionTime;
}

std::string Scheduler::getSchedulingReport() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    
    ss << "\n========== SCHEDULING REPORT ==========\n";
    ss << "Algorithm: Priority Scheduling\n";
    ss << "Total Jobs: " << totalJobs << "\n";
    ss << "Completed Jobs: " << completedJobs << "\n";
    ss << "Average Waiting Time: " << getAverageWaitingTime() << " seconds\n";
    ss << "Average Turnaround Time: " << getAverageTurnaroundTime() << " seconds\n";
    ss << "Throughput: " << getThroughput() << " jobs/second\n";
    ss << "=======================================\n";
    
    return ss.str();
}