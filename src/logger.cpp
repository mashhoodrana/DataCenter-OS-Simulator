// ============================================
// FILE: logger.cpp
// DESCRIPTION: Simplified logging implementation
// ============================================

#include "../include/logger.hpp"
#include <direct.h>
#include <sys/stat.h>
#include <chrono>

// Global logger instance
Logger* globalLogger = nullptr;

Logger::Logger() {
    std::cout << "[DEBUG] Inside Logger constructor...\n";
    std::cout.flush();
    
    // Create logs directory
    struct stat info;
    if (stat("logs", &info) != 0) {
        _mkdir("logs");
    }
    
    std::cout << "[DEBUG] Logs directory created/verified\n";
    std::cout.flush();
    
    // Open system log file
    systemLog.open("logs/system.log", std::ios::app);
    if (!systemLog.is_open()) {
        std::cerr << "Error: Could not open system.log" << std::endl;
    } else {
        std::cout << "[DEBUG] System log opened successfully\n";
        std::cout.flush();
    }
    
    logEvent("========== System Started ==========");
}

Logger::~Logger() {
    if (systemLog.is_open()) {
        logEvent("========== System Shutdown ==========\n");
        systemLog.close();
    }
}

std::string Logger::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

void Logger::logEvent(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    
    std::string logMessage = "[" + getCurrentTime() + "] " + message;
    
    // Write to file
    if (systemLog.is_open()) {
        systemLog << logMessage << std::endl;
        systemLog.flush();
    }
    
    // Also print to console
    std::cout << logMessage << std::endl;
}

void Logger::logJobEvent(int jobId, const std::string& message) {
    std::string formattedMessage = "Job " + std::to_string(jobId) + ": " + message;
    logEvent(formattedMessage);
}

void Logger::createJobLog(int jobId, const std::string& summary) {
    std::lock_guard<std::mutex> lock(logMutex);
    
    std::string filename = "logs/job_" + std::to_string(jobId) + ".log";
    std::ofstream jobLog(filename);
    
    if (jobLog.is_open()) {
        jobLog << "========== Job " << jobId << " Summary ==========" << std::endl;
        jobLog << summary << std::endl;
        jobLog << "============================================" << std::endl;
        jobLog.close();
    }
}