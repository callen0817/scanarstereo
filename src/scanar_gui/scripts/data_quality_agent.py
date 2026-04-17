#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Imu, Image, PointCloud2
import time

class DataQualityAgent(Node):
    def __init__(self):
        super().__init__('data_quality_agent')
        self.imu_sub = self.create_subscription(Imu, '/dev1/imu/data', self.imu_cb, 100)
        self.lidar_sub = self.create_subscription(PointCloud2, '/dev1/rslidar_points', self.lidar_cb, 10)
        self.cam_sub = self.create_subscription(Image, '/dev1/oak/rgb/image_raw', self.cam_cb, 10)
        self.imu_count = 0
        self.cam_count = 0
        self.lidar_count = 0
        self.start_time = time.time()
        self.timer = self.create_timer(2.0, self.report_cb)

    def imu_cb(self, msg): self.imu_count += 1
    def lidar_cb(self, msg): self.lidar_count += 1
    def cam_cb(self, msg): self.cam_count += 1

    def report_cb(self):
        elapsed = time.time() - self.start_time
        if elapsed == 0: return
        ir, cr, lr = self.imu_count/elapsed, self.cam_count/elapsed, self.lidar_count/elapsed
        self.get_logger().info(f'IMU: {ir:.1f}Hz | CAM: {cr:.1f}Hz | LIDAR: {lr:.1f}Hz')
        if ir < 190.0: self.get_logger().error('IMU RATE FAILURE! (<200Hz)')
        if cr < 24.0: self.get_logger().error('CAMERA RATE FAILURE! (<25fps)')
        self.imu_count = self.cam_count = self.lidar_count = 0
        self.start_time = time.time()

if __name__ == '__main__':
    rclpy.init()
    rclpy.spin(DataQualityAgent())
