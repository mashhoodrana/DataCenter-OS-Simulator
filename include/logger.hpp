// ============================================
// FILE: logger.hpp
// DESCRIPTION: Simplified logging system
// ============================================

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <fstream>
#include <mutex>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>

class Logger {
private:
    std::ofstream systemLog;
    std::mutex logMutex;
    
    std::string getCurrentTime();

public:
    Logger();
    ~Logger();
    
    // Logging methods
    void logEvent(const std::string& message);
    void logJobEvent(int jobId, const std::string& message);
    void createJobLog(int jobId, const std::string& summary);
};

// Global logger instance
extern Logger* globalLogger;

#endif 