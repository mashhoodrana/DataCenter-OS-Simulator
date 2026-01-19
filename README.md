# Smart Data Center Resource Management System - Operating System

## 🎯 Project Overview

A comprehensive Operating System simulation that models a **Data Center Resource Management System** with multiple concurrent jobs competing for limited computing resources. Features both **Console** and **GUI** interfaces built with Qt6.

![Version](https://img.shields.io/badge/version-1.0-blue)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey)
![License](https://img.shields.io/badge/license-Educational-green)

## 📚 OS Concepts Implemented

### 1. Process/Thread Management
- Each job runs as a separate thread
- Multi-phase execution (CPU → Disk I/O → Network)
- Dynamic job creation and lifecycle management
- Thread synchronization and coordination

### 2. CPU Scheduling
- **Priority Scheduling Algorithm**
  - **HIGH Priority:** System jobs (2x CPU allocation)
  - **MEDIUM Priority:** Paid users
  - **LOW Priority:** Free users
- Performance metrics: waiting time, turnaround time, throughput

### 3. Synchronization & Concurrency
- **Mutex locks** for critical section protection
- **Condition variables** for resource waiting queues
- **Atomic operations** for thread-safe flag management
- Race condition and deadlock prevention

### 4. Deadlock Handling
- **Banker's Algorithm** implementation
- Safe state verification before resource allocation
- Dynamic safe sequence generation
- Real-time deadlock prevention statistics

### 5. Memory Management
- Dynamic memory allocation for job structures
- Resource tracking with STL containers
- Proper cleanup and memory leak prevention
- Smart pointer usage in GUI components

### 6. File Management & Logging
- System-wide event log: `logs/system.log`
- Per-job summary files: `logs/job_*.log`
- Thread-safe logging with mutex protection
- Timestamped entries with millisecond precision

### 7. Performance Metrics
- Average waiting time per job
- Average turnaround time
- Resource utilization percentages (CPU, RAM, Disk, Network)
- System throughput (jobs/second)
- Deadlock prevention statistics

---

## 🖥️ Dual Interface Support

### Console Application
- Traditional command-line interface
- Real-time text-based output
- Detailed event logging
- Performance statistics display

### GUI Application (Qt6)
- **Modern, professional interface** with 4 screens:
  1. **Welcome Screen** - Beautiful gradient splash
  2. **Configuration Screen** - Parameter setup
  3. **Simulation Screen** - Real-time monitoring with:
     - Live resource utilization bars
     - Performance metrics dashboard
     - Active jobs table
     - System event log
  4. **Results Screen** - Final statistics and log access
- **Real-time updates** every 500ms
- **Cross-platform** compatibility
- **Thread-safe** GUI updates

---

## 🛠️ System Requirements

### All Platforms
- **C++17** compatible compiler
- **CMake 3.16+** (for building)

### Console Application
- **Windows:** MinGW-w64 or MSVC
- **Linux:** GCC 7+ or Clang 5+
- **macOS:** Xcode Command Line Tools

### GUI Application (Additional)
- **Qt 6.5+** (Qt6 Core, Qt6 Widgets)
- **Compiler:**
  - Windows: MinGW or MSVC 2019+
  - Linux: GCC 7+ or Clang 5+
  - macOS: Xcode 10+

---

## Installation

### Windows

#### 1. Install Qt6
Download from: https://www.qt.io/download-qt-installer
- Select: Qt 6.x → Desktop (MinGW or MSVC)
- Include: CMake, Qt Creator

#### 2. Install MinGW (if using MinGW)
From: https://www.mingw-w64.org/ or MSYS2

Add to PATH: `C:\Qt\Tools\mingw1120_64\bin`

### Linux (Ubuntu/Debian)

```bash
# Install dependencies
sudo apt update
sudo apt install build-essential cmake
sudo apt install qt6-base-dev qt6-tools-dev

# Verify installation
g++ --version
cmake --version
qmake6 --version
```

### Linux (Fedora)

```bash
sudo dnf install gcc-c++ cmake
sudo dnf install qt6-qtbase-devel qt6-qttools-devel
```

### macOS

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install Qt6 via Homebrew
brew install qt@6 cmake

# Add Qt to PATH
echo 'export PATH="/usr/local/opt/qt@6/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc
```

---

## 🚀 Build and Run

### Using CMake (Recommended - Builds Both Console & GUI)

#### All Platforms:

```bash
# 1. Create build directory
mkdir build
cd build

# 2. Configure
cmake ..

# 3. Build
cmake --build .

# Windows: Run
bin\datacenter_console.exe      # Console version
bin\datacenter_gui.exe          # GUI version

# Linux/macOS: Run
./bin/datacenter_console        # Console version
./bin/datacenter_gui            # GUI version
```

#### Specify Qt Path (if not auto-detected):

```bash
# Windows
cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/6.5.0/mingw_64"

# Linux
cmake .. -DCMAKE_PREFIX_PATH=/usr/lib/qt6

# macOS
cmake .. -DCMAKE_PREFIX_PATH=/usr/local/opt/qt@6
```

### Using Qt Creator (GUI Only)

1. Open **Qt Creator**
2. `File → Open File or Project`
3. Select `CMakeLists.txt`
4. Configure project with your Qt kit
5. Press `Ctrl+R` (or `⌘+R` on macOS) to build and run

### Using Makefile (Console Only - Windows)

```bash
# Build
mingw32-make

# Run
bin\datacenter.exe

# Clean
mingw32-make clean
```

---

## 📊 Sample Outputs

### Console Output

```
╔════════════════════════════════════════════════════════════╗
║                                                            ║
║        DATA CENTER RESOURCE MANAGEMENT SYSTEM              ║
║           Operating System Concepts Simulation             ║
║                                                            ║
╚════════════════════════════════════════════════════════════╝

┌─────────────────────────────────────────────────────────┐
│ SYSTEM CONFIGURATION                                    │
├─────────────────────────────────────────────────────────┤
│ Total CPU Cores:      16 cores                          │
│ Total RAM:            64 GB                             │
│ Total Disk Slots:      8 slots                          │
│ Total Network Slots:   4 slots                          │
└─────────────────────────────────────────────────────────┘

Enter number of jobs to simulate (recommended: 10-20): 15

[2025-01-11 14:23:15.123] Job 1: Arrived with priority=HIGH
[2025-01-11 14:23:15.456] ✅ Banker's Algorithm verified - System remains in SAFE state
[2025-01-11 14:23:15.789] Job 1: Resources allocated - CPU:4 RAM:8GB Disk:2 Network:1
[2025-01-11 14:23:16.012] Job 1: Started execution (waited 0.89s)
...

╔════════════════════════════════════════════════════════════╗
║                   SIMULATION COMPLETE                      ║
╚════════════════════════════════════════════════════════════╝

========== SCHEDULING REPORT ==========
Algorithm: Priority Scheduling
Total Jobs: 15
Completed Jobs: 15
Average Waiting Time: 2.34 seconds
Average Turnaround Time: 5.67 seconds
Throughput: 4.2 jobs/second
=======================================

========== RESOURCE UTILIZATION ==========
CPU Utilization:     78.45%
RAM Utilization:     65.23%
Disk Utilization:    42.18%
Network Utilization: 31.56%
==========================================

========== DEADLOCK MANAGEMENT REPORT ==========
Deadlocks Detected: 0
Deadlocks Prevented: 3
Method: Banker's Algorithm
===============================================

✅ All logs saved to 'logs/' directory
```

### GUI Screenshots

**Welcome Screen:**
- Beautiful gradient background
- Large "Get Started" button

**Configuration Screen:**
- Number of jobs selector (5-50)
- Scheduling algorithm dropdown
- System resources information

**Simulation Screen (Real-time):**
- 🔷 CPU Cores: [████████░░] 78%
- 🔶 RAM Memory: [██████░░░░] 65%
- 💾 Disk I/O: [████░░░░░░] 42%
- 🌐 Network: [███░░░░░░░] 31%
- Active jobs table with live updates
- Scrolling event log

**Results Screen:**
- Complete statistics summary
- Log file location display
- Options to view logs, start new simulation, or exit

---

## 📁 Project Structure

```
DataCenter-Simulator/
├── include/
│   ├── gui/
│   │   └── mainwindow.h           # GUI main window header
│   ├── deadlock.hpp               # Banker's Algorithm
│   ├── job.hpp                    # Job/Thread class
│   ├── logger.hpp                 # Logging system
│   ├── resource_manager.hpp       # Resource allocation
│   └── scheduler.hpp              # CPU Scheduler
│
├── src/
│   ├── gui/
│   │   ├── main_gui.cpp           # GUI entry point
│   │   └── mainwindow.cpp         # GUI implementation
│   ├── deadlock.cpp               # Banker's Algorithm implementation
│   ├── job.cpp                    # Job thread implementation
│   ├── logger.cpp                 # Cross-platform logging
│   ├── main.cpp                   # Console entry point
│   ├── resource_manager.cpp       # Resource management
│   └── scheduler.cpp              # Priority scheduler
│
├── logs/                          # Generated at runtime
│   ├── system.log                 # System-wide events
│   └── job_*.log                  # Individual job summaries
│
├── build/                         # CMake build directory
│   └── bin/
│       ├── datacenter_console     # Console executable
│       └── datacenter_gui         # GUI executable
│
├── CMakeLists.txt                 # CMake configuration
├── Makefile                       # Windows Makefile (console)
└── README.md                      
```

---

## 🎮 Usage Guide

### Console Application

1. Run the executable
2. Enter number of jobs (5-50 recommended)
3. Watch real-time simulation
4. Review final statistics
5. Check `logs/` directory for detailed logs

### GUI Application

1. **Launch** the application
2. **Welcome Screen:** Click "Get Started"
3. **Configuration:**
   - Set number of jobs (default: 10)
   - Select scheduling algorithm
   - Click "Start Simulation"
4. **Simulation:** Watch real-time updates
   - Resource bars animate
   - Metrics update every 500ms
   - Jobs appear in table
   - Events scroll in log
5. **Results:** View final statistics
   - Click "View Logs" to open logs folder
   - Click "New Simulation" to run again
   - Click "Exit" to close

---

## 🎓 Key Features

### Core Features
✅ **Multi-threaded** job execution  
✅ **Thread-safe** resource management  
✅ **Banker's Algorithm** for deadlock prevention  
✅ **Priority-based** CPU scheduling  
✅ **Comprehensive logging** system  
✅ **Performance metrics** collection  
✅ **Cross-platform** compatibility

### GUI Features
✅ **Professional interface** with modern styling  
✅ **Real-time visualization** of resource usage  
✅ **Live job monitoring** with status updates  
✅ **Interactive controls** (start, stop, reset)  
✅ **Detailed results** screen with statistics  
✅ **Log file integration** with one-click access  
✅ **Responsive design** with scrolling support

---

## 🔧 Configuration

### System Resources (Configurable in code)
- **CPU Cores:** 16 cores (default)
- **RAM:** 64 GB (default)
- **Disk I/O Slots:** 8 slots (default)
- **Network Channels:** 4 channels (default)

Modify in `main.cpp` or `mainwindow.cpp`:
```cpp
const int TOTAL_CPU = 16;      // Change CPU cores
const int TOTAL_RAM = 64;      // Change RAM (GB)
const int TOTAL_DISK = 8;      // Change disk slots
const int TOTAL_NETWORK = 4;   // Change network channels
```

### Job Generation
Jobs are randomly generated with:
- **Random priority** (HIGH, MEDIUM, LOW)
- **Random resource needs** within configured limits
- **Arrival time simulation** with 100ms intervals

---

## 🐛 Troubleshooting

### Qt not found
```bash
# Set Qt path manually
cmake .. -DCMAKE_PREFIX_PATH="/path/to/qt6"
```

### Build errors
```bash
# Clean and rebuild
rm -rf build
mkdir build
cd build
cmake ..
cmake --build .
```

### Missing logs directory
The `logs/` directory is created automatically at runtime.

### GUI doesn't start
- Verify Qt6 is installed: `qmake6 --version`
- Check Qt DLLs are accessible (Windows)
- Run from Qt Creator for easier debugging

---

## 📈 Future Enhancements

### Planned Features
- [ ] Additional scheduling algorithms (Round Robin, SJF, FCFS)
- [ ] Real-time resource usage graphs
- [ ] Job priority adjustment during runtime
- [ ] Save/load simulation configurations
- [ ] Export results to CSV/Excel
- [ ] Network-based distributed simulation
- [ ] Advanced memory management strategies

### Contribution Ideas
- Dark/Light theme toggle
- Custom resource allocation policies
- Multi-language support
- Performance benchmarking tools

---

## 👨‍💻 Technical Details

### Threading Model
- Main thread handles UI/console
- Each job runs in separate thread
- Mutex protection for shared resources
- Condition variables for synchronization

### Banker's Algorithm
- Safe state verification before allocation
- Need matrix calculation
- Safe sequence generation
- Prevention counter tracking

### GUI Architecture
- Qt6 Widgets framework
- Model-View separation
- Thread-safe updates via Qt signals
- Timer-based UI refresh (500ms)

---

## 📚 Learning Outcomes

Students/Users will understand:
1. **Process Management** - Thread creation, lifecycle, synchronization
2. **CPU Scheduling** - Priority algorithms, performance metrics
3. **Deadlock Handling** - Banker's Algorithm, prevention strategies
4. **Resource Management** - Allocation policies, utilization tracking
5. **Synchronization** - Mutex, condition variables, atomic operations
6. **File I/O** - Logging systems, cross-platform compatibility
7. **GUI Programming** - Qt framework, event-driven architecture

---

## 👨‍💻 Author

**Created for Operating Systems Course Project**

### Technologies Used
- **Language:** C++17.
- **GUI Framework:** Qt6
- **Build System:** CMake
- **Threading:** C++11 std::thread
- **Synchronization:** std::mutex, std::condition_variable
- **Platforms:** Windows, Linux, macOS

---

## 📄 License

Educational project for academic purposes. Free to use and modify for learning.


## 📞 Support

For issues or questions:
1. Check the troubleshooting section
2. Review Qt documentation: https://doc.qt.io/
3. Verify CMake configuration
4. Check compiler compatibility

---

**Status:** ✅ Production Ready  
**Version:** 1.0  
**Last Updated:** January 2025

---

## Quick Start Commands

```bash
# Clone/Download project
# cd DataCenter-Simulator

# Build everything
mkdir build && cd build
cmake ..
cmake --build .

# Run console version
./bin/datacenter_console        # Linux/macOS
bin\datacenter_console.exe      # Windows

# Run GUI version
./bin/datacenter_gui            # Linux/macOS
bin\datacenter_gui.exe          # Windows

# View logs
cd ../logs
cat system.log                  # Linux/macOS
type system.log                 # Windows
```

**Enjoy simulating your data center!**

## 🤝 Contributing
Contributions are welcome!
Check the Issues section for `good first issue` and `help wanted` labels.
