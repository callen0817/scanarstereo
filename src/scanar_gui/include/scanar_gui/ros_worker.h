#ifndef ROS_WORKER_H
#define ROS_WORKER_H

#include <QObject>
#include <QImage>
#include <QString>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
#include <thread>

class RosWorker : public QObject {
    Q_OBJECT
public:
    explicit RosWorker(QObject *parent = nullptr);
    void start();
    void stop();

signals:
    void imageReceived(QImage img);
    void statusUpdated(QString status);
    void odomReceived(double x, double y, double z);
    void lidarReceived(int points);

private:
    void imageCallback(const sensor_msgs::msg::Image::SharedPtr msg);
    void odomCallback(const nav_msgs::msg::Odometry::SharedPtr msg);
    void lidarCallback(const sensor_msgs::msg::PointCloud2::SharedPtr msg);

    rclcpp::Node::SharedPtr node_;
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr img_sub_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub_;
    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr lidar_sub_;
    std::thread spin_thread_;
};

#endif
