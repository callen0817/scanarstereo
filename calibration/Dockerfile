FROM arm64v8/ros:noetic-robot

# Install dependencies for Kalibr
RUN apt-get update && apt-get install -y \
    python3-pip \
    python3-catkin-tools \
    ros-noetic-vision-opencv \
    ros-noetic-image-transport \
    libopencv-dev \
    git \
    wget \
    python3-tk \
    libv4l-dev \
    libsuitesparse-dev

# Setup workspace
RUN mkdir -p /kalibr_ws/src
WORKDIR /kalibr_ws/src

# Clone Kalibr
RUN git clone https://github.com/ethz-asl/kalibr.git

# Build Kalibr (Note: Kalibr build can be complex, this is a simplified version)
WORKDIR /kalibr_ws
RUN /bin/bash -c "source /opt/ros/noetic/setup.bash && catkin build -DCMAKE_BUILD_TYPE=Release"

# Entrypoint
COPY ./entrypoint.sh /
RUN chmod +x /entrypoint.sh
ENTRYPOINT ["/entrypoint.sh"]
CMD ["bash"]
