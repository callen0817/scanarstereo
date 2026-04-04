#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <sensor_msgs/msg/nav_sat_fix.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <tf2_ros/transform_broadcaster.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

#include "InertialSense.h"

using namespace std::chrono_literals;

class Uins3Driver : public rclcpp::Node {
public:
    Uins3Driver() : Node("uins3_driver"), is_running_(true), time_offset_(0.0), offset_initialized_(false) {
        port_ = this->declare_parameter<std::string>("port", "/dev/ttyACM0");
        baudrate_ = this->declare_parameter<int>("baudrate", 921600);
        frame_id_imu_ = this->declare_parameter<std::string>("frame_id_imu", "imu_link");
        frame_id_odom_ = this->declare_parameter<std::string>("frame_id_odom", "odom");
        frame_id_base_ = this->declare_parameter<std::string>("frame_id_base", "base_link");

        auto qos = rclcpp::SensorDataQoS();
        imu_pub_ = this->create_publisher<sensor_msgs::msg::Imu>("/imu/data", qos);
        gps_pub_ = this->create_publisher<sensor_msgs::msg::NavSatFix>("/gps/fix", qos);
        odom_pub_ = this->create_publisher<nav_msgs::msg::Odometry>("/odom", qos);

        tf_broadcaster_ = std::make_unique<tf2_ros::TransformBroadcaster>(*this);

        initDevice();
        worker_thread_ = std::thread(&Uins3Driver::workerLoop, this);
    }

    ~Uins3Driver() {
        is_running_ = false;
        if (worker_thread_.joinable()) worker_thread_.join();
        is_.Close();
    }

private:
    InertialSense is_;
    std::thread worker_thread_;
    std::atomic<bool> is_running_;
    
    std::string port_, frame_id_imu_, frame_id_odom_, frame_id_base_;
    int baudrate_;
    double time_offset_;
    bool offset_initialized_;

    rclcpp::Publisher<sensor_msgs::msg::Imu>::SharedPtr imu_pub_;
    rclcpp::Publisher<sensor_msgs::msg::NavSatFix>::SharedPtr gps_pub_;
    rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr odom_pub_;
    std::unique_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;

    void initDevice() {
        RCLCPP_INFO(this->get_logger(), "Connecting to uINS-3 at %s", port_.c_str());
        
        if (!is_.Open(port_.c_str(), baudrate_)) {
            RCLCPP_FATAL(this->get_logger(), "Failed to open uINS-3 serial port!");
            throw std::runtime_error("Serial Port Error");
        }

        // Setup the SDK 1.11.1 data callback utilizing std::function capturing
        auto callback = [this](InertialSense* i, p_data_t* data, int pHandle) {
            RCLCPP_INFO(this->get_logger(), "Received data ID: %d", data->hdr.id);
            switch (data->hdr.id) {
                case DID_IMU:
                    this->publishImu(reinterpret_cast<imu_t*>(data->buf));
                    break;
                case DID_INS_2:
                    this->publishOdom(reinterpret_cast<ins_2_t*>(data->buf));
                    break;
                case DID_GPS1_POS:
                    this->publishGps(reinterpret_cast<gps_pos_t*>(data->buf));
                    break;
            }
        };

        is_.BroadcastBinaryData(DID_IMU, 2, callback);
        is_.BroadcastBinaryData(DID_INS_2, 10, callback);
        is_.BroadcastBinaryData(DID_GPS1_POS, 200, callback);

        RCLCPP_INFO(this->get_logger(), "uINS-3 Initialized. Streaming enabled.");
    }

    void workerLoop() {
        while (is_running_ && rclcpp::ok()) {
            is_.Update(); 
            std::this_thread::sleep_for(1ms);
        }
    }

    rclcpp::Time getSyncedTime(double gps_tow) {
        if (!offset_initialized_) {
            double current_ros_time = this->now().seconds();
            time_offset_ = current_ros_time - gps_tow;
            offset_initialized_ = true;
        }
        return rclcpp::Time((gps_tow + time_offset_) * 1e9);
    }

    void publishImu(imu_t* imu) {
        sensor_msgs::msg::Imu msg;
        msg.header.stamp = getSyncedTime(imu->time);
        msg.header.frame_id = frame_id_imu_;

        msg.linear_acceleration.x = imu->I.acc[1];
        msg.linear_acceleration.y = imu->I.acc[0];
        msg.linear_acceleration.z = -imu->I.acc[2];

        msg.angular_velocity.x = imu->I.pqr[1];
        msg.angular_velocity.y = imu->I.pqr[0];
        msg.angular_velocity.z = -imu->I.pqr[2];

        imu_pub_->publish(msg);
    }

    void publishOdom(ins_2_t* ins) {
        nav_msgs::msg::Odometry msg;
        msg.header.stamp = getSyncedTime(ins->timeOfWeek);
        msg.header.frame_id = frame_id_odom_;
        msg.child_frame_id = frame_id_base_;

        tf2::Quaternion q_ned(ins->qn2b[1], ins->qn2b[2], ins->qn2b[3], ins->qn2b[0]); 
        tf2::Quaternion q_rot;
        q_rot.setRPY(M_PI, 0, M_PI/2);
        tf2::Quaternion q_enu = q_rot * q_ned;
        
        msg.pose.pose.orientation = tf2::toMsg(q_enu);

        msg.pose.pose.position.x = ins->uvw[1];
        msg.pose.pose.position.y = ins->uvw[0];
        msg.pose.pose.position.z = -ins->uvw[2];

        odom_pub_->publish(msg);

        geometry_msgs::msg::TransformStamped t;
        t.header.stamp = msg.header.stamp;
        t.header.frame_id = frame_id_base_;
        t.child_frame_id = frame_id_imu_;
        t.transform.rotation.w = 1.0; 
        tf_broadcaster_->sendTransform(t);
    }

    void publishGps(gps_pos_t* gps) {
        sensor_msgs::msg::NavSatFix msg;
        msg.header.stamp = getSyncedTime(gps->timeOfWeekMs / 1000.0);
        msg.header.frame_id = "gps_link";

        msg.latitude = gps->lla[0];
        msg.longitude = gps->lla[1];
        msg.altitude = gps->lla[2];
        
        msg.position_covariance[0] = gps->hAcc * gps->hAcc;
        msg.position_covariance[4] = gps->hAcc * gps->hAcc;
        msg.position_covariance[8] = gps->vAcc * gps->vAcc;
        
        if (gps->status & GPS_STATUS_FIX_RTK_FIX) msg.status.status = 2;
        else if (gps->status & GPS_STATUS_FIX_RTK_FLOAT) msg.status.status = 1;
        else msg.status.status = 0;

        gps_pub_->publish(msg);
    }
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::executors::MultiThreadedExecutor executor;
    auto node = std::make_shared<Uins3Driver>();
    executor.add_node(node);
    executor.spin();
    rclcpp::shutdown();
    return 0;
}
