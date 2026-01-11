// ============================================
// FILE: mainwindow.cpp  
// DESCRIPTION: COMPLETE FIXED VERSION
// ============================================

#include "../../include/gui/mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMenuBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QHeaderView>
#include <QDateTime>
#include <QFont>
#include <QSplitter>
#include <QFormLayout>
#include <QFileDialog>
#include <QTextStream>
#include <QDesktopServices>
#include <QScrollArea>
#include <random>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , isRunning(false)
    , stopRequested(false)
    , resourceManager(nullptr)
    , deadlockManager(nullptr)
    , scheduler(nullptr)
{
    if (!globalLogger) {
        globalLogger = new Logger();
    }
    
    setupUI();
    setupMenuBar();
    setupStatusBar();
    
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &MainWindow::updateUI);
    
    showWelcomeScreen();
}

MainWindow::~MainWindow()
{
    if (updateTimer) {
        updateTimer->stop();
    }
    stopRequested = true;
    if (simulationThread.joinable()) {
        simulationThread.join();
    }
    cleanupSimulation();
}

void MainWindow::setupUI()
{
    setWindowTitle("Data Center Resource Management System - OS Project");
    setMinimumSize(1400, 900);
    resize(1600, 1000);
    
    setStyleSheet(R"(
        QMainWindow { background-color: #f0f2f5; }
        QWidget { color: #1a1a1a; }
        QGroupBox {
            font-size: 15px; font-weight: bold; color: #1a1a1a;
            border: 2px solid #c0c0c0; border-radius: 10px;
            margin-top: 15px; padding: 20px 15px 15px 15px;
            background-color: #ffffff;
        }
        QGroupBox::title {
            subcontrol-origin: margin; subcontrol-position: top left;
            left: 15px; padding: 5px 12px;
            background-color: #ffffff; color: #1976D2;
        }
        QPushButton {
            border: none; border-radius: 8px;
            padding: 14px 30px; font-weight: bold;
            font-size: 15px; min-height: 50px; color: white;
        }
        QLabel { color: #1a1a1a; font-size: 14px; }
        QProgressBar {
            border: 2px solid #d0d0d0; border-radius: 8px;
            text-align: center; background-color: #f5f5f5;
            color: #000000; font-weight: bold;
            font-size: 13px; min-height: 38px;
        }
        QProgressBar::chunk { border-radius: 6px; }
        QTableWidget {
            border: 2px solid #d0d0d0; border-radius: 8px;
            background-color: white; gridline-color: #e0e0e0;
            font-size: 13px; color: #1a1a1a;
        }
        QTableWidget::item { padding: 12px; color: #1a1a1a; }
        QHeaderView::section {
            background-color: #1976D2; color: white;
            padding: 14px; border: none;
            font-weight: bold; font-size: 14px;
        }
        QTextEdit {
            border: 2px solid #d0d0d0; border-radius: 8px;
            background-color: #fafafa;
            font-family: 'Consolas', 'Courier New', monospace;
            font-size: 12px; color: #1a1a1a; padding: 10px;
        }
        QSpinBox, QComboBox {
            padding: 12px; border: 2px solid #1976D2;
            border-radius: 8px; background: white;
            font-size: 16px; color: #1a1a1a; min-height: 50px;
        }
    )");
    
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);
    
    welcomeScreen = createWelcomeScreen();
    configScreen = createConfigScreen();
    simulationScreen = createSimulationScreen();
    resultsScreen = createResultsScreen();
    
    stackedWidget->addWidget(welcomeScreen);
    stackedWidget->addWidget(configScreen);
    stackedWidget->addWidget(simulationScreen);
    stackedWidget->addWidget(resultsScreen);
}

void MainWindow::setupMenuBar()
{
    QMenuBar *menuBar = new QMenuBar(this);
    menuBar->setStyleSheet(
        "QMenuBar { background-color: white; color: #1a1a1a; padding: 8px; font-size: 13px; }"
        "QMenuBar::item { padding: 10px 15px; }"
        "QMenuBar::item:selected { background-color: #1976D2; color: white; }"
        "QMenu { background-color: white; color: #1a1a1a; border: 1px solid #d0d0d0; }"
        "QMenu::item { padding: 8px 30px; }"
        "QMenu::item:selected { background-color: #1976D2; color: white; }"
    );
    setMenuBar(menuBar);
    
    QMenu *fileMenu = menuBar->addMenu("&File");
    QAction *newSimAction = fileMenu->addAction("New Simulation");
    connect(newSimAction, &QAction::triggered, this, &MainWindow::showConfigScreen);
    fileMenu->addSeparator();
    QAction *exitAction = fileMenu->addAction("Exit");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
    
    QMenu *helpMenu = menuBar->addMenu("&Help");
    QAction *aboutAction = helpMenu->addAction("About");
    connect(aboutAction, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, "About",
            "<h2 style='color: #1976D2;'>Data Center Simulator</h2>"
            "<p style='color: #1a1a1a;'><b>Version:</b> 1.0<br>"
            "<b>OS Concepts:</b> Threading, Scheduling, Deadlock Prevention</p>");
    });
}

void MainWindow::setupStatusBar()
{
    statusBar()->showMessage("Ready");
    statusBar()->setStyleSheet(
        "QStatusBar { border-top: 2px solid #d0d0d0; padding: 10px; "
        "background-color: white; color: #1a1a1a; font-size: 13px; }"
    );
}

// ==================== WELCOME SCREEN ====================
QWidget* MainWindow::createWelcomeScreen()
{
    QWidget *screen = new QWidget(this);
    screen->setStyleSheet(
        "QWidget { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "stop:0 #1565C0, stop:1 #1E88E5); }"
    );
    
    QVBoxLayout *layout = new QVBoxLayout(screen);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(40);
    
    QLabel *iconLabel = new QLabel("🏢", screen);
    QFont iconFont;
    iconFont.setPointSize(100);
    iconLabel->setFont(iconFont);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet("color: white;");
    
    QLabel *title = new QLabel("Data Center\nResource Manager", screen);
    QFont titleFont;
    titleFont.setPointSize(52);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setStyleSheet("color: white;");
    title->setAlignment(Qt::AlignCenter);
    
    QLabel *subtitle = new QLabel("Operating System Concepts Simulation", screen);
    QFont subtitleFont;
    subtitleFont.setPointSize(22);
    subtitle->setFont(subtitleFont);
    subtitle->setStyleSheet("color: #E3F2FD;");
    subtitle->setAlignment(Qt::AlignCenter);
    
    btnGetStarted = new QPushButton("Get Started →", screen);
    btnGetStarted->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; "
        "font-size: 24px; padding: 22px 90px; border-radius: 12px; }"
        "QPushButton:hover { background-color: #45a049; }"
    );
    btnGetStarted->setCursor(Qt::PointingHandCursor);
    connect(btnGetStarted, &QPushButton::clicked, this, &MainWindow::showConfigScreen);
    
    layout->addStretch();
    layout->addWidget(iconLabel);
    layout->addWidget(title);
    layout->addWidget(subtitle);
    layout->addSpacing(60);
    layout->addWidget(btnGetStarted, 0, Qt::AlignCenter);
    layout->addStretch();
    
    return screen;
}

// ==================== CONFIG SCREEN ====================
QWidget* MainWindow::createConfigScreen()
{
    QWidget *screen = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(screen);
    mainLayout->setContentsMargins(100, 60, 100, 60);
    mainLayout->setSpacing(40);
    
    QLabel *header = new QLabel("⚙️ Configuration", screen);
    QFont headerFont;
    headerFont.setPointSize(40);
    headerFont.setBold(true);
    header->setFont(headerFont);
    header->setStyleSheet("color: #1976D2;");
    header->setAlignment(Qt::AlignCenter);
    
    QGroupBox *configGroup = new QGroupBox("Simulation Parameters", screen);
    QVBoxLayout *configLayout = new QVBoxLayout(configGroup);
    configLayout->setSpacing(40);
    configLayout->setContentsMargins(40, 50, 40, 40);
    
    // Jobs
    QHBoxLayout *jobLayout = new QHBoxLayout();
    QLabel *lblJobs = new QLabel("📋 Number of Jobs:", screen);
    QFont labelFont;
    labelFont.setPointSize(17);
    labelFont.setBold(true);
    lblJobs->setFont(labelFont);
    lblJobs->setStyleSheet("color: #1a1a1a;");
    
    spinJobCount = new QSpinBox(screen);
    spinJobCount->setRange(5, 50);
    spinJobCount->setValue(10);
    spinJobCount->setMinimumWidth(220);
    QFont spinFont;
    spinFont.setPointSize(18);
    spinJobCount->setFont(spinFont);
    
    jobLayout->addWidget(lblJobs);
    jobLayout->addStretch();
    jobLayout->addWidget(spinJobCount);
    
    // Algorithm
    QHBoxLayout *schedLayout = new QHBoxLayout();
    QLabel *lblSched = new QLabel("⚡ Algorithm:", screen);
    lblSched->setFont(labelFont);
    lblSched->setStyleSheet("color: #1a1a1a;");
    
    cmbScheduling = new QComboBox(screen);
    cmbScheduling->addItems({"Priority Scheduling"});
    cmbScheduling->setMinimumWidth(280);
    cmbScheduling->setFont(spinFont);
    
    schedLayout->addWidget(lblSched);
    schedLayout->addStretch();
    schedLayout->addWidget(cmbScheduling);
    
    configLayout->addLayout(jobLayout);
    configLayout->addLayout(schedLayout);
    
    // Info
    QLabel *info = new QLabel(
        "<p style='color: #1a1a1a; font-size: 15px; line-height: 1.8;'>"
        "<b style='color: #1976D2;'>System Resources:</b><br>"
        "• CPU: 16 cores<br>• RAM: 64 GB<br>• Disk: 8 slots<br>• Network: 4 channels</p>",
        screen
    );
    info->setStyleSheet("padding: 25px; background: #E3F2FD; border-radius: 10px; border-left: 5px solid #1976D2;");
    info->setWordWrap(true);
    configLayout->addWidget(info);
    
    // Buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(30);
    
    btnBackToWelcome = new QPushButton("← Back", screen);
    btnBackToWelcome->setStyleSheet(
        "QPushButton { background-color: #757575; color: white; font-size: 17px; "
        "padding: 16px 50px; border-radius: 10px; }"
        "QPushButton:hover { background-color: #616161; }"
    );
    btnBackToWelcome->setCursor(Qt::PointingHandCursor);
    connect(btnBackToWelcome, &QPushButton::clicked, this, &MainWindow::showWelcomeScreen);
    
    btnStartSimulation = new QPushButton("▶ Start", screen);
    btnStartSimulation->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; font-size: 19px; font-weight: bold; "
        "padding: 16px 70px; border-radius: 10px; }"
        "QPushButton:hover { background-color: #45a049; }"
    );
    btnStartSimulation->setCursor(Qt::PointingHandCursor);
    connect(btnStartSimulation, &QPushButton::clicked, this, &MainWindow::onStartSimulation);
    
    btnLayout->addWidget(btnBackToWelcome);
    btnLayout->addStretch();
    btnLayout->addWidget(btnStartSimulation);
    
    mainLayout->addWidget(header);
    mainLayout->addWidget(configGroup);
    mainLayout->addLayout(btnLayout);
    mainLayout->addStretch();
    
    return screen;
}

// ==================== SIMULATION SCREEN - FULLY VISIBLE ====================
QWidget* MainWindow::createSimulationScreen()
{
    QWidget *screen = new QWidget(this);
    
    // FIXED: Use scroll area to show everything
    QScrollArea *scrollArea = new QScrollArea(screen);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: none; background-color: #f0f2f5; }");
    
    QWidget *contentWidget = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(contentWidget);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(25, 25, 25, 25);
    
    // Control Panel
    QGroupBox *controlGroup = new QGroupBox("⚙️ Simulation Controls", contentWidget);
    controlGroup->setMaximumHeight(120);
    QHBoxLayout *controlLayout = new QHBoxLayout(controlGroup);
    controlLayout->setContentsMargins(20, 30, 20, 20);
    
    QLabel *statusLabel = new QLabel("🟢 Status: Running", contentWidget);
    QFont statusFont;
    statusFont.setPointSize(15);
    statusFont.setBold(true);
    statusLabel->setFont(statusFont);
    statusLabel->setStyleSheet("color: #4CAF50;");
    
    btnStop = new QPushButton("⏹ Stop Simulation", contentWidget);
    btnStop->setStyleSheet(
        "QPushButton { background-color: #f44336; color: white; font-size: 15px; font-weight: bold; "
        "padding: 14px 35px; border-radius: 8px; min-width: 160px; }"
        "QPushButton:hover { background-color: #d32f2f; }"
    );
    btnStop->setCursor(Qt::PointingHandCursor);
    connect(btnStop, &QPushButton::clicked, this, &MainWindow::onStopSimulation);
    
    controlLayout->addWidget(statusLabel);
    controlLayout->addStretch();
    controlLayout->addWidget(btnStop);
    mainLayout->addWidget(controlGroup);
    
    // Resources and Metrics Side by Side
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->setSpacing(20);
    
    // Resources Group
    QGroupBox *resGroup = new QGroupBox("📊 Resource Utilization", contentWidget);
    resGroup->setMinimumHeight(380);
    resGroup->setMaximumHeight(420);
    QVBoxLayout *resLayout = new QVBoxLayout(resGroup);
    resLayout->setSpacing(15);
    resLayout->setContentsMargins(20, 35, 20, 20);
    
    QFont barLabelFont;
    barLabelFont.setPointSize(13);
    barLabelFont.setBold(true);
    
    // CPU
    QLabel *lblCPU = new QLabel("🔷 CPU Cores (16 total)", contentWidget);
    lblCPU->setFont(barLabelFont);
    lblCPU->setStyleSheet("color: #1a1a1a;");
    cpuUsageBar = new QProgressBar(contentWidget);
    cpuUsageBar->setMaximum(100);
    cpuUsageBar->setValue(0);
    cpuUsageBar->setFormat("%p%");
    cpuUsageBar->setStyleSheet("QProgressBar::chunk { background-color: #2196F3; }");
    cpuUsageBar->setMinimumHeight(35);
    
    // RAM
    QLabel *lblRAM = new QLabel("🔶 RAM Memory (64 GB)", contentWidget);
    lblRAM->setFont(barLabelFont);
    lblRAM->setStyleSheet("color: #1a1a1a;");
    ramUsageBar = new QProgressBar(contentWidget);
    ramUsageBar->setMaximum(100);
    ramUsageBar->setValue(0);
    ramUsageBar->setFormat("%p%");
    ramUsageBar->setStyleSheet("QProgressBar::chunk { background-color: #FF9800; }");
    ramUsageBar->setMinimumHeight(35);
    
    // Disk
    QLabel *lblDisk = new QLabel("💾 Disk I/O (8 slots)", contentWidget);
    lblDisk->setFont(barLabelFont);
    lblDisk->setStyleSheet("color: #1a1a1a;");
    diskUsageBar = new QProgressBar(contentWidget);
    diskUsageBar->setMaximum(100);
    diskUsageBar->setValue(0);
    diskUsageBar->setFormat("%p%");
    diskUsageBar->setStyleSheet("QProgressBar::chunk { background-color: #9C27B0; }");
    diskUsageBar->setMinimumHeight(35);
    
    // Network
    QLabel *lblNet = new QLabel("🌐 Network (4 channels)", contentWidget);
    lblNet->setFont(barLabelFont);
    lblNet->setStyleSheet("color: #1a1a1a;");
    networkUsageBar = new QProgressBar(contentWidget);
    networkUsageBar->setMaximum(100);
    networkUsageBar->setValue(0);
    networkUsageBar->setFormat("%p%");
    networkUsageBar->setStyleSheet("QProgressBar::chunk { background-color: #4CAF50; }");
    networkUsageBar->setMinimumHeight(35);
    
    resLayout->addWidget(lblCPU);
    resLayout->addWidget(cpuUsageBar);
    resLayout->addWidget(lblRAM);
    resLayout->addWidget(ramUsageBar);
    resLayout->addWidget(lblDisk);
    resLayout->addWidget(diskUsageBar);
    resLayout->addWidget(lblNet);
    resLayout->addWidget(networkUsageBar);
    resLayout->addStretch();
    
    // Metrics Group
    QGroupBox *metGroup = new QGroupBox("📈 Performance Metrics", contentWidget);
    metGroup->setMinimumHeight(380);
    metGroup->setMaximumHeight(420);
    QVBoxLayout *metLayout = new QVBoxLayout(metGroup);
    metLayout->setSpacing(18);
    metLayout->setContentsMargins(20, 35, 20, 20);
    
    QFont metFont;
    metFont.setPointSize(14);
    
    lblTotalJobs = new QLabel("📋 Total Jobs: 0", contentWidget);
    lblTotalJobs->setFont(metFont);
    lblTotalJobs->setStyleSheet("color: #1a1a1a;");
    
    lblActiveJobs = new QLabel("🔄 Active Jobs: 0", contentWidget);
    lblActiveJobs->setFont(metFont);
    lblActiveJobs->setStyleSheet("color: #2196F3; font-weight: bold;");
    
    lblCompletedJobs = new QLabel("✅ Completed: 0", contentWidget);
    lblCompletedJobs->setFont(metFont);
    lblCompletedJobs->setStyleSheet("color: #4CAF50; font-weight: bold;");
    
    QFrame *sep1 = new QFrame(contentWidget);
    sep1->setFrameShape(QFrame::HLine);
    sep1->setStyleSheet("background-color: #d0d0d0; min-height: 2px; margin: 5px 0;");
    
    lblAvgWaitTime = new QLabel("⏱️ Avg Wait: 0.00s", contentWidget);
    lblAvgWaitTime->setFont(metFont);
    lblAvgWaitTime->setStyleSheet("color: #1a1a1a;");
    
    lblThroughput = new QLabel("⚡ Throughput: 0.00 jobs/s", contentWidget);
    lblThroughput->setFont(metFont);
    lblThroughput->setStyleSheet("color: #1a1a1a;");
    
    QFrame *sep2 = new QFrame(contentWidget);
    sep2->setFrameShape(QFrame::HLine);
    sep2->setStyleSheet("background-color: #d0d0d0; min-height: 2px; margin: 5px 0;");
    
    lblDeadlocksPrevented = new QLabel("🛡️ Deadlocks Prevented: 0", contentWidget);
    lblDeadlocksPrevented->setFont(metFont);
    lblDeadlocksPrevented->setStyleSheet("color: #FF5722; font-weight: bold;");
    
    metLayout->addWidget(lblTotalJobs);
    metLayout->addWidget(lblActiveJobs);
    metLayout->addWidget(lblCompletedJobs);
    metLayout->addWidget(sep1);
    metLayout->addWidget(lblAvgWaitTime);
    metLayout->addWidget(lblThroughput);
    metLayout->addWidget(sep2);
    metLayout->addWidget(lblDeadlocksPrevented);
    metLayout->addStretch();
    
    topLayout->addWidget(resGroup, 1);
    topLayout->addWidget(metGroup, 1);
    mainLayout->addLayout(topLayout);
    
    // Job Table
    QGroupBox *tableGroup = new QGroupBox("📊 Active Jobs", contentWidget);
    tableGroup->setMinimumHeight(280);
    QVBoxLayout *tableLayout = new QVBoxLayout(tableGroup);
    tableLayout->setContentsMargins(15, 30, 15, 15);
    
    jobTable = new QTableWidget(contentWidget);
    jobTable->setColumnCount(6);
    jobTable->setHorizontalHeaderLabels({"Job ID", "Priority", "Status", "CPU", "RAM (GB)", "Wait (s)"});
    jobTable->horizontalHeader()->setStretchLastSection(true);
    jobTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    jobTable->setAlternatingRowColors(true);
    jobTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    jobTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableLayout->addWidget(jobTable);
    mainLayout->addWidget(tableGroup);
    
    // Event Log
    QGroupBox *logGroup = new QGroupBox("📜 System Event Log", contentWidget);
    logGroup->setMinimumHeight(220);
    logGroup->setMaximumHeight(250);
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    logLayout->setContentsMargins(15, 30, 15, 15);
    
    logView = new QTextEdit(contentWidget);
    logView->setReadOnly(true);
    logLayout->addWidget(logView);
    mainLayout->addWidget(logGroup);
    
    scrollArea->setWidget(contentWidget);
    
    QVBoxLayout *screenLayout = new QVBoxLayout(screen);
    screenLayout->setContentsMargins(0, 0, 0, 0);
    screenLayout->addWidget(scrollArea);
    
    return screen;
}

// ==================== RESULTS SCREEN - FIXED ====================
QWidget* MainWindow::createResultsScreen()
{
    QWidget *screen = new QWidget(this);
    
    // Use scroll area to prevent overlapping
    QScrollArea *scrollArea = new QScrollArea(screen);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: none; background-color: #f0f2f5; }");
    
    QWidget *contentWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(contentWidget);
    layout->setSpacing(30);
    layout->setContentsMargins(80, 60, 80, 60);
    
    // Icon
    QLabel *icon = new QLabel("✅", contentWidget);
    QFont iconFont;
    iconFont.setPointSize(80);
    icon->setFont(iconFont);
    icon->setAlignment(Qt::AlignCenter);
    icon->setStyleSheet("color: #4CAF50;");
    
    // Header
    QLabel *header = new QLabel("Simulation Complete!", contentWidget);
    QFont headerFont;
    headerFont.setPointSize(38);
    headerFont.setBold(true);
    header->setFont(headerFont);
    header->setStyleSheet("color: #4CAF50;");
    header->setAlignment(Qt::AlignCenter);
    
    // Stats Group
    QGroupBox *statsGroup = new QGroupBox("📊 Results", contentWidget);
    statsGroup->setMinimumHeight(520);
    QVBoxLayout *statsLayout = new QVBoxLayout(statsGroup);
    statsLayout->setSpacing(18);
    statsLayout->setContentsMargins(35, 45, 35, 35);
    
    QFont statsFont;
    statsFont.setPointSize(15);
    
    lblFinalTotalJobs = new QLabel("Total Jobs: 0", contentWidget);
    lblFinalTotalJobs->setFont(statsFont);
    lblFinalTotalJobs->setStyleSheet("color: #1a1a1a;");
    
    lblFinalCompletedJobs = new QLabel("Completed: 0", contentWidget);
    lblFinalCompletedJobs->setFont(statsFont);
    lblFinalCompletedJobs->setStyleSheet("color: #4CAF50; font-weight: bold;");
    
    lblFinalAvgWaitTime = new QLabel("Avg Wait: 0.00s", contentWidget);
    lblFinalAvgWaitTime->setFont(statsFont);
    lblFinalAvgWaitTime->setStyleSheet("color: #1a1a1a;");
    
    lblFinalAvgTurnaroundTime = new QLabel("Avg Turnaround: 0.00s", contentWidget);
    lblFinalAvgTurnaroundTime->setFont(statsFont);
    lblFinalAvgTurnaroundTime->setStyleSheet("color: #1a1a1a;");
    
    lblFinalThroughput = new QLabel("Throughput: 0.00", contentWidget);
    lblFinalThroughput->setFont(statsFont);
    lblFinalThroughput->setStyleSheet("color: #1a1a1a;");
    
    QFrame *sep1 = new QFrame(contentWidget);
    sep1->setFrameShape(QFrame::HLine);
    sep1->setStyleSheet("background-color: #bbb; min-height: 2px; margin: 10px 0;");
    
    lblFinalCPUUtil = new QLabel("CPU: 0.00%", contentWidget);
    lblFinalCPUUtil->setFont(statsFont);
    lblFinalCPUUtil->setStyleSheet("color: #1a1a1a;");
    
    lblFinalRAMUtil = new QLabel("RAM: 0.00%", contentWidget);
    lblFinalRAMUtil->setFont(statsFont);
    lblFinalRAMUtil->setStyleSheet("color: #1a1a1a;");
    
    lblFinalDiskUtil = new QLabel("Disk: 0.00%", contentWidget);
    lblFinalDiskUtil->setFont(statsFont);
    lblFinalDiskUtil->setStyleSheet("color: #1a1a1a;");
    
    lblFinalNetworkUtil = new QLabel("Network: 0.00%", contentWidget);
    lblFinalNetworkUtil->setFont(statsFont);
    lblFinalNetworkUtil->setStyleSheet("color: #1a1a1a;");
    
    QFrame *sep2 = new QFrame(contentWidget);
    sep2->setFrameShape(QFrame::HLine);
    sep2->setStyleSheet("background-color: #bbb; min-height: 2px; margin: 10px 0;");
    
    lblFinalDeadlocksPrevented = new QLabel("Deadlocks Prevented: 0", contentWidget);
    lblFinalDeadlocksPrevented->setFont(statsFont);
    lblFinalDeadlocksPrevented->setStyleSheet("color: #FF5722; font-weight: bold;");
    
    statsLayout->addWidget(lblFinalTotalJobs);
    statsLayout->addWidget(lblFinalCompletedJobs);
    statsLayout->addSpacing(10);
    statsLayout->addWidget(lblFinalAvgWaitTime);
    statsLayout->addWidget(lblFinalAvgTurnaroundTime);
    statsLayout->addWidget(lblFinalThroughput);
    statsLayout->addWidget(sep1);
    statsLayout->addWidget(lblFinalCPUUtil);
    statsLayout->addWidget(lblFinalRAMUtil);
    statsLayout->addWidget(lblFinalDiskUtil);
    statsLayout->addWidget(lblFinalNetworkUtil);
    statsLayout->addWidget(sep2);
    statsLayout->addWidget(lblFinalDeadlocksPrevented);
    
    // Log location info
    QString logsPath = QDir::currentPath() + "/logs";
    QLabel *logInfo = new QLabel(
        QString("<p style='color: #1a1a1a; font-size: 14px; line-height: 1.7;'>"
                "<b style='color: #1976D2;'>📁 Log Files Location:</b><br>"
                "<code style='background: #e0e0e0; padding: 5px; border-radius: 4px;'>%1</code><br><br>"
                "<b>Files:</b> system.log, job_*.log</p>").arg(logsPath),
        contentWidget
    );
    logInfo->setStyleSheet("padding: 20px; background: #E3F2FD; border-radius: 8px; border-left: 4px solid #1976D2;");
    logInfo->setWordWrap(true);
    
    // Buttons - FIXED SPACING
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(25);
    btnLayout->setContentsMargins(0, 20, 0, 0);
    
    btnViewLogs = new QPushButton("📄 View Logs", contentWidget);
    btnViewLogs->setStyleSheet(
        "QPushButton { background-color: #2196F3; color: white; font-size: 16px; font-weight: bold; "
        "padding: 16px 45px; border-radius: 10px; }"
        "QPushButton:hover { background-color: #1976D2; }"
    );
    btnViewLogs->setCursor(Qt::PointingHandCursor);
    connect(btnViewLogs, &QPushButton::clicked, this, &MainWindow::onViewLogs);
    
    btnNewSimulation = new QPushButton("🔄 New", contentWidget);
    btnNewSimulation->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; font-size: 16px; font-weight: bold; "
        "padding: 16px 45px; border-radius: 10px; }"
        "QPushButton:hover { background-color: #45a049; }"
    );
    btnNewSimulation->setCursor(Qt::PointingHandCursor);
    connect(btnNewSimulation, &QPushButton::clicked, this, &MainWindow::onNewSimulation);
    
    btnExit = new QPushButton("❌ Exit", contentWidget);
    btnExit->setStyleSheet(
        "QPushButton { background-color: #757575; color: white; font-size: 16px; font-weight: bold; "
        "padding: 16px 45px; border-radius: 10px; }"
        "QPushButton:hover { background-color: #616161; }"
    );
    btnExit->setCursor(Qt::PointingHandCursor);
    connect(btnExit, &QPushButton::clicked, this, &QMainWindow::close);
    
    btnLayout->addStretch();
    btnLayout->addWidget(btnViewLogs);
    btnLayout->addWidget(btnNewSimulation);
    btnLayout->addWidget(btnExit);
    btnLayout->addStretch();
    
    layout->addWidget(icon);
    layout->addWidget(header);
    layout->addSpacing(20);
    layout->addWidget(statsGroup);
    layout->addSpacing(15);
    layout->addWidget(logInfo);
    layout->addSpacing(20);
    layout->addLayout(btnLayout);
    layout->addStretch();
    
    scrollArea->setWidget(contentWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(screen);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(scrollArea);
    
    return screen;
}

// ==================== NAVIGATION ====================
void MainWindow::showWelcomeScreen()
{
    stackedWidget->setCurrentWidget(welcomeScreen);
    statusBar()->showMessage("Ready");
}

void MainWindow::showConfigScreen()
{
    stackedWidget->setCurrentWidget(configScreen);
    statusBar()->showMessage("Configure parameters");
}

void MainWindow::showSimulationScreen()
{
    stackedWidget->setCurrentWidget(simulationScreen);
    statusBar()->showMessage("Running simulation...");
}

void MainWindow::showResultsScreen()
{
    stackedWidget->setCurrentWidget(resultsScreen);
    statusBar()->showMessage("Completed");
}

// ==================== SIMULATION CONTROL ====================
void MainWindow::onStartSimulation()
{
    int numJobs = spinJobCount->value();
    
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Starting Simulation");
    msgBox.setText(QString("<h3 style='color: #1976D2;'>Starting Simulation</h3>"
                          "<p style='color: #1a1a1a;'><b>Jobs:</b> %1</p>").arg(numJobs));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
    
    showSimulationScreen();
    
    jobTable->setRowCount(0);
    logView->clear();
    cpuUsageBar->setValue(0);
    ramUsageBar->setValue(0);
    diskUsageBar->setValue(0);
    networkUsageBar->setValue(0);
    
    lblTotalJobs->setText(QString("📋 Total: %1").arg(numJobs));
    lblActiveJobs->setText("🔄 Active: 0");
    lblCompletedJobs->setText("✅ Completed: 0");
    lblAvgWaitTime->setText("⏱️ Wait: 0.00s");
    lblThroughput->setText("⚡ Throughput: 0.00");
    lblDeadlocksPrevented->setText("🛡️ Deadlocks: 0");
    
    isRunning = true;
    stopRequested = false;
    updateTimer->start(100);  // FASTER: Update every 100ms
    
    if (simulationThread.joinable()) {
        simulationThread.join();
    }
    simulationThread = std::thread(&MainWindow::runSimulation, this, numJobs);
}

void MainWindow::onStopSimulation()
{
    stopRequested = true;
    isRunning = false;
    updateTimer->stop();
    
    QMessageBox::information(this, "Stopped",
        "<p style='color: #1a1a1a; font-size: 14px;'>Simulation stopped.</p>");
    
    if (simulationThread.joinable()) {
        simulationThread.join();
    }
    showResultsScreen();
    displayFinalResults();
}

void MainWindow::onNewSimulation()
{
    cleanupSimulation();
    showConfigScreen();
}

void MainWindow::onViewLogs()
{
    QString logsPath = QDir::currentPath() + "/logs";
    QDesktopServices::openUrl(QUrl::fromLocalFile(logsPath));
}

// ==================== SIMULATION - FIXED FOR REAL-TIME DISPLAY ====================
void MainWindow::runSimulation(int numJobs)
{
    try {
        cleanupSimulation();
        addLogMessage("System", "Initializing...");
        
        resourceManager = new ResourceManager(16, 64, 8, 4);
        deadlockManager = new DeadlockManager(resourceManager);
        scheduler = new Scheduler(resourceManager, deadlockManager);
        
        addLogMessage("System", QString("Creating %1 jobs...").arg(numJobs));
        
        std::random_device rd;
        std::mt19937 gen(rd());
        
        // Create jobs
        std::vector<Job*> allJobs;
        for (int i = 1; i <= numJobs && !stopRequested; i++) {
            std::uniform_int_distribution<> prioDist(1, 3);
            JobPriority priority = static_cast<JobPriority>(prioDist(gen));
            
            std::uniform_int_distribution<> cpuDist(1, 4);
            std::uniform_int_distribution<> ramDist(2, 16);
            std::uniform_int_distribution<> diskDist(1, 3);
            std::uniform_int_distribution<> netDist(1, 2);
            
            int cpuMultiplier = (priority == JobPriority::HIGH) ? 2 : 1;
            
            ResourceRequest needs(cpuDist(gen) * cpuMultiplier, ramDist(gen), 
                                 diskDist(gen), netDist(gen));
            
            Job* job = new Job(i, priority, needs, resourceManager, deadlockManager);
            scheduler->addJob(job);
            allJobs.push_back(job);
            
            QString priorityStr;
            switch (priority) {
                case JobPriority::HIGH: priorityStr = "HIGH"; break;
                case JobPriority::MEDIUM: priorityStr = "MEDIUM"; break;
                case JobPriority::LOW: priorityStr = "LOW"; break;
            }
            
            addLogMessage(QString("Job %1").arg(i), 
                QString("Created with %1 priority (CPU:%2, RAM:%3GB)")
                .arg(priorityStr).arg(needs.cpuCores).arg(needs.ramGB));
            
            addJobToTable(i, priorityStr, "WAITING", needs.cpuCores, needs.ramGB);
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }
        
        if (!stopRequested) {
            addLogMessage("System", "Starting scheduling...");
            scheduler->scheduleAll();
            
            // Monitor jobs in real-time
            int lastCompleted = 0;
            while (isRunning && !stopRequested) {
                int completed = scheduler->getCompletedJobs();
                
                // Update job statuses
                for (size_t i = 0; i < allJobs.size(); i++) {
                    Job* job = allJobs[i];
                    QString status;
                    
                    switch (job->getStatus()) {
                        case JobStatus::WAITING: status = "WAITING"; break;
                        case JobStatus::RUNNING: 
                            status = "RUNNING";
                            if (lastCompleted < (int)i + 1) {
                                addLogMessage(QString("Job %1").arg(job->getId()), "Started");
                            }
                            break;
                        case JobStatus::BLOCKED: status = "BLOCKED"; break;
                        case JobStatus::COMPLETED: 
                            status = "COMPLETED";
                            break;
                    }
                    
                    updateJobStatus(job->getId(), status);
                }
                
                // Check completion
                if (completed > lastCompleted) {
                    for (int i = lastCompleted + 1; i <= completed; i++) {
                        addLogMessage(QString("Job %1").arg(i), "Completed");
                    }
                    lastCompleted = completed;
                }
                
                if (completed >= numJobs) {
                    break;
                }
                
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }
            
            scheduler->waitForAllJobs();
            addLogMessage("System", "All jobs completed!");
        }
        
        isRunning = false;
        updateTimer->stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        QMetaObject::invokeMethod(this, [this]() {
            showResultsScreen();
            displayFinalResults();
        }, Qt::QueuedConnection);
        
    } catch (const std::exception& e) {
        addLogMessage("ERROR", QString("Error: %1").arg(e.what()));
        isRunning = false;
        updateTimer->stop();
    }
}

void MainWindow::updateUI()
{
    if (!resourceManager || !scheduler) return;
    
    // Get PEAK resource usage (not instant)
    static int peakCPU = 0, peakRAM = 0, peakDisk = 0, peakNet = 0;
    
    int cpu = static_cast<int>(resourceManager->getCPUUtilization());
    int ram = static_cast<int>(resourceManager->getRAMUtilization());
    int disk = static_cast<int>(resourceManager->getDiskUtilization());
    int net = static_cast<int>(resourceManager->getNetworkUtilization());
    
    // Track peaks
    if (cpu > peakCPU) peakCPU = cpu;
    if (ram > peakRAM) peakRAM = ram;
    if (disk > peakDisk) peakDisk = disk;
    if (net > peakNet) peakNet = net;
    
    // Show peak values (they won't drop to 0 immediately)
    cpuUsageBar->setValue(peakCPU);
    ramUsageBar->setValue(peakRAM);
    diskUsageBar->setValue(peakDisk);
    networkUsageBar->setValue(peakNet);
    
    int active = scheduler->getTotalJobs() - scheduler->getCompletedJobs();
    int completed = scheduler->getCompletedJobs();
    
    lblActiveJobs->setText(QString("🔄 Active: %1").arg(active));
    lblCompletedJobs->setText(QString("✅ Completed: %1").arg(completed));
    
    if (completed > 0) {
        lblAvgWaitTime->setText(QString("⏱️ Wait: %1s")
            .arg(scheduler->getAverageWaitingTime(), 0, 'f', 2));
        lblThroughput->setText(QString("⚡ Throughput: %1")
            .arg(scheduler->getThroughput(), 0, 'f', 2));
    }
    
    lblDeadlocksPrevented->setText(QString("🛡️ Deadlocks: %1")
        .arg(deadlockManager->getDeadlocksPrevented()));
    
    // Log high usage
    static int lastLoggedCPU = 0;
    if (peakCPU >= 50 && peakCPU != lastLoggedCPU && peakCPU % 25 == 0) {
        addLogMessage("System", QString("CPU peak: %1%").arg(peakCPU));
        lastLoggedCPU = peakCPU;
    }
}

void MainWindow::displayFinalResults()
{
    if (!scheduler || !resourceManager || !deadlockManager) return;
    
    lblFinalTotalJobs->setText(QString("Total Jobs: %1").arg(scheduler->getTotalJobs()));
    lblFinalCompletedJobs->setText(QString("Completed: %1").arg(scheduler->getCompletedJobs()));
    lblFinalAvgWaitTime->setText(QString("Avg Wait: %1s").arg(scheduler->getAverageWaitingTime(), 0, 'f', 2));
    lblFinalAvgTurnaroundTime->setText(QString("Avg Turnaround: %1s").arg(scheduler->getAverageTurnaroundTime(), 0, 'f', 2));
    lblFinalThroughput->setText(QString("Throughput: %1 jobs/s").arg(scheduler->getThroughput(), 0, 'f', 2));
    lblFinalCPUUtil->setText(QString("CPU: %1%").arg(resourceManager->getCPUUtilization(), 0, 'f', 2));
    lblFinalRAMUtil->setText(QString("RAM: %1%").arg(resourceManager->getRAMUtilization(), 0, 'f', 2));
    lblFinalDiskUtil->setText(QString("Disk: %1%").arg(resourceManager->getDiskUtilization(), 0, 'f', 2));
    lblFinalNetworkUtil->setText(QString("Network: %1%").arg(resourceManager->getNetworkUtilization(), 0, 'f', 2));
    lblFinalDeadlocksPrevented->setText(QString("Deadlocks Prevented: %1").arg(deadlockManager->getDeadlocksPrevented()));
}

void MainWindow::addLogMessage(const QString& source, const QString& message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString logEntry = QString("<span style='color: #666;'>[%1]</span> "
                               "<span style='color: #1976D2; font-weight: bold;'>%2:</span> "
                               "<span style='color: #1a1a1a;'>%3</span>")
                        .arg(timestamp, source, message);
    
    QMetaObject::invokeMethod(this, [this, logEntry]() {
        logView->append(logEntry);
    }, Qt::QueuedConnection);
}

// ==================== HELPER FUNCTIONS FOR JOB TABLE ====================
void MainWindow::addJobToTable(int jobId, const QString& priority, const QString& status,
                                int cpuCores, int ramGB)
{
    QMetaObject::invokeMethod(this, [this, jobId, priority, status, cpuCores, ramGB]() {
        int row = jobTable->rowCount();
        jobTable->insertRow(row);
        
        auto* idItem = new QTableWidgetItem(QString::number(jobId));
        idItem->setTextAlignment(Qt::AlignCenter);
        
        auto* prioItem = new QTableWidgetItem(priority);
        prioItem->setTextAlignment(Qt::AlignCenter);
        if (priority == "HIGH") {
            prioItem->setForeground(QColor("#f44336"));
            prioItem->setFont(QFont("", -1, QFont::Bold));
        } else if (priority == "MEDIUM") {
            prioItem->setForeground(QColor("#FF9800"));
        }
        
        auto* statusItem = new QTableWidgetItem(status);
        statusItem->setTextAlignment(Qt::AlignCenter);
        statusItem->setForeground(QColor("#2196F3"));
        
        auto* cpuItem = new QTableWidgetItem(QString::number(cpuCores));
        cpuItem->setTextAlignment(Qt::AlignCenter);
        
        auto* ramItem = new QTableWidgetItem(QString::number(ramGB));
        ramItem->setTextAlignment(Qt::AlignCenter);
        
        auto* waitItem = new QTableWidgetItem("0.00");
        waitItem->setTextAlignment(Qt::AlignCenter);
        
        jobTable->setItem(row, 0, idItem);
        jobTable->setItem(row, 1, prioItem);
        jobTable->setItem(row, 2, statusItem);
        jobTable->setItem(row, 3, cpuItem);
        jobTable->setItem(row, 4, ramItem);
        jobTable->setItem(row, 5, waitItem);
        
        jobTable->scrollToBottom();
    }, Qt::QueuedConnection);
}

void MainWindow::updateJobStatus(int jobId, const QString& newStatus)
{
    QMetaObject::invokeMethod(this, [this, jobId, newStatus]() {
        for (int row = 0; row < jobTable->rowCount(); row++) {
            auto* idItem = jobTable->item(row, 0);
            if (idItem && idItem->text().toInt() == jobId) {
                auto* statusItem = jobTable->item(row, 2);
                if (statusItem && statusItem->text() != newStatus) {
                    statusItem->setText(newStatus);
                    
                    if (newStatus == "RUNNING") {
                        statusItem->setForeground(QColor("#4CAF50"));
                        statusItem->setFont(QFont("", -1, QFont::Bold));
                    } else if (newStatus == "COMPLETED") {
                        statusItem->setForeground(QColor("#757575"));
                    }
                }
                break;
            }
        }
    }, Qt::QueuedConnection);
}

void MainWindow::cleanupSimulation()
{
    if (updateTimer && updateTimer->isActive()) {
        updateTimer->stop();
    }
    if (scheduler) {
        delete scheduler;
        scheduler = nullptr;
    }
    if (deadlockManager) {
        delete deadlockManager;
        deadlockManager = nullptr;
    }
    if (resourceManager) {
        delete resourceManager;
        resourceManager = nullptr;
    }
}