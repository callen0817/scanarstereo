from pathlib import Path
from rosbags.highlevel import AnyReader
from rosbags.rosbag1 import Writer
from rosbags.typesys import get_typestore, Stores
import os

def run_conversion():
    typestore = get_typestore(Stores.ROS2_HUMBLE)
    
    # We map the dev1 namespace back to standard topics expected by Kalibr
    remap = {
        '/dev1/oak/rgb/image_raw': '/oak/rgb/image_raw',
        '/dev1/imu/data': '/imu/data',
        '/dev1/rslidar_points': '/rslidar_points'
    }

    bag_dir = Path('/home/scanarst/scanarv2/calibration/device 1/dev1_calibration_bag')
    target_name = '/home/scanarst/scanarv2/kalibr_ros1.bag'

    if not bag_dir.exists():
        print(f"Bag directory {bag_dir} does not exist.")
        return

    if Path(target_name).exists():
        os.remove(target_name)
        
    print(f"Converting {bag_dir} to {target_name}...")
    with AnyReader([bag_dir], default_typestore=typestore) as reader, Writer(Path(target_name)) as writer:
        conn_map = {}
        for conn in reader.connections:
            topic = remap.get(conn.topic, conn.topic)
            conn_map[conn.id] = writer.add_connection(topic, conn.msgtype)
            
        count = 0
        for conn, timestamp, data in reader.messages():
            writer.write(conn_map[conn.id], timestamp, data)
            count += 1
            if count % 1000 == 0:
                print(f"Processed {count} messages...")
    print("Conversion complete.")

if __name__ == '__main__':
    run_conversion()
