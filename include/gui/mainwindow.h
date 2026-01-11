#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QProgressBar>
#include <QTextEdit>
#include <QComboBox>
#include <memory>
#include <vector>

// Forward declarations for your core classes
// We'll integrate these in Phase 2
// class ResourceManager;
// class Scheduler;
// class DeadlockDetector;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Navigation
    void showWelcomeScreen();
    void showConfigScreen();
    void showSimulationScreen();
    void showResultsScreen();
    
    // Simulation control
    void onStartSimulation();
    void onStopSimulation();
    void onPauseSimulation();
    void onResetSimulation();
    
    // Timer updates
    void updateUI();
    void updateResourceUsage();
    void updateJobList();
    void updateMetrics();

private:
    // Screen setup functions
    QWidget* createWelcomeScreen();
    QWidget* createConfigScreen();
    QWidget* createSimulationScreen();
    QWidget* createResultsScreen();
    
    // UI Setup
    void setupUI();
    void setupMenuBar();
    void setupStatusBar();
    
    // Simulation control
    void initializeSimulation(int numJobs);
    void cleanupSimulation();
    
    // Main container
    QStackedWidget *stackedWidget;
    
    // Screen widgets
    QWidget *welcomeScreen;
    QWidget *configScreen;
    QWidget *simulationScreen;
    QWidget *resultsScreen;
    
    // Welcome Screen widgets
    QPushButton *btnGetStarted;
    
    // Config Screen widgets
    QSpinBox *spinJobCount;
    QComboBox *cmbPriorityDist;
    QSpinBox *spinCPUCores;
    QSpinBox *spinRAMSize;
    QSpinBox *spinDiskOps;
    QSpinBox *spinNetworkSpeed;
    QPushButton *btnStartSimulation;
    QPushButton *btnBackToWelcome;
    
    // Simulation Screen widgets
    QPushButton *btnPause;
    QPushButton *btnStop;
    QProgressBar *cpuUsageBar;
    QProgressBar *ramUsageBar;
    QProgressBar *diskUsageBar;
    QProgressBar *networkUsageBar;
    QLabel *lblTotalJobs;
    QLabel *lblCompletedJobs;
    QLabel *lblActiveJobs;
    QLabel *lblAvgWaitTime;
    QLabel *lblThroughput;
    QLabel *lblDeadlocksPrevented;
    QTableWidget *jobTable;
    QTextEdit *logView;
    
    // Results Screen widgets
    QLabel *lblFinalStats;
    QPushButton *btnRunAgain;
    QPushButton *btnExport;
    
    // Simulation state
    QTimer *updateTimer;
    bool isRunning;
    bool isPaused;
    int totalJobs;
    int completedJobs;
    int activeJobs;
    int simulationCounter;
    
    // Configuration values
    int configCPUCores;
    int configRAMSize;
    int configDiskOps;
    int configNetworkSpeed;
    
    // Mock data for testing
    std::vector<int> mockJobIds;
};

#endif // MAINWINDOW_H