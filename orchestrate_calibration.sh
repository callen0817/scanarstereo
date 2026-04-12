#!/bin/bash
LOGFILE="/home/scanarst/scanarv2/calibration_orchestrator.log"
> "$LOGFILE"
exec > >(tee -a "$LOGFILE") 2>&1

echo "======================================"
echo " Calibration Orchestrator Started "
echo "======================================"

echo "[1/4] Building Kalibr Docker Image (This will take a while)..."
cd /home/scanarst/scanarv2/calibration

sg docker -c 'bash -c "docker build --network host -t kalibr ."'

if [ $? -ne 0 ]; then
    echo "ERROR: Docker build failed! Check calibration_orchestrator.log"
    exit 1
fi

echo "Docker build completed successfully!"

echo "[2/4] Running Kalibr: Camera Intrinsic Calibration..."
sg docker -c "docker run --rm --network host -v /home/scanarst/scanarv2:/data kalibr kalibr_calibrate_cameras --bag /data/kalibr_ros1.bag --topics /oak/rgb/image_raw --models pinhole-radtan --target /data/calibration/aprilgrid_10x8.yaml"

if [ ! -f "/home/scanarst/scanarv2/camchain-data_kalibr_ros1.yaml" ]; then
    echo "ERROR: Camera calibration failed."
    exit 1
fi
mv /home/scanarst/scanarv2/camchain-data_kalibr_ros1.yaml /home/scanarst/scanarv2/calibration/camchain.yaml

echo "[3/4] Running Kalibr: Camera-IMU Extrinsic Calibration..."
sg docker -c "docker run --rm --network host -v /home/scanarst/scanarv2:/data kalibr kalibr_calibrate_imu_camera --bag /data/kalibr_ros1.bag --cam /data/calibration/camchain.yaml --imu /data/calibration/imu_uins3.yaml --target /data/calibration/aprilgrid_10x8.yaml"

if [ ! -f "/home/scanarst/scanarv2/camchain-imucam-data_kalibr_ros1.yaml" ]; then
    echo "ERROR: IMU-Camera calibration failed to produce output."
    exit 1
fi

mv /home/scanarst/scanarv2/camchain-imucam-data_kalibr_ros1.yaml /home/scanarst/scanarv2/calibration/camchain-imucam.yaml
mv /home/scanarst/scanarv2/results-imucam-data_kalibr_ros1.txt /home/scanarst/scanarv2/calibration/results-imucam.txt

echo "[4/4] Kalibr Pipeline Finished!"
