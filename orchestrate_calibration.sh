#!/bin/bash
LOGFILE="/home/scanarst/scanarv2/calibration_orchestrator.log"
> "$LOGFILE"
exec > >(tee -a "$LOGFILE") 2>&1

echo "======================================"
echo " Calibration Orchestrator Started "
echo "======================================"

echo "[1/2] Copying rosbag to kalibr_ros1.bag for x86 processing..."
cp /home/scanarst/scanarv2/kalibr_ros1.bag /home/scanarst/scanarv2/calibration/device\ 1/kalibr/kalibr_ros1.bag

echo "[2/2] Packing Device 1 Calibration Data..."
cd /home/scanarst/scanarv2/calibration/device\ 1
tar -czvf dev1_calibration_data.tar.gz kalibr/ calib/

echo "======================================"
echo " Kalibr preprocessing on ARM64 is not supported natively without complex Docker build logic."
echo " Please transfer the 'dev1_calibration_data.tar.gz' file to an x86 machine and run Kalibr there."
echo "======================================"
