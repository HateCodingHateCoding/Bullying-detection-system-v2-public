import os, stat
import subprocess
import shutil
import re
import argparse
import time
import sys
import os
import pathlib
import traceback
import yaml

from image.img_analysis import Analysis_IMG
from packet_main import ImgtoolSigner

IMAGE_TOOL_PATH = 'tools\scripts\image_packet\image\imgtool.py'
FLASH_LIST_WORK_PATH = 'libs\spiflash'

def get_path(path):
    current_file_path = os.path.abspath(__file__)
    current_folder = os.path.dirname(current_file_path)
    ret_path = os.path.normpath(os.path.join(current_folder, "..", "..", "..", path))
    return ret_path

def get_root_path():
    current_file_path = os.path.abspath(__file__)
    current_folder = os.path.dirname(current_file_path)
    ret_path = os.path.normpath(os.path.join(current_folder, "..", "..", ".."))
    return ret_path


def flash_list_img_import(root_path, work_path, input, output):
    print('= ' * 32)
    print(f"import {input} to {output}")
    print('- ' * 32)
    input_img_path = os.path.join(root_path, work_path, input)
    output_yaml_path = os.path.join(root_path, work_path, output)

    if not os.path.exists(input_img_path):
        raise Exception(f"input image file not found {input_img_path}")

    if os.path.exists(output_yaml_path):
        os.remove(output_yaml_path)

    subimg_list = Analysis_IMG.flash_list_import(input_img_path, output_yaml_path)
    if subimg_list == None:
        raise Exception(f"input image invalid")
    print("\nimport success!!")
    print('= ' * 32)


def flash_list_img_export(root_path, work_path, input, output):
    print('= ' * 32)
    print(f"export {input} to {output}")
    print('- ' * 32)
    input_yaml_path = os.path.join(root_path, work_path, input)
    output_img_path = os.path.join(root_path, work_path, output)

    if not os.path.exists(input_yaml_path):
        raise Exception(f"input yaml file not found {input_yaml_path}")

    base_name, ext = os.path.splitext(output_img_path)
    output_bin_path = base_name + '.bin'

    img_version = Analysis_IMG.flash_list_export(input_yaml_path, output_bin_path)
    str_img_ver = str(img_version['iv_major']) + '.' + str(img_version['iv_minor']) + '.' + \
            str(img_version['iv_revision']) + '+' + str(img_version['iv_build_num'])

    formats = []
    formats.append('bin')
    tool_path = get_path(IMAGE_TOOL_PATH)
    work_path = get_path(FLASH_LIST_WORK_PATH)

    base_conf = {'app-section-max-size': 524288, 
                'bootloader-section-size': 16384,
                'data-storage-size': 8192,
                'fix-sram-map': False, 
                'flash-max-size': '0K',
                'head-size': 32, 
                'sram-base-addr': 4195328, 
                'sram-max-size': '294k'
                }

    subimg = {'cpu-id': 'cpusf',
                'enter-point': 0,
                'move-address': None,
                'img-id': 14, 
                'in-file': None, 
                'merge': False, 
                'name': 'spiflash_list', 
                'out-file': None, 
                'pack': True, 
                'start-run': False, 
                'store-dir': 'sys', 
                'version': str_img_ver
                }

    work_dir = os.path.join(root_path, work_path)
    bin_base, bin_ext = os.path.splitext(output)
    subimg['in-file'] = bin_base + '.bin'
    subimg['in-file'] =  work_dir / pathlib.Path(subimg['in-file'])
    subimg['out-file'] = output
    subimg['out-file'] =  work_dir / pathlib.Path(subimg['out-file'])

    signer = ImgtoolSigner()
    signer.sign(tool_path, work_path, base_conf, subimg, formats)

    print("\nexport success!!")
    print('= ' * 32)


def main(mode, work_path, input, output):

    try:
        root_path = get_root_path()
        root_path = root_path.replace('\\','/')
        work_path = work_path.replace('\\','/')
        if mode == 'import':
            flash_list_img_import(root_path, work_path, input, output)
        elif mode == 'export':
            flash_list_img_export(root_path, work_path, input, output)
        else:
            raise Exception(f"mode error, must import or export")

    except Exception as e:
        print(f"Error: {e}")
        traceback.print_exc()
        sys.exit(1) #异常退出


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-mode', help='import or export')
    parser.add_argument('-path', help='work path, must in sdk dir')
    parser.add_argument('-input', help='input file, img or ymal')
    parser.add_argument('-output', help='input file, img or ymale')
    args = parser.parse_args()

    main(args.mode, args.path, args.input, args.output)
