import sys
import os
import glob
import traceback
from packet_main import Packet
from elftools.elf.elffile import ELFFile

current_prj_path = os.environ.get("CURRENT_PRJ_PATH")
root_dir = os.environ.get("SDK_ROOT_DIR")


try:
    print("image packaging...")
    img_cfg_template = os.path.normpath(os.path.join(root_dir, 'platform',  'utils', 'img_cfg_template.yml'))
    build_dir = os.path.normpath(os.path.join(current_prj_path, 'Obj'))
    Packet.do_run(root_dir, build_dir, img_cfg_template)

except Exception as e:
    print(f"Error: {e}")
    traceback.print_exc()
    sys.exit(1) #异常退出
    