#include "mainwindow.h"
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
#include <QFrame>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , isRunning(false)
    , isPaused(false)
    , totalJobs(0)
    , completedJobs(0)
    , activeJobs(0)
    , simulationCounter(0)
    , configCPUCores(8)
    , configRAMSize(32)
    , configDiskOps(100)
    , configNetworkSpeed(1000)
{
    setupUI();
    setupMenuBar();
    setupStatusBar();
    
    // Initialize timer for UI updates
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &MainWindow::updateUI);
    
    // Start with welcome screen
    showWelcomeScreen();
}

MainWindow::~MainWindow()
{
    cleanupSimulation();
}

void MainWindow::setupUI()
{
    setWindowTitle("Data Center Resource Management System");
    setMinimumSize(1200, 800);
    resize(1400, 900);
    
    // Create stacked widget for multiple screens
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);
    
    // Create all screens
    welcomeScreen = createWelcomeScreen();
    configScreen = createConfigScreen();
    simulationScreen = createSimulationScreen();
    resultsScreen = createResultsScreen();
    
    // Add screens to stack
    stackedWidget->addWidget(welcomeScreen);
    stackedWidget->addWidget(configScreen);
    stackedWidget->addWidget(simulationScreen);
    stackedWidget->addWidget(resultsScreen);
}

// ==================== WELCOME SCREEN ====================
QWidget* MainWindow::createWelcomeScreen()
{
    QWidget *screen = new QWidget(this);
    screen->setStyleSheet("QWidget { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #1a1a2e, stop:1 #16213e); }");
    
    QVBoxLayout *layout = new QVBoxLayout(screen);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(30);
    
    // Logo/Icon placeholder
    QLabel *iconLabel = new QLabel("🏢", screen);
    iconLabel->setStyleSheet("font-size: 80px;");
    iconLabel->setAlignment(Qt::AlignCenter);
    
    // Title
    QLabel *title = new QLabel("Data Center Resource\nManagement System", screen);
    title->setStyleSheet(
        "font-size: 36px; "
        "font-weight: bold; "
        "color: white; "
        "padding: 20px;"
    );
    title->setAlignment(Qt::AlignCenter);
    
    // Subtitle
    QLabel *subtitle = new QLabel("Operating System Concepts Simulation", screen);
    subtitle->setStyleSheet(
        "font-size: 18px; "
        "color: #b0b0b0; "
        "padding: 10px;"
    );
    subtitle->setAlignment(Qt::AlignCenter);
    
    // Feature list
    QLabel *features = new QLabel(
        "✓ Multi-threaded Job Execution\n"
        "✓ Priority-based CPU Scheduling\n"
        "✓ Deadlock Prevention (Banker's Algorithm)\n"
        "✓ Real-time Resource Monitoring\n"
        "✓ Performance Metrics & Analytics",
        screen
    );
    features->setStyleSheet(
        "font-size: 14px; "
        "color: #e0e0e0; "
        "padding: 20px; "
        "line-height: 1.8;"
    );
    features->setAlignment(Qt::AlignCenter);
    
    // Get Started button
    btnGetStarted = new QPushButton("Get Started →", screen);
    btnGetStarted->setStyleSheet(
        "QPushButton { "
        "  background-color: #4CAF50; "
        "  color: white; "
        "  font-size: 18px; "
        "  font-weight: bold; "
        "  padding: 15px 50px; "
        "  border-radius: 8px; "
        "  border: none; "
        "} "
        "QPushButton:hover { "
        "  background-color: #45a049; "
        "} "
        "QPushButton:pressed { "
        "  background-color: #3d8b40; "
        "}"
    );
    btnGetStarted->setCursor(Qt::PointingHandCursor);
    btnGetStarted->setMinimumWidth(250);
    connect(btnGetStarted, &QPushButton::clicked, this, &MainWindow::showConfigScreen);
    
    // Version
    QLabel *version = new QLabel("Version 1.0 | Created for OS Course", screen);
    version->setStyleSheet("font-size: 11px; color: #707070; padding: 20px;");
    version->setAlignment(Qt::AlignCenter);
    
    layout->addStretch();
    layout->addWidget(iconLabel);
    layout->addWidget(title);
    layout->addWidget(subtitle);
    layout->addSpacing(20);
    layout->addWidget(features);
    layout->addSpacing(30);
    layout->addWidget(btnGetStarted, 0, Qt::AlignCenter);
    layout->addSpacing(20);
    layout->addWidget(version);
    layout->addStretch();
    
    return screen;
}

// ==================== CONFIG SCREEN ====================
QWidget* MainWindow::createConfigScreen()
{
    QWidget *screen = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(screen);
    mainLayout->setContentsMargins(50, 30, 50, 30);
    mainLayout->setSpacing(20);
    
    // Header
    QLabel *header = new QLabel("⚙️ Simulation Configuration", screen);
    header->setStyleSheet("font-size: 24px; font-weight: bold; color: #2196F3; padding: 10px;");
    header->setAlignment(Qt::AlignCenter);
    
    QLabel *subtitle = new QLabel("Configure the parameters for your data center simulation", screen);
    subtitle->setStyleSheet("font-size: 13px; color: #666; padding-bottom: 20px;");
    subtitle->setAlignment(Qt::AlignCenter);
    
    // Configuration form
    QGroupBox *configGroup = new QGroupBox("Simulation Parameters", screen);
    configGroup->setStyleSheet(
        "QGroupBox { "
        "  font-weight: bold; "
        "  font-size: 14px; "
        "  border: 2px solid #ddd; "
        "  border-radius: 8px; "
        "  margin-top: 10px; "
        "  padding: 20px; "
        "} "
        "QGroupBox::title { "
        "  subcontrol-origin: margin; "
        "  left: 10px; "
        "  padding: 0 5px; "
        "}"
    );
    
    QFormLayout *formLayout = new QFormLayout(configGroup);
    formLayout->setSpacing(15);
    formLayout->setContentsMargins(20, 30, 20, 20);
    
    // Number of jobs
    spinJobCount = new QSpinBox(screen);
    spinJobCount->setRange(1, 100);
    spinJobCount->setValue(15);
    spinJobCount->setMinimumHeight(35);
    spinJobCount->setStyleSheet("font-size: 13px; padding: 5px;");
    formLayout->addRow("📋 Number of Jobs:", spinJobCount);
    
    // Priority distribution
    cmbPriorityDist = new QComboBox(screen);
    cmbPriorityDist->addItems({"Balanced", "High Priority Heavy", "Low Priority Heavy", "Random"});
    cmbPriorityDist->setMinimumHeight(35);
    cmbPriorityDist->setStyleSheet("font-size: 13px; padding: 5px;");
    formLayout->addRow("⚖️ Priority Distribution:", cmbPriorityDist);
    
    // Separator
    QFrame *line1 = new QFrame(screen);
    line1->setFrameShape(QFrame::HLine);
    line1->setStyleSheet("background: #ddd;");
    formLayout->addRow(line1);
    
    QLabel *resourceLabel = new QLabel("Resource Configuration", screen);
    resourceLabel->setStyleSheet("font-weight: bold; color: #555; margin-top: 10px;");
    formLayout->addRow(resourceLabel);
    
    // CPU Cores
    spinCPUCores = new QSpinBox(screen);
    spinCPUCores->setRange(1, 32);
    spinCPUCores->setValue(8);
    spinCPUCores->setMinimumHeight(35);
    spinCPUCores->setStyleSheet("font-size: 13px; padding: 5px;");
    spinCPUCores->setSuffix(" cores");
    formLayout->addRow("🔷 CPU Cores:", spinCPUCores);
    
    // RAM
    spinRAMSize = new QSpinBox(screen);
    spinRAMSize->setRange(4, 128);
    spinRAMSize->setValue(32);
    spinRAMSize->setMinimumHeight(35);
    spinRAMSize->setStyleSheet("font-size: 13px; padding: 5px;");
    spinRAMSize->setSuffix(" GB");
    formLayout->addRow("🔶 RAM Size:", spinRAMSize);
    
    // Disk Operations
    spinDiskOps = new QSpinBox(screen);
    spinDiskOps->setRange(10, 500);
    spinDiskOps->setValue(100);
    spinDiskOps->setMinimumHeight(35);
    spinDiskOps->setStyleSheet("font-size: 13px; padding: 5px;");
    spinDiskOps->setSuffix(" ops");
    formLayout->addRow("💾 Disk I/O Capacity:", spinDiskOps);
    
    // Network Speed
    spinNetworkSpeed = new QSpinBox(screen);
    spinNetworkSpeed->setRange(100, 10000);
    spinNetworkSpeed->setValue(1000);
    spinNetworkSpeed->setSingleStep(100);
    spinNetworkSpeed->setMinimumHeight(35);
    spinNetworkSpeed->setStyleSheet("font-size: 13px; padding: 5px;");
    spinNetworkSpeed->setSuffix(" Mbps");
    formLayout->addRow("🌐 Network Speed:", spinNetworkSpeed);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);
    
    btnBackToWelcome = new QPushButton("← Back", screen);
    btnBackToWelcome->setStyleSheet(
        "QPushButton { "
        "  background-color: #757575; "
        "  color: white; "
        "  font-size: 14px; "
        "  padding: 12px 30px; "
        "  border-radius: 6px; "
        "  border: none; "
        "} "
        "QPushButton:hover { background-color: #616161; }"
    );
    btnBackToWelcome->setCursor(Qt::PointingHandCursor);
    connect(btnBackToWelcome, &QPushButton::clicked, this, &MainWindow::showWelcomeScreen);
    
    btnStartSimulation = new QPushButton("▶ Start Simulation", screen);
    btnStartSimulation->setStyleSheet(
        "QPushButton { "
        "  background-color: #4CAF50; "
        "  color: white; "
        "  font-size: 14px; "
        "  font-weight: bold; "
        "  padding: 12px 40px; "
        "  border-radius: 6px; "
        "  border: none; "
        "} "
        "QPushButton:hover { background-color: #45a049; }"
    );
    btnStartSimulation->setCursor(Qt::PointingHandCursor);
    connect(btnStartSimulation, &QPushButton::clicked, this, &MainWindow::onStartSimulation);
    
    buttonLayout->addWidget(btnBackToWelcome);
    buttonLayout->addStretch();
    buttonLayout->addWidget(btnStartSimulation);
    
    mainLayout->addWidget(header);
    mainLayout->addWidget(subtitle);
    mainLayout->addWidget(configGroup);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch();
    
    return screen;
}

// ==================== SIMULATION SCREEN ====================
QWidget* MainWindow::createSimulationScreen()
{
    QWidget *screen = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(screen);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    
    // Control Panel
    QGroupBox *controlGroup = new QGroupBox("⚙️ Simulation Controls", screen);
    QHBoxLayout *controlLayout = new QHBoxLayout(controlGroup);
    
    btnPause = new QPushButton("⏸ Pause", screen);
    btnPause->setStyleSheet("padding: 8px 20px; font-size: 13px;");
    btnPause->setMinimumWidth(100);
    
    btnStop = new QPushButton("⏹ Stop", screen);
    btnStop->setStyleSheet(
        "QPushButton { background-color: #f44336; color: white; font-weight: bold; padding: 8px 20px; font-size: 13px; }"
    );
    btnStop->setMinimumWidth(100);
    
    controlLayout->addWidget(btnPause);
    controlLayout->addWidget(btnStop);
    controlLayout->addStretch();
    
    connect(btnPause, &QPushButton::clicked, this, &MainWindow::onPauseSimulation);
    connect(btnStop, &QPushButton::clicked, this, &MainWindow::onStopSimulation);
    
    mainLayout->addWidget(controlGroup);
    
    // Middle Section: Resources + Metrics
    QHBoxLayout *middleLayout = new QHBoxLayout();
    
    // Resource Usage
    QGroupBox *resourceGroup = new QGroupBox("📊 Resource Usage", screen);
    QVBoxLayout *resourceLayout = new QVBoxLayout(resourceGroup);
    resourceLayout->setSpacing(8);
    
    QLabel *lblCPU = new QLabel("🔷 CPU Cores:", screen);
    cpuUsageBar = new QProgressBar(screen);
    cpuUsageBar->setMaximum(8);
    cpuUsageBar->setValue(0);
    cpuUsageBar->setTextVisible(true);
    cpuUsageBar->setFormat("%v / %m cores (%p%)");
    cpuUsageBar->setStyleSheet("QProgressBar::chunk { background-color: #2196F3; }");
    cpuUsageBar->setMinimumHeight(25);
    
    QLabel *lblRAM = new QLabel("🔶 RAM:", screen);
    ramUsageBar = new QProgressBar(screen);
    ramUsageBar->setMaximum(32);
    ramUsageBar->setValue(0);
    ramUsageBar->setTextVisible(true);
    ramUsageBar->setFormat("%v / %m GB (%p%)");
    ramUsageBar->setStyleSheet("QProgressBar::chunk { background-color: #FF9800; }");
    ramUsageBar->setMinimumHeight(25);
    
    QLabel *lblDisk = new QLabel("💾 Disk I/O:", screen);
    diskUsageBar = new QProgressBar(screen);
    diskUsageBar->setMaximum(100);
    diskUsageBar->setValue(0);
    diskUsageBar->setTextVisible(true);
    diskUsageBar->setFormat("%v / %m ops (%p%)");
    diskUsageBar->setStyleSheet("QProgressBar::chunk { background-color: #9C27B0; }");
    diskUsageBar->setMinimumHeight(25);
    
    QLabel *lblNetwork = new QLabel("🌐 Network:", screen);
    networkUsageBar = new QProgressBar(screen);
    networkUsageBar->setMaximum(1000);
    networkUsageBar->setValue(0);
    networkUsageBar->setTextVisible(true);
    networkUsageBar->setFormat("%v / %m Mbps (%p%)");
    networkUsageBar->setStyleSheet("QProgressBar::chunk { background-color: #4CAF50; }");
    networkUsageBar->setMinimumHeight(25);
    
    resourceLayout->addWidget(lblCPU);
    resourceLayout->addWidget(cpuUsageBar);
    resourceLayout->addWidget(lblRAM);
    resourceLayout->addWidget(ramUsageBar);
    resourceLayout->addWidget(lblDisk);
    resourceLayout->addWidget(diskUsageBar);
    resourceLayout->addWidget(lblNetwork);
    resourceLayout->addWidget(networkUsageBar);
    resourceLayout->addStretch();
    
    // Performance Metrics
    QGroupBox *metricsGroup = new QGroupBox("📈 Performance Metrics", screen);
    QVBoxLayout *metricsLayout = new QVBoxLayout(metricsGroup);
    metricsLayout->setSpacing(15);
    
    QFont metricsFont;
    metricsFont.setPointSize(11);
    
    lblTotalJobs = new QLabel("📋 Total Jobs: 0", screen);
    lblTotalJobs->setFont(metricsFont);
    
    lblActiveJobs = new QLabel("🔄 Active Jobs: 0", screen);
    lblActiveJobs->setFont(metricsFont);
    lblActiveJobs->setStyleSheet("color: #2196F3;");
    
    lblCompletedJobs = new QLabel("✅ Completed: 0", screen);
    lblCompletedJobs->setFont(metricsFont);
    lblCompletedJobs->setStyleSheet("color: #4CAF50;");
    
    lblAvgWaitTime = new QLabel("⏱️ Avg Wait Time: 0.00s", screen);
    lblAvgWaitTime->setFont(metricsFont);
    
    lblThroughput = new QLabel("⚡ Throughput: 0.00 jobs/s", screen);
    lblThroughput->setFont(metricsFont);
    
    lblDeadlocksPrevented = new QLabel("🛡️ Deadlocks Prevented: 0", screen);
    lblDeadlocksPrevented->setFont(metricsFont);
    lblDeadlocksPrevented->setStyleSheet("color: #FF5722;");
    
    metricsLayout->addWidget(lblTotalJobs);
    metricsLayout->addWidget(lblActiveJobs);
    metricsLayout->addWidget(lblCompletedJobs);
    metricsLayout->addSpacing(10);
    metricsLayout->addWidget(lblAvgWaitTime);
    metricsLayout->addWidget(lblThroughput);
    metricsLayout->addSpacing(10);
    metricsLayout->addWidget(lblDeadlocksPrevented);
    metricsLayout->addStretch();
    
    middleLayout->addWidget(resourceGroup, 1);
    middleLayout->addWidget(metricsGroup, 1);
    mainLayout->addLayout(middleLayout);
    
    // Job Table + Log
    QSplitter *bottomSplitter = new QSplitter(Qt::Vertical, screen);
    
    QGroupBox *jobGroup = new QGroupBox("📊 Active Jobs", screen);
    QVBoxLayout *jobLayout = new QVBoxLayout(jobGroup);
    
    jobTable = new QTableWidget(screen);
    jobTable->setColumnCount(7);
    jobTable->setHorizontalHeaderLabels({
        "Job ID", "Priority", "Status", "CPU Time", "RAM (GB)", "Phase", "Wait Time"
    });
    jobTable->horizontalHeader()->setStretchLastSection(true);
    jobTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    jobTable->setAlternatingRowColors(true);
    jobTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    jobTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    jobTable->setMinimumHeight(200);
    jobLayout->addWidget(jobTable);
    
    QGroupBox *logGroup = new QGroupBox("📜 System Log", screen);
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    
    logView = new QTextEdit(screen);
    logView->setReadOnly(true);
    logView->setMinimumHeight(150);
    logView->setMaximumHeight(200);
    QFont logFont("Consolas", 9);
    logView->setFont(logFont);
    logLayout->addWidget(logView);
    
    bottomSplitter->addWidget(jobGroup);
    bottomSplitter->addWidget(logGroup);
    bottomSplitter->setStretchFactor(0, 3);
    bottomSplitter->setStretchFactor(1, 1);
    
    mainLayout->addWidget(bottomSplitter, 1);
    
    return screen;
}

// ==================== RESULTS SCREEN ====================
QWidget* MainWindow::createResultsScreen()
{
    QWidget *screen = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(screen);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(30);
    layout->setContentsMargins(50, 50, 50, 50);
    
    QLabel *header = new QLabel("✅ Simulation Complete!", screen);
    header->setStyleSheet("font-size: 32px; font-weight: bold; color: #4CAF50;");
    header->setAlignment(Qt::AlignCenter);
    
    lblFinalStats = new QLabel("", screen);
    lblFinalStats->setStyleSheet(
        "font-size: 14px; "
        "padding: 30px; "
        "border: 2px solid #ddd; "
        "border-radius: 10px; "
        "background: #f9f9f9;"
    );
    lblFinalStats->setAlignment(Qt::AlignLeft);
    lblFinalStats->setMinimumWidth(500);
    
    QHBoxLayout *btnLayout = new QHBoxLayout();
    
    btnExport = new QPushButton("📄 Export Results", screen);
    btnExport->setStyleSheet(
        "QPushButton { background-color: #2196F3; color: white; padding: 12px 30px; font-size: 14px; border-radius: 6px; }"
    );
    btnExport->setCursor(Qt::PointingHandCursor);
    connect(btnExport, &QPushButton::clicked, this, [this]() {
        QMessageBox::information(this, "Export", "Export feature coming in Phase 2!");
    });
    
    btnRunAgain = new QPushButton("↻ Run New Simulation", screen);
    btnRunAgain->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; padding: 12px 30px; font-size: 14px; font-weight: bold; border-radius: 6px; }"
    );
    btnRunAgain->setCursor(Qt::PointingHandCursor);
    connect(btnRunAgain, &QPushButton::clicked, this, &MainWindow::onResetSimulation);
    
    btnLayout->addWidget(btnExport);
    btnLayout->addWidget(btnRunAgain);
    
    layout->addWidget(header);
    layout->addWidget(lblFinalStats);
    layout->addSpacing(20);
    layout->addLayout(btnLayout);
    layout->addStretch();
    
    return screen;
}

// ==================== NAVIGATION ====================
void MainWindow::showWelcomeScreen()
{
    stackedWidget->setCurrentWidget(welcomeScreen);
    statusBar()->showMessage("Welcome to Data Center Simulator");
}

void MainWindow::showConfigScreen()
{
    stackedWidget->setCurrentWidget(configScreen);
    statusBar()->showMessage("Configure simulation parameters");
}

void MainWindow::showSimulationScreen()
{
    stackedWidget->setCurrentWidget(simulationScreen);
    statusBar()->showMessage("🔄 Simulation running...");
}

void MainWindow::showResultsScreen()
{
    stackedWidget->setCurrentWidget(resultsScreen);
    statusBar()->showMessage("Simulation completed successfully");
}

// ==================== SIMULATION CONTROL ====================
void MainWindow::onStartSimulation()
{
    // Get config values
    int numJobs = spinJobCount->value();
    configCPUCores = spinCPUCores->value();
    configRAMSize = spinRAMSize->value();
    configDiskOps = spinDiskOps->value();
    configNetworkSpeed = spinNetworkSpeed->value();
    
    // Update progress bar maximums
    cpuUsageBar->setMaximum(configCPUCores);
    ramUsageBar->setMaximum(configRAMSize);
    diskUsageBar->setMaximum(configDiskOps);
    networkUsageBar->setMaximum(configNetworkSpeed);
    
    initializeSimulation(numJobs);
    
    isRunning = true;
    isPaused = false;
    
    btnPause->setEnabled(true);
    btnStop->setEnabled(true);
    
    // Initialize log
    logView->clear();
    logView->append("=== Data Center Resource Management System ===");
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    logView->append(QString("[%1] Simulation started with %2 jobs").arg(timestamp).arg(numJobs));
    logView->append(QString("[%1] Resources: %2 CPU cores, %3 GB RAM, %4 Disk ops, %5 Mbps Network\n")
        .arg(timestamp).arg(configCPUCores).arg(configRAMSize).arg(configDiskOps).arg(configNetworkSpeed));
    
    updateTimer->start(100);
    showSimulationScreen();
}

void MainWindow::onStopSimulation()
{
    if (isRunning) {
        updateTimer->stop();
        
        QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
        logView->append(QString("[%1] Simulation stopped").arg(timestamp));
        
        // Prepare results
        double avgWaitTime = completedJobs > 0 ? (simulationCounter / 10.0) / completedJobs : 0.0;
        double throughput = completedJobs > 0 ? completedJobs / (simulationCounter / 10.0) : 0.0;
        int deadlocksPrevented = simulationCounter / 50;
        
        lblFinalStats->setText(QString(
            "<h3>Final Statistics</h3>"
            "<br>"
            "<b>📋 Total Jobs:</b> %1<br>"
            "<b>✅ Completed Jobs:</b> %2<br>"
            "<b>⏱️ Average Wait Time:</b> %3s<br>"
            "<b>⚡ Throughput:</b> %4 jobs/s<br>"
            "<b>🛡️ Deadlocks Prevented:</b> %5<br>"
            "<br>"
            "<b>💻 CPU Cores Used:</b> %6<br>"
            "<b>💾 RAM Allocated:</b> %7 GB<br>"
            "<b>📊 Disk Operations:</b> %8<br>"
            "<b>🌐 Network Speed:</b> %9 Mbps"
        )
        .arg(totalJobs)
        .arg(completedJobs)
        .arg(avgWaitTime, 0, 'f', 2)
        .arg(throughput, 0, 'f', 2)
        .arg(deadlocksPrevented)
        .arg(configCPUCores)
        .arg(configRAMSize)
        .arg(configDiskOps)
        .arg(configNetworkSpeed));
        
        cleanupSimulation();
        
        isRunning = false;
        isPaused = false;
        
        btnPause->setEnabled(false);
        btnPause->setText("⏸ Pause");
        btnStop->setEnabled(false);
        
        showResultsScreen();
    }
}

void MainWindow::onPauseSimulation()
{
    if (isRunning && !isPaused) {
        updateTimer->stop();
        isPaused = true;
        btnPause->setText("▶ Resume");
        statusBar()->showMessage("⏸ Simulation paused");
        
        QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
        logView->append(QString("[%1] Simulation paused").arg(timestamp));
    } 
    else if (isPaused) {
        updateTimer->start(100);
        isPaused = false;
        btnPause->setText("⏸ Pause");
        statusBar()->showMessage("🔄 Simulation running...");
        
        QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
        logView->append(QString("[%1] Simulation resumed").arg(timestamp));
    }
}

void MainWindow::onResetSimulation()
{
    cpuUsageBar->setValue(0);
    ramUsageBar->setValue(0);
    diskUsageBar->setValue(0);
    networkUsageBar->setValue(0);
    
    lblTotalJobs->setText("📋 Total Jobs: 0");
    lblActiveJobs->setText("🔄 Active Jobs: 0");
    lblCompletedJobs->setText("✅ Completed: 0");
    lblAvgWaitTime->setText("⏱️ Avg Wait Time: 0.00s");
    lblThroughput->setText("⚡ Throughput: 0.00 jobs/s");
    lblDeadlocksPrevented->setText("🛡️ Deadlocks Prevented: 0");
    
    jobTable->setRowCount(0);
    
    totalJobs = 0;
    completedJobs = 0;
    activeJobs = 0;
    simulationCounter = 0;
    mockJobIds.clear();
    
    showConfigScreen();
}

