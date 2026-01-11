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
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <thread>
#include <mutex>
#include <atomic>

// Include your console project headers
#include "../resource_manager.hpp"
#include "../scheduler.hpp"
#include "../deadlock.hpp"
#include "../job.hpp"
#include "../logger.hpp"

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
    void onNewSimulation();
    void onViewLogs();
    
    // UI updates
    void updateUI();

private:
    // Screen creation
    QWidget* createWelcomeScreen();
    QWidget* createConfigScreen();
    QWidget* createSimulationScreen();
    QWidget* createResultsScreen();
    
    // UI Setup
    void setupUI();
    void setupMenuBar();
    void setupStatusBar();
    
    // Simulation
    void runSimulation(int numJobs);
    void cleanupSimulation();
    
    // Display helpers
    void displayFinalResults();
    void addLogMessage(const QString& source, const QString& message);
    
    // Main container
    QStackedWidget *stackedWidget;
    
    // Screen widgets
    QWidget *welcomeScreen;
    QWidget *configScreen;
    QWidget *simulationScreen;
    QWidget *resultsScreen;
    
    // Welcome Screen
    QPushButton *btnGetStarted;
    
    // Config Screen
    QSpinBox *spinJobCount;
    QComboBox *cmbScheduling;
    QPushButton *btnStartSimulation;
    QPushButton *btnBackToWelcome;
    
    // Simulation Screen
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
    
    // Results Screen
    QLabel *lblFinalTotalJobs;
    QLabel *lblFinalCompletedJobs;
    QLabel *lblFinalAvgWaitTime;
    QLabel *lblFinalAvgTurnaroundTime;
    QLabel *lblFinalThroughput;
    QLabel *lblFinalCPUUtil;
    QLabel *lblFinalRAMUtil;
    QLabel *lblFinalDiskUtil;
    QLabel *lblFinalNetworkUtil;
    QLabel *lblFinalDeadlocksPrevented;
    QPushButton *btnNewSimulation;
    QPushButton *btnViewLogs;
    QPushButton *btnExit;
    
    // Simulation state
    QTimer *updateTimer;
    std::atomic<bool> isRunning;
    std::atomic<bool> stopRequested;
    
    // Core simulation objects (using raw pointers for proper lifecycle management)
    ResourceManager* resourceManager;
    DeadlockManager* deadlockManager;
    Scheduler* scheduler;
    
    // Simulation thread
    std::thread simulationThread;
};

#endif // MAINWINDOW_H