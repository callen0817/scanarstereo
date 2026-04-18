#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/imu.hpp>

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    auto node = rclcpp::Node::make_shared("uins3_driver");
    auto imu_pub = node->create_publisher<sensor_msgs::msg::Imu>("/dev1/imu/data", 100);
    
    auto timer = node->create_wall_timer(std::chrono::milliseconds(2), [&node, &imu_pub]() {
        sensor_msgs::msg::Imu msg;
        msg.header.stamp = node->now();
        msg.header.frame_id = "imu_link";
        msg.angular_velocity.x = 1.0;
        imu_pub->publish(msg);
    });

    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
