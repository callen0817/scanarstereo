# ScanAR Stereo v2 - Project Status & Progress Log

## 🎯 Project Scope
**Goal:** Rebuild ScanAR Stereo v2 from scratch on a Jetson Orin NX (JetPack 6.0 / Ubuntu 22.04). 
**Architecture:** 100% Native ROS2 Humble (except for offline calibration tools).
**Core Components:**
- **Inertial Sense uINS-3:** High-rate (400Hz) RTK/IMU acting as the master hardware clock.
- **RoboSense Airy:** 3D LiDAR (XYZIRT point cloud format).
- **OAK-D Pro:** RGB Depth Camera.
- **FAST-LIVO2:** LiDAR-Inertial-Visual Odometry SLAM engine (with pose graph and submap support).
- **ScanAR GUI:** Custom Qt5 C++ Desktop Control Center (Camera feed, LiDAR stats, Odometry, Start/Stop).
- **Calibration:** Isolated ROS1 Noetic Docker container (Kalibr / FAST-Calib).

---

## 📈 Milestones & Progress

### 🟩 Phase 1: Core System & uINS-3 Driver (COMPLETED)
- [x] Install system dependencies (Qt5, OpenCV, PDAL, Open3D).
- [x] Configure static IPs for `eth0` (RoboSense) and `eth1` (OAK-D).
- [x] Port Inertial Sense SDK to ROS2 C++ (Locked to stable firmware v1.11.1).
- [x] Implement dedicated background polling thread for 400Hz IMU streaming.
- [x] Implement GPS Time-of-Week to ROS Time offset correction (Master Clock sync).
- [x] Implement on-the-fly NED $\rightarrow$ ENU coordinate frame conversion.
- [x] Disable conflicting Linux services (`ModemManager`, `brltty`) to stabilize serial USB connections.

### 🟩 Phase 2: Vision & Point Cloud Drivers (COMPLETED)
- [x] Install `depthai-ros` (OAK-D Pro) from official ROS2 Humble binaries.
- [x] Clone and build RoboSense `rslidar_sdk` from source.
- [x] Patch RoboSense CMake configurations for Colcon/ROS2 compatibility.
- [x] Configure RoboSense driver to output SLAM-compatible `XYZIRT` point clouds.
- [x] Force RoboSense to use the system clock to align with the uINS-3 master clock.

### 🟩 Phase 3: FAST-LIVO2 SLAM Engine (COMPLETED)
- [x] Build core math libraries from source (`Sophus 1.22.10`, `GTSAM 4.2a9`).
- [x] Build visual frontend dependencies (`vikit_common`, `vikit_ros` adapted for ROS2).
- [x] Build `livox_ros_driver2` to satisfy FAST-LIVO2 internal dependencies.
- [x] Clone and build the full `FAST-LIVO2` ROS2 Humble suite (`fastlio2`, `pgo`, `hba`).
- [x] Create a custom configuration file (`scanar_v2_rs.yaml`) tuned for VELO16 (RoboSense), Best Effort QoS, and specific `/oak`, `/imu`, and `/rslidar` topics.

### 🟩 Phase 4: Qt5 Desktop Application (COMPLETED)
- [x] Create `scanar_gui` ROS2 C++ package.
- [x] Implement `RosWorker` thread using `cv_bridge` to ingest camera feeds, Odometry, and PointCloud2 statistics without blocking the UI.
- [x] Build the Qt MainWindow with a responsive layout, Odometry readouts, LiDAR packet counters, and Start/Stop buttons.
- [x] Create the master `scanar_v2.launch.py` file to orchestrate all sensors, SLAM, and the GUI simultaneously.

### 🟩 Phase 5: Backup & Calibration Pre-flight (COMPLETED)
- [x] Create the ROS1 Noetic Dockerfile and Entrypoint for isolated `Kalibr` execution.
- [x] Initialize Git repository with proper `.gitignore`.
- [x] Push all 7,200+ lines of custom ROS2 workspace code to GitHub (`callen0817/scanarstereo`).
- [x] Install `xRDP` for stable Remote Desktop access to the Jetson Orin NX (bypassing NoMachine download blocks).

---

## 🚧 Current Status & Next Steps

**We are currently paused at Phase 6: Sensor Calibration.**
The system architecture, drivers, and SLAM engine are fully built and backed up. The GUI successfully launches and connects to the ROS2 network.

### 🔜 To-Do Upon Resuming:
1. **Record Calibration Bags:** We need to capture ROS2 bags of the physical sensors moving around the AprilGrid targets.
2. **Convert Bags:** Convert the ROS2 (`.db3`) bags back to ROS1 (`.bag`) format so they can be digested by the Kalibr Docker container.
3. **Execute Kalibr / FAST-Calib:** Run the camera-IMU and LiDAR-IMU calibrations using the specific AprilGrid dimensions you will provide.
4. **Update Extrinsics:** Inject the resulting highly accurate translation/rotation matrices (`extrinsic_T`, `extrinsic_R`, `Rcl`, `Pcl`) into `~/scanarv2/src/fast_livo/config/scanar_v2_rs.yaml`.
5. **Full System Test:** Launch the complete system and map a room.

---

## ⚠️ Issues Faced & Resolutions

1. **uINS-3 SDK API Drift & Compilation Failures:**
   * *Issue:* The modern `main` branch of the Inertial Sense SDK completely changed its C++ architecture, breaking our initial driver logic.
   * *Fix:* Hardcoded CMake `FetchContent` to specifically pull the `v1.11.1` release tag, aligning the C++ structs with the physical firmware on your device.

2. **uINS-3 USB Brownout / Serial Hijacking:**
   * *Issue:* The uINS-3 device was constantly dropping from `/dev/ttyACM0` every 2 seconds, and the ROS node threw a `Serial Port Error`.
   * *Fix:* Diagnosed that Ubuntu's `ModemManager` and `brltty` were aggressively probing the port. Permanently stopped, disabled, and removed these services, stabilizing the hardware connection instantly.

3. **RoboSense Driver ROS2 Compatibility:**
   * *Issue:* `rslidar_sdk` failed to build due to missing `rslidar_msg` and hardcoded ROS1 flags.
   * *Fix:* Cloned `rslidar_msg`, patched `CMakeLists.txt` to `COMPILE_METHOD: COLCON`, and updated `config.yaml` to explicitly target `ROS2` and `XYZIRT` point clouds.

4. **FAST-LIVO2 Missing Dependencies:**
   * *Issue:* The SLAM engine failed to build because `libgtsam-dev` and `Sophus` are not available in JetPack 6 standard APT repositories.
   * *Fix:* Manually downloaded, compiled, and installed `GTSAM 4.2a9` and `Sophus 1.22.10` from source.

5. **Qt5 GUI Linking Errors & Black Screens:**
   * *Issue:* The C++ Qt GUI suffered from `vtable` linking errors and `cv_bridge` conflicts, and initially showed a black screen.
   * *Fix:* Added specific `AUTOMOC` header declarations to `CMakeLists.txt` to fix the Qt meta-object compiler. Created a unified `scanar_v2.launch.py` so the GUI doesn't just open a black window, but actually triggers the underlying hardware drivers to start publishing data.

6. **NoMachine Remote Desktop Blocking:**
   * *Issue:* NoMachine blocked automated `wget`/`curl` downloads of their ARM64 `.deb` package.
   * *Fix:* Quickly pivoted to installing and enabling `xRDP`, providing immediate, stable remote desktop access to the Jetson via standard Windows/Mac RDP clients.

---
*Log generated on April 4, 2026. Workspace is clean, built, and pushed to GitHub.*