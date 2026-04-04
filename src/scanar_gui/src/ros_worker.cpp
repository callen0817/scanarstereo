#include "scanar_gui/ros_worker.h"

RosWorker::RosWorker(QObject *parent) : QObject(parent) {
    node_ = rclcpp::Node::make_shared("scanar_gui_worker");

    auto qos = rclcpp::SensorDataQoS();

    img_sub_ = node_->create_subscription<sensor_msgs::msg::Image>(
        "/oak/rgb/image_raw", qos, std::bind(&RosWorker::imageCallback, this, std::placeholders::_1));

    odom_sub_ = node_->create_subscription<nav_msgs::msg::Odometry>(
        "/odom", qos, std::bind(&RosWorker::odomCallback, this, std::placeholders::_1));

    lidar_sub_ = node_->create_subscription<sensor_msgs::msg::PointCloud2>(
        "/rslidar_points", qos, std::bind(&RosWorker::lidarCallback, this, std::placeholders::_1));
}

void RosWorker::start() {
    spin_thread_ = std::thread([this]() {
        rclcpp::spin(node_);
    });
}

void RosWorker::stop() {
    rclcpp::shutdown();
    if (spin_thread_.joinable()) spin_thread_.join();
}

void RosWorker::imageCallback(const sensor_msgs::msg::Image::SharedPtr msg) {
    try {
        cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::RGB8);
        QImage qimg(cv_ptr->image.data, cv_ptr->image.cols, cv_ptr->image.rows, cv_ptr->image.step, QImage::Format_RGB888);
        emit imageReceived(qimg.copy());
    } catch (cv_bridge::Exception& e) {
        emit statusUpdated(QString("cv_bridge error: %1").arg(e.what()));
    }
}

void RosWorker::odomCallback(const nav_msgs::msg::Odometry::SharedPtr msg) {
    emit odomReceived(msg->pose.pose.position.x, msg->pose.pose.position.y, msg->pose.pose.position.z);
}

void RosWorker::lidarCallback(const sensor_msgs::msg::PointCloud2::SharedPtr msg) {
    int points = msg->width * msg->height;
    emit lidarReceived(points);
}
