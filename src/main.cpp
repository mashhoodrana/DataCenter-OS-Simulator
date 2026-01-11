// ============================================
// FILE: main.cpp
// DESCRIPTION: Main simulation entry point
// ============================================

#include "../include/logger.hpp"
#include "../include/resource_manager.hpp"
#include "../include/deadlock.hpp"
#include "../include/scheduler.hpp"
#include "../include/job.hpp"
#include <iostream>
#include <random>
#include <iomanip>

void printHeader() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                            ║\n";
    std::cout << "║        DATA CENTER RESOURCE MANAGEMENT SYSTEM              ║\n";
    std::cout << "║           Operating System Concepts Simulation             ║\n";
    std::cout << "║                                                            ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
}

void printSystemConfiguration(ResourceManager* rm) {
    auto total = rm->getTotalResources();
    
    std::cout << "┌─────────────────────────────────────────────────────────┐\n";
    std::cout << "│ SYSTEM CONFIGURATION                                    │\n";
    std::cout << "├─────────────────────────────────────────────────────────┤\n";
    std::cout << "│ Total CPU Cores:     " << std::setw(3) << total.cpuCores << " cores                      │\n";
    std::cout << "│ Total RAM:           " << std::setw(3) << total.ramGB << " GB                        │\n";
    std::cout << "│ Total Disk Slots:    " << std::setw(3) << total.diskSlots << " slots                      │\n";
    std::cout << "│ Total Network Slots: " << std::setw(3) << total.networkSlots << " slots                      │\n";
    std::cout << "└─────────────────────────────────────────────────────────┘\n\n";
}

void printFinalReport(Scheduler* scheduler, ResourceManager* rm, DeadlockManager* dm) {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                   SIMULATION COMPLETE                      ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";
    
    std::cout << scheduler->getSchedulingReport();
    
    std::cout << "\n========== RESOURCE UTILIZATION ==========\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "CPU Utilization:     " << rm->getCPUUtilization() << "%\n";
    std::cout << "RAM Utilization:     " << rm->getRAMUtilization() << "%\n";
    std::cout << "Disk Utilization:    " << rm->getDiskUtilization() << "%\n";
    std::cout << "Network Utilization: " << rm->getNetworkUtilization() << "%\n";
    std::cout << "==========================================\n";
    
    std::cout << dm->getSafetyReport();
    
    std::cout << "\n✅ All logs saved to 'logs/' directory\n";
    std::cout << "✅ Individual job summaries available in logs/job_*.log\n\n";
}

Job* generateRandomJob(int id, ResourceManager* rm, DeadlockManager* dm) {
    std::random_device rd;
    std::mt19937 gen(rd());
    
    std::uniform_int_distribution<> prioDist(1, 3);
    JobPriority priority = static_cast<JobPriority>(prioDist(gen));
    
    std::uniform_int_distribution<> cpuDist(1, 4);
    std::uniform_int_distribution<> ramDist(2, 16);
    std::uniform_int_distribution<> diskDist(1, 3);
    std::uniform_int_distribution<> netDist(1, 2);
    
    int cpuMultiplier = (priority == JobPriority::HIGH) ? 2 : 1;
    
    ResourceRequest needs(
        cpuDist(gen) * cpuMultiplier,
        ramDist(gen),
        diskDist(gen),
        netDist(gen)
    );
    
    return new Job(id, priority, needs, rm, dm);
}

int main() {
    printHeader();
    
    std::cout << "Initializing Data Center Simulation...\n\n";
    std::cout << "[DEBUG] Step 1: Creating logger...\n";
    std::cout.flush();
    
    // Create global logger
    globalLogger = new Logger();
    
    std::cout << "[DEBUG] Logger created successfully\n";
    std::cout.flush();
    
    const int TOTAL_CPU = 16;
    const int TOTAL_RAM = 64;
    const int TOTAL_DISK = 8;
    const int TOTAL_NETWORK = 4;
    
    std::cout << "[DEBUG] Step 2: Creating ResourceManager...\n";
    std::cout.flush();
    
    ResourceManager resourceManager(TOTAL_CPU, TOTAL_RAM, TOTAL_DISK, TOTAL_NETWORK);
    
    std::cout << "[DEBUG] Step 3: Creating DeadlockManager...\n";
    std::cout.flush();
    
    DeadlockManager deadlockManager(&resourceManager);
    
    std::cout << "[DEBUG] Step 4: Creating Scheduler...\n";
    std::cout.flush();
    
    Scheduler scheduler(&resourceManager, &deadlockManager);
    
    std::cout << "[DEBUG] Initialization complete!\n\n";
    std::cout.flush();
    
    printSystemConfiguration(&resourceManager);
    
    int numJobs;
    std::cout << "Enter number of jobs to simulate (recommended: 10-20): ";
    std::cout.flush();
    std::cin >> numJobs;
    
    if (numJobs < 1 || numJobs > 100) {
        std::cout << "Invalid input. Using default: 15 jobs\n";
        numJobs = 15;
    }
    
    std::cout << "\nGenerating " << numJobs << " jobs...\n";
    std::cout << "────────────────────────────────────────────────────────────\n\n";
    std::cout.flush();
    
    for (int i = 1; i <= numJobs; i++) {
        Job* job = generateRandomJob(i, &resourceManager, &deadlockManager);
        scheduler.addJob(job);
    }
    
    std::cout << "\n" << numJobs << " jobs created and added to scheduler\n";
    std::cout << "Starting simulation...\n";
    std::cout << "════════════════════════════════════════════════════════════\n\n";
    std::cout.flush();
    
    auto startTime = std::chrono::steady_clock::now();
    
    scheduler.scheduleAll();
    scheduler.waitForAllJobs();
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    printFinalReport(&scheduler, &resourceManager, &deadlockManager);
    
    std::cout << "Total Simulation Time: " << duration.count() << " seconds\n\n";
    
    std::cout << "Press Enter to exit...";
    std::cin.ignore();
    std::cin.get();
    
    delete globalLogger;
    
    return 0;
}