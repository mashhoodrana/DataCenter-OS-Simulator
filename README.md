# Data Center Resource Management System

## 🎯 Project Overview

A comprehensive Operating System simulation that models a **Data Center Resource Management System** with multiple concurrent jobs competing for limited computing resources.

## 📚 OS Concepts Implemented

### 1. Process/Thread Management
- Each job runs as a separate thread
- Multi-phase execution (CPU → Disk → Network)
- Dynamic job creation and lifecycle management

### 2. CPU Scheduling
- **Priority Scheduling Algorithm**
  - HIGH Priority: System jobs
  - MEDIUM Priority: Paid users
  - LOW Priority: Free users
- Performance metrics: waiting time, turnaround time, throughput

### 3. Synchronization & Concurrency
- **Mutex locks** for RAM and Disk access
- **Semaphores** for CPU core allocation
- **Condition variables** for resource waiting queues
- Race condition prevention

### 4. Deadlock Handling
- **Banker's Algorithm** implementation
- Safe state verification before resource allocation
- Deadlock prevention with safe sequence generation
- Statistics tracking

### 5. Memory Management
- Dynamic memory allocation for job structures
- Resource tracking using vectors
- Proper deallocation and cleanup

### 6. File Management & Logging
- System-wide event log: `logs/system.log`
- Per-job summary files: `logs/job_*.log`
- Thread-safe logging with timestamps

### 7. Performance Metrics
- Average waiting time
- Average turnaround time
- Resource utilization percentages
- Throughput calculation
- Deadlock prevention statistics

## 🛠️ System Requirements

- **OS**: Windows 10/11
- **Compiler**: MinGW-w64 (g++ with C++17 support)
- **IDE**: VS Code (recommended)

## 📦 Installation

### 1. Install MinGW-w64

Download and install from: https://www.mingw-w64.org/

Or use MSYS2:
```bash
# Install MSYS2 from https://www.msys2.org/
# Then run in MSYS2 terminal:
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-gdb
pacman -S make
```

Add to PATH: `C:\msys64\mingw64\bin`

### 2. Verify Installation
```bash
g++ --version
mingw32-make --version
```

## 🚀 Build and Run

### Build the project:
```bash
mingw32-make
```

### Run the simulation:
```bash
mingw32-make run
```

### Or manually:
```bash
bin\datacenter.exe
```

### Clean build files:
```bash
mingw32-make clean
```

### Clean everything (including logs):
```bash
mingw32-make cleanall
```

## 📊 Sample Output
```
╔════════════════════════════════════════════════════════════╗
║                                                            ║
║        DATA CENTER RESOURCE MANAGEMENT SYSTEM              ║
║           Operating System Concepts Simulation             ║
║                                                            ║
╚════════════════════════════════════════════════════════════╝

Enter number of jobs to simulate: 15

[2025-01-11 14:23:15] Job 1 arrived (Priority: HIGH)
[2025-01-11 14:23:16] Banker's Algorithm: Safe state verified
[2025-01-11 14:23:16] Job 1 resources allocated
...

========== SIMULATION COMPLETE ==========
Average Waiting Time: 2.34 seconds
Throughput: 4.2 jobs/second
Deadlocks Prevented: 3
=========================================
```

## 📁 Project Structure
```
DataCenter-Simulator/
├── include/
│   ├── logger.hpp              # Logging system
│   ├── resource_manager.hpp    # Resource allocation
│   ├── deadlock.hpp            # Banker's Algorithm
│   ├── job.hpp                 # Job/Thread class
│   └── scheduler.hpp           # CPU Scheduler
├── src/
│   ├── main.cpp               # Entry point
│   ├── logger.cpp
│   ├── resource_manager.cpp
│   ├── deadlock.cpp
│   ├── job.cpp
│   └── scheduler.cpp
├── logs/                      # Generated logs
├── bin/                       # Executable
├── obj/                       # Object files
├── Makefile
└── README.md
```

## 🎓 Key Features

✅ **Multi-threaded** job execution  
✅ **Thread-safe** resource management  
✅ **Banker's Algorithm** for deadlock prevention  
✅ **Priority-based** CPU scheduling  
✅ **Comprehensive logging** system  
✅ **Performance metrics** collection  
✅ **Modular design** for easy GUI integration  

## 📈 Future Enhancements

- [ ] Qt-based GUI interface
- [ ] Real-time resource visualization
- [ ] Additional scheduling algorithms (Round Robin, SJF)
- [ ] Advanced memory management strategies
- [ ] Network-based distributed simulation

## 👨‍💻 Author

Created for Operating Systems Course Project

## 📄 License

Educational project for academic purposes