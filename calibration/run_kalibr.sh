#!/bin/bash
echo "=== Running Joint Camera-IMU Extrinsic Calibration ==="
echo "Note: Skipping the separate intrinsics pass to avoid the N=1 single-camera bug."

sudo docker run -it -w /data --rm -v $(pwd):/data stereolabs/kalibr:kinetic \
    kalibr_calibrate_imu_camera --bag /data/kalibr_fixed.bag \
    --cam /data/camchain_guess.yaml \
    --imu /data/imu_uins3.yaml \
    --target /data/aprilgrid_10x8.yaml

echo "=== Done! The resulting T_cam_imu matrix is inside results-imucam-kalibr_fixed.txt ==="
