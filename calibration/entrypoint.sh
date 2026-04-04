#!/bin/bash
source /opt/ros/noetic/setup.bash
if [ -f "/kalibr_ws/devel/setup.bash" ]; then
    source /kalibr_ws/devel/setup.bash
fi
exec "$@"
