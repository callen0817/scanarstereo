#include "scanar_gui/mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("ScanAR Stereo v2 Control Center");
    resize(800, 600);

    auto *central_widget = new QWidget(this);
    setCentralWidget(central_widget);
    auto *layout = new QVBoxLayout(central_widget);

    image_label_ = new QLabel("Camera Feed (Waiting for /oak/rgb/image_raw...)", this);
    image_label_->setAlignment(Qt::AlignCenter);
    // Allow the image label to shrink and expand properly so it doesn't push buttons offscreen
    image_label_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    image_label_->setStyleSheet("background-color: black; color: white;");
    layout->addWidget(image_label_, 1); // stretch factor 1

    auto *info_layout = new QHBoxLayout();
    odom_label_ = new QLabel("Pose: X: 0.00, Y: 0.00, Z: 0.00", this);
    lidar_label_ = new QLabel("LiDAR: Waiting for /rslidar_points...", this);
    info_layout->addWidget(odom_label_);
    info_layout->addWidget(lidar_label_);
    layout->addLayout(info_layout);

    status_label_ = new QLabel("System Ready. Run 'ros2 launch scanar_gui scanar_v2.launch.py' to start sensors.", this);
    layout->addWidget(status_label_);

    auto *button_layout = new QHBoxLayout();
    start_button_ = new QPushButton("START SLAM", this);
    stop_button_ = new QPushButton("STOP & SAVE", this);
    start_button_->setMinimumHeight(40);
    stop_button_->setMinimumHeight(40);
    button_layout->addWidget(start_button_);
    button_layout->addWidget(stop_button_);
    layout->addLayout(button_layout);

    ros_worker_ = new RosWorker(this);
    connect(ros_worker_, &RosWorker::imageReceived, this, &MainWindow::updateImage);
    connect(ros_worker_, &RosWorker::odomReceived, this, &MainWindow::updateOdom);
    connect(ros_worker_, &RosWorker::lidarReceived, this, &MainWindow::updateLidar);
    connect(start_button_, &QPushButton::clicked, this, &MainWindow::onStartClicked);
    connect(stop_button_, &QPushButton::clicked, this, &MainWindow::onStopClicked);

    ros_worker_->start();
}

MainWindow::~MainWindow() {
    ros_worker_->stop();
}

void MainWindow::updateImage(QImage img) {
    image_label_->setPixmap(QPixmap::fromImage(img).scaled(image_label_->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::updateOdom(double x, double y, double z) {
    odom_label_->setText(QString("Pose: X: %1, Y: %2, Z: %3").arg(x, 0, 'f', 2).arg(y, 0, 'f', 2).arg(z, 0, 'f', 2));
}

void MainWindow::updateLidar(int points) {
    lidar_label_->setText(QString("LiDAR: %1 points/scan").arg(points));
}

void MainWindow::onStartClicked() {
    status_label_->setText("SLAM Running...");
}

void MainWindow::onStopClicked() {
    status_label_->setText("Saving Project...");
}
