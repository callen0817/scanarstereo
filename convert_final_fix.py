from pathlib import Path
from rosbags.highlevel import AnyReader
from rosbags.rosbag1 import Writer
from rosbags.typesys import get_typestore, Stores
import os

def run_conversion():
    typestore = get_typestore(Stores.ROS2_HUMBLE)
    
    for bag_name, target_name, remap in [
        ('kalibr_bag', 'kalibr_ros1.bag', {'/oak/rgb/image_rect': '/oak/rgb/image_raw'}),
        ('fastcalib_bag', 'fastcalib_ros1.bag', {})
    ]:
        if not Path(bag_name).exists():
            continue
        if Path(target_name).exists():
            os.remove(target_name)
            
        print(f"Converting {bag_name}...")
        with AnyReader([Path(bag_name)], default_typestore=typestore) as reader, \
             Writer(Path(target_name)) as writer:
            conn_map = {}
            for conn in reader.connections:
                topic = remap.get(conn.topic, conn.topic)
                conn_map[conn.id] = writer.add_connection(topic, conn.msgtype)
            for conn, timestamp, data in reader.messages():
                writer.write(conn_map[conn.id], timestamp, data)

run_conversion()
