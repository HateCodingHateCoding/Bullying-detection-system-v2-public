import abc
import argparse
import os
import pathlib
import pickle
import platform
import shutil
import shlex
import subprocess
import sys
import yaml
import struct
import re
import copy
import glob
from elftools.elf.elffile import ELFFile
from image.img_analysis import Analysis_IMG


IMG_ID_BOOT         = 0
IMG_ID_MERGE        = 1
IMG_ID_SYS0         = 2
IMG_ID_SYS1         = 3
IMG_ID_MMW          = 5
IMG_ID_MMW_ALG      = 6
IMG_ID_MPW          = 8
IMG_ID_CUSTOM_CFG   = 13
IMG_ID_FLASH_LIST   = 14

config_list = [
    {"name":"CONFIG_BOARD_NAME",                "value":""},
    {"name":"CONFIG_BOOT_NAME",                 "value":""},
    {"name":"CONFIG_SYS0_NAME",                 "value":""},
    {"name":"CONFIG_SYS1_NAME",                 "value":""},
    {"name":"CONFIG_MMW_NAME",                  "value":""},
    {"name":"CONFIG_MMW_ALG_NAME",              "value":""},
    {"name":"CONFIG_MPW_NAME",                  "value":""},
    {"name":"CONFIG_FLASH_LIST_NAME",           "value":""},

    {"name":"CONFIG_IMG_BOOT_PATH_STR",         "value":""},
    {"name":"CONFIG_IMG_SYS_PATH_STR",          "value":""},
    {"name":"CONFIG_IMG_MMW_PATH_STR",          "value":""},
    {"name":"CONFIG_IMG_MMW_ALG_PATH_STR",      "value":""},
    {"name":"CONFIG_IMG_MPW_PATH_STR",          "value":""},
    {"name":"CONFIG_IMG_FLASH_LIST_PATH_STR",   "value":""},
    {"name":"CONFIG_IMG_CUSTOM_CFG_PATH_STR",   "value":""},
    
    {"name":"CONFIG_IMG_BOOT",                  "value":0},
    {"name":"CONFIG_IMG_SYS0",                  "value":0},
    {"name":"CONFIG_IMG_SYS1",                  "value":0},
    {"name":"CONFIG_IMG_MMW",                   "value":0},
    {"name":"CONFIG_IMG_MMW_DEVELOP",           "value":0},
    {"name":"CONFIG_IMG_MMW_ALG",               "value":0},
    {"name":"CONFIG_IMG_MMW_ALG_DEVELOP",       "value":0},
    {"name":"CONFIG_IMG_MPW",                   "value":0},
    {"name":"CONFIG_IMG_MPW_DEVELOP",           "value":0},
    {"name":"CONFIG_IMG_FLASH_LIST",            "value":0},
    {"name":"CONFIG_IMG_CUSTOM_CFG",            "value":0},
]

boot_ver_list = [
    {"name":"CONFIG_BOOT_VER_MAJOR",            "value":0},
    {"name":"CONFIG_BOOT_VER_MINOR",            "value":0},
    {"name":"CONFIG_BOOT_VER_REVISION",         "value":0},
]

merge_ver_list = [
    {"name":"CONFIG_MERGE_VER_R_TYPE",          "value":0},
    {"name":"CONFIG_MERGE_VER_R_MINOR",         "value":0},
    {"name":"CONFIG_MERGE_VER_M_NUMBER",        "value":0},
    {"name":"CONFIG_MERGE_VER_M_PROJECT",       "value":0},
    {"name":"CONFIG_MERGE_VER_M_MINOR",         "value":0},
    {"name":"CONFIG_MERGE_VER_V_MAJOR",         "value":0},
    {"name":"CONFIG_MERGE_VER_V_MINOR",         "value":0},
    {"name":"CONFIG_MERGE_VER_V_REVISION",      "value":0},
]

sys_ver_list = [
    {"name":"CONFIG_SYS_VER_V_MAJOR",           "value":0},
    {"name":"CONFIG_SYS_VER_V_MINOR",           "value":0},
    {"name":"CONFIG_SYS_VER_V_REVISION",        "value":0},
    {"name":"CONFIG_SYS_VER_V_BUILD",           "value":0},
]

mmw_ver_list = [
    {"name":"CONFIG_MMW_VER_V_MAJOR",           "value":0},
    {"name":"CONFIG_MMW_VER_V_MINOR",           "value":0},
    {"name":"CONFIG_MMW_VER_V_REVISION",        "value":0},
    {"name":"CONFIG_MMW_VER_V_BUILD",           "value":0},
]

mmw_alg_ver_list = [
    {"name":"CONFIG_MMW_ALG_VER_V_MAJOR",       "value":0},
    {"name":"CONFIG_MMW_ALG_VER_V_MINOR",       "value":0},
    {"name":"CONFIG_MMW_ALG_VER_V_REVISION",    "value":0},
    {"name":"CONFIG_MMW_ALG_VER_V_BUILD",       "value":0},
]

mpw_ver_list = [
    {"name":"CONFIG_MPW_VER_V_MAJOR",           "value":0},
    {"name":"CONFIG_MPW_VER_V_MINOR",           "value":0},
    {"name":"CONFIG_MPW_VER_V_REVISION",        "value":0},
    {"name":"CONFIG_MPW_VER_V_BUILD",           "value":0},
]

section_list = [
    # bootloader
    {"name":"boot_head_pad",    "start-addr":0, "end-addr":0},
    {"name":"boot_isram_text",  "start-addr":0, "end-addr":0},
    {"name":"boot_ixip_text",   "start-addr":0, "end-addr":0},
    {"name":"boot_ixip_rodata", "start-addr":0, "end-addr":0},
    {"name":"boot_dsram_data",  "start-addr":0, "end-addr":0},
    {"name":"boot_dsram_bss",   "start-addr":0, "end-addr":0},
    {"name":"boot_dsram_stack", "start-addr":0, "end-addr":0},

    # system, sys0 or sys1
    {"name":"head_pad",         "start-addr":0, "end-addr":0},
    {"name":"isram_text",       "start-addr":0, "end-addr":0},
    {"name":"isram_rodata",     "start-addr":0, "end-addr":0},
    {"name":"dsram_data",       "start-addr":0, "end-addr":0},
    {"name":"dsram_bss",        "start-addr":0, "end-addr":0},
    {"name":"dsram_stack",      "start-addr":0, "end-addr":0},
    {"name":"ipsram_text",      "start-addr":0, "end-addr":0},
    {"name":"ipsram_rodata",    "start-addr":0, "end-addr":0},
    {"name":"dpsram_data",      "start-addr":0, "end-addr":0},
    {"name":"dpsram_bss",       "start-addr":0, "end-addr":0},
    {"name":"ixip_text",        "start-addr":0, "end-addr":0},
    {"name":"ixip_rodata",      "start-addr":0, "end-addr":0},

    # mmw
    {"name":"mmwhead_pad",      "start-addr":0, "end-addr":0},
    {"name":"mmwisram_text",    "start-addr":0, "end-addr":0},
    {"name":"mmwisram_rodata",  "start-addr":0, "end-addr":0},
    {"name":"mmwdsram_data",    "start-addr":0, "end-addr":0},
    {"name":"mmwdsram_bss",     "start-addr":0, "end-addr":0},
    {"name":"mmwixip_text",     "start-addr":0, "end-addr":0},
    {"name":"mmwixip_rodata",   "start-addr":0, "end-addr":0},

    # mmw alg
    {"name":"mmwalghead_pad",      "start-addr":0, "end-addr":0},
    {"name":"mmwalgisram_text",    "start-addr":0, "end-addr":0},
    {"name":"mmwalgisram_rodata",  "start-addr":0, "end-addr":0},
    {"name":"mmwalgdsram_data",    "start-addr":0, "end-addr":0},
    {"name":"mmwalgdsram_bss",     "start-addr":0, "end-addr":0},
    {"name":"mmwalgixip_text",     "start-addr":0, "end-addr":0},
    {"name":"mmwalgixip_rodata",   "start-addr":0, "end-addr":0},
]


# The numerical values in the yaml file are displayed in hex
def represent_hex(dumper, data):
        return dumper.represent_scalar('tag:yaml.org,2002:int', hex(data))


def git_revision(path):
    rc = subprocess.Popen(['git', 'rev-parse', '--is-inside-work-tree'],
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            cwd=path).wait()
    if rc == 0:
        # A git repo.
        popen = subprocess.Popen(['git', 'rev-parse', 'HEAD'],
                                    stdout=subprocess.PIPE,
                                    stderr=subprocess.PIPE,
                                    cwd=path)
        stdout, stderr = popen.communicate()
        stdout = stdout.decode('utf-8')

        if not (popen.returncode or stderr):
            revision = stdout.rstrip()

            rc = subprocess.Popen(['git', 'diff-index', '--quiet', 'HEAD',
                                    '--'],
                                    stdout=None,
                                    stderr=None,
                                    cwd=path).wait()
            if rc:
                return revision + '-dirty', True
            return revision, False
    return None, False

def get_version_info_from_elf(macros, elf_path, root_path):
    version_value = {}
    if (macros == 'CONFIG_IMG_MERGE'):
        get_version_info(elf_path, merge_ver_list)
        ver_conf = Build_Info(merge_ver_list)
        r_type = ver_conf.get('CONFIG_MERGE_VER_R_TYPE')
        r_min = ver_conf.get('CONFIG_MERGE_VER_R_MINOR')
        m_num = ver_conf.get('CONFIG_MERGE_VER_M_NUMBER')
        m_prj = ver_conf.get('CONFIG_MERGE_VER_M_PROJECT')
        m_min = ver_conf.get('CONFIG_MERGE_VER_M_MINOR')
        v_maj = ver_conf.get('CONFIG_MERGE_VER_V_MAJOR')
        v_min = ver_conf.get('CONFIG_MERGE_VER_V_MINOR')
        v_rev = ver_conf.get('CONFIG_MERGE_VER_V_REVISION')
        version_value['r'] = str(r_type) + '.' + str(r_min)
        version_value['m'] = str(m_num) + '.' + str(m_prj) + '.' + str(m_min)
        version_value['v'] = str(v_maj) + '.' + str(v_min) + '.' + str(v_rev)
        return version_value
    elif macros == 'CONFIG_IMG_BOOT':
        get_version_info(elf_path, boot_ver_list)
        ver_conf = Build_Info(boot_ver_list)
        v_maj = ver_conf.get('CONFIG_BOOT_VER_MAJOR')
        v_min = ver_conf.get('CONFIG_BOOT_VER_MINOR')
        v_rev = ver_conf.get('CONFIG_BOOT_VER_REVISION')
        v_version = str(v_maj) + '.' + str(v_min) + '.' + str(v_rev)
        version_value['v'] = v_version
        return version_value
    elif (macros == 'CONFIG_IMG_SYS0') or (macros == 'CONFIG_IMG_SYS1'):
        get_version_info(elf_path, sys_ver_list)
        ver_conf = Build_Info(sys_ver_list)
        v_maj = ver_conf.get('CONFIG_SYS_VER_V_MAJOR')
        v_min = ver_conf.get('CONFIG_SYS_VER_V_MINOR')
        v_rev = ver_conf.get('CONFIG_SYS_VER_V_REVISION')
        v_build = ver_conf.get('CONFIG_SYS_VER_V_BUILD')

        v_version = str(v_maj) + '.' + str(v_min) + '.' + str(v_rev) + '+' + str(v_build)
        version_value['v'] = v_version
        return version_value
    elif macros == 'CONFIG_IMG_MMW':
        get_version_info(elf_path, mmw_ver_list)
        ver_conf = Build_Info(mmw_ver_list)
        v_maj = ver_conf.get('CONFIG_MMW_VER_V_MAJOR')
        v_min = ver_conf.get('CONFIG_MMW_VER_V_MINOR')
        v_rev = ver_conf.get('CONFIG_MMW_VER_V_REVISION')
        str_build = git_revision(root_path)
        if str_build[0] == None:
            v_build = ver_conf.get('CONFIG_SYS_VER_V_BUILD')
        else:
            v_build = str_build[0][0:8]

        v_version = str(v_maj) + '.' + str(v_min) + '.' + str(v_rev) + '+' + str(v_build)
        version_value['v'] = v_version
        return version_value
    elif macros == 'CONFIG_IMG_MMW_ALG':
        get_version_info(elf_path, mmw_alg_ver_list)
        ver_conf = Build_Info(mmw_alg_ver_list)
        v_maj = ver_conf.get('CONFIG_MMW_ALG_VER_V_MAJOR')
        v_min = ver_conf.get('CONFIG_MMW_ALG_VER_V_MINOR')
        v_rev = ver_conf.get('CONFIG_MMW_ALG_VER_V_REVISION')
        str_build = git_revision(root_path)
        if str_build[0] == None:
            v_build = ver_conf.get('CONFIG_SYS_VER_V_BUILD')
        else:
            v_build = str_build[0][0:8]
        v_version = str(v_maj) + '.' + str(v_min) + '.' + str(v_rev) + '+' + str(v_build)
        version_value['v'] = v_version
        return version_value
    elif macros == 'CONFIG_IMG_MPW':
        get_version_info(elf_path, mpw_ver_list)
        ver_conf = Build_Info(mpw_ver_list)
        v_maj = ver_conf.get('CONFIG_MPW_VER_V_MAJOR')
        v_min = ver_conf.get('CONFIG_MPW_VER_V_MINOR')
        v_rev = ver_conf.get('CONFIG_MPW_VER_V_REVISION')
        str_build = git_revision(root_path)
        if str_build[0] == None:
            v_build = ver_conf.get('CONFIG_SYS_VER_V_BUILD')
        else:
            v_build = str_build[0][0:8]
        v_version = str(v_maj) + '.' + str(v_min) + '.' + str(v_rev) + '+' + str(v_build)
        version_value['v'] = v_version
        return version_value

def get_img_info_from_elf(sub_img_cfg, elf_path, macros, root_dir, build_dir, store_dir, section_conf):

    in_file =  sub_img_cfg['name'] + '.bin'
    out_file = sub_img_cfg['name'] + '.img'
    in_file_path = os.path.normpath(os.path.join(build_dir, in_file)) 
    out_file_path = os.path.normpath(os.path.join(build_dir, out_file))

    if os.path.exists(in_file_path) and os.path.getsize(in_file_path) > 0:
        os.remove(in_file_path)
        if os.path.exists(out_file_path):
            os.remove(out_file_path)

    remap_list = []
    move_list = []
    bin_size = 0

    start_run = False
    enter_point = 0
    img_id = 0
    cpu_id = 'cpusf'
    if macros == 'CONFIG_IMG_SYS0':
        start_run = True
        cpu_id = 'cpus'
        img_id = IMG_ID_SYS0
    elif macros == 'CONFIG_IMG_SYS1':
        start_run = True
        cpu_id = 'cpuf'
        img_id = IMG_ID_SYS1
    elif macros == 'CONFIG_IMG_MMW':
        cpu_id = 'cpusf'
        img_id = IMG_ID_MMW
    elif macros == 'CONFIG_IMG_MMW_ALG':
        cpu_id = 'cpusf'
        img_id = IMG_ID_MMW_ALG
    elif macros == 'CONFIG_IMG_MPW':
        cpu_id = 'cpusf'
        img_id = IMG_ID_MPW


    with open(elf_path, 'rb') as f:
        elf = ELFFile(f)
        with open(in_file_path, 'wb') as wf:
            flash_addr = section_conf.get_start_addr('head_pad')
            flash_offset = 0
            # section = elf.get_section_by_name("head_pad")
            # data = section.data()
            # wf.write(data)
            # section_size = section.data_size
            head_pad_size = section_conf.get_size('head_pad')
            flash_offset += head_pad_size
            # print(f"head-pad : size {head_pad_size} off {flash_offset}")


            # ixip-text ixip-rodata ---------------------------------------------------
            ixip_text_size =  section_conf.get_size('ixip_text')
            ixip_text_addr = section_conf.get_start_addr('ixip_text')
            if (flash_addr + flash_offset) != ixip_text_addr:
                raise Exception(f"error: ixip text {hex(ixip_text_addr)}")
            # fill ixip-text    
            if ixip_text_size > 0:
                section = elf.get_section_by_name("ixip_text")
                data = section.data()
                if len(data) <= ixip_text_size:
                    wf.write(data)
                    flash_offset += len(data)                    
                else:
                    raise Exception(f"error: ixip text len")
            # print(f"ixip-text : size {ixip_text_size} off {flash_offset}")

            # ixip-rodata
            ixip_rodata_size =  section_conf.get_size('ixip_rodata')
            ixip_rodata_addr = section_conf.get_start_addr('ixip_rodata')
            # fill pad data
            ixip_pad_len = ixip_rodata_addr - (flash_addr + flash_offset)
            if ixip_pad_len > 0:
                padding = bytearray([0xFF] * ixip_pad_len)
                wf.write(padding)
                flash_offset += ixip_pad_len
                print(f"Fill data for ixip-text: {ixip_pad_len}")
            elif ixip_pad_len < 0:
                raise Exception(f"error: ixip rodata start address less ixip text end address")
            # fill ixip-rodata
            if ixip_rodata_size > 0:
                section = elf.get_section_by_name("ixip_rodata")
                data = section.data()
                if len(data) <= ixip_rodata_size:
                    wf.write(data)
                    flash_offset += len(data)                    
                else:
                    raise Exception(f"error: ixip rodata len")
            # print(f"ixip-text: adddr: {hex(ixip_text_addr)}, size: {hex(ixip_text_size)}")
            # print(f"ixip-rodata: adddr: {hex(ixip_rodata_addr)}, size: {hex(ixip_rodata_size)}")
            # print(f"flash-offset: {hex(flash_offset)}")


            # isram-text + dsram-rodata -----------------------------------------------
            iram_addr = 0
            isram_size = 0
            iram_flash_offset = flash_offset

            # fill iram-text
            isram_text_addr = section_conf.get_start_addr('isram_text')
            isram_text_size =  section_conf.get_size('isram_text')
            iram_addr = isram_text_addr
            if isram_text_size > 0:
                section = elf.get_section_by_name("isram_text")
                data = section.data()
                if len(data) <= isram_text_size:
                    wf.write(data)
                    flash_offset += len(data)
                    isram_size += len(data)              
                else:
                    raise Exception(f"error: iram text len")

            # iram-rodata
            isram_rodata_addr = section_conf.get_start_addr('isram_rodata')
            isram_rodata_size =  section_conf.get_size('isram_rodata')
            # fill pad data
            iram_pad_len = isram_rodata_addr - (iram_addr + isram_size)
            # print(f"isram_rodata_addr: {hex(isram_rodata_addr)}, iram_addr: {hex(iram_addr)}, isram_size: {hex(isram_size)}")
            # print(f"iram_pad_len : {hex(iram_pad_len)}")
            if iram_pad_len > 0:
                padding = bytearray([0xFF] * iram_pad_len)
                wf.write(padding)
                flash_offset += iram_pad_len
                isram_size += iram_pad_len
                print(f"Fill data for isram-rodata: {iram_pad_len}")
            elif iram_pad_len < 0:
                # print(f'isram_rodata_addr: {hex(isram_rodata_addr)}')
                # print(f'flash_addr: {hex(flash_addr)}')
                # print(f'flash_offset: {hex(flash_offset)}')
                raise Exception(f"error: iram rodata start address less iram text end address")
            # fill rodata
            if isram_rodata_size > 0:
                section = elf.get_section_by_name("isram_rodata")
                data = section.data()
                if len(data) <= isram_rodata_size:
                    wf.write(data)
                    flash_offset += len(data)
                    isram_size += len(data)              
                else:
                    raise Exception(f"error: iram rodata len")
            # remap -ram
            if isram_size > 0:
                isram_remap_size = isram_size
                if isram_remap_size % 128 > 0 :
                    isram_remap_size += (128 - isram_remap_size % 128)
                if macros == 'CONFIG_IMG_SYS0':
                    remap_list.append({'type' : 'sram-ibus-cpus',
                                    'offset' : 0,
                                    'run-address' : iram_addr,
                                    'size' : isram_remap_size})
                    enter_point = iram_addr
                elif macros == 'CONFIG_IMG_SYS1': 
                    remap_list.append({'type' : 'sram-ibus-cpuf',
                                    'offset' : 0,
                                    'run-address' : iram_addr,
                                    'size' : isram_remap_size})
                    enter_point = iram_addr
                # move
                move_list.append({'subimg-offset': iram_flash_offset,
                        'run-address': iram_addr,
                        'size' :  isram_size})
            # print(f"isram-text: adddr: {hex(isram_text_addr)}, size: {hex(isram_text_size)}")
            # print(f"isram-rodata: adddr: {hex(isram_rodata_addr)}, size: {hex(isram_rodata_size)}")
            # print(f"flash-offset: {hex(flash_offset)}")


            # dram-data, dram-bss, dram-stack -------------------------------------------
            dram_addr = 0
            dram_flash_offset = flash_offset

            dsram_data_addr = section_conf.get_start_addr('dsram_data')
            dsram_data_size =  section_conf.get_size('dsram_data')
            dram_addr = dsram_data_addr
            if dsram_data_size > 0:
                section = elf.get_section_by_name("dsram_data")
                data = section.data()
                if len(data) <= dsram_data_size:
                    wf.write(data)
                    flash_offset += len(data)
                    # dsram_size += len(data)              
                else:
                    raise Exception(f"error: dram data len")
            # dsram_bss_size =  section_conf.get_size('dsram_bss')
            dsram_stack_size =  section_conf.get_size('dsram_stack')
            dsram_stack_addr = section_conf.get_start_addr('dsram_stack')
            # print(f"dsram_stack_addr: {hex(dsram_stack_addr)} dsram_stack_size : {hex(dsram_stack_size)}")
            # remap
            dsram_remap_size = dsram_stack_addr + dsram_stack_size - dsram_data_addr
            # print(f"dsram_remap_size: {hex(dsram_remap_size)}")
            if dsram_remap_size > 0:
                if dsram_remap_size % 128 > 0 :
                    dsram_remap_size += (128 - dsram_remap_size % 128)
                if macros == 'CONFIG_IMG_SYS0':
                    remap_list.append({'type' : 'sram-dbus-cpus',
                                    'offset' : 0,
                                    'run-address' : dram_addr,
                                    'size' : dsram_remap_size})
                elif macros == 'CONFIG_IMG_SYS1': 
                    remap_list.append({'type' : 'sram-dbus-cpuf',
                                    'offset' : 0,
                                    'run-address' : dram_addr,
                                    'size' : dsram_remap_size})
            # move
            if dsram_data_size > 0:
                move_list.append({'subimg-offset': dram_flash_offset,
                        'run-address': dram_addr,
                        'size' :  dsram_data_size})
            # print(f"dsram-data: adddr: {hex(dsram_data_addr)}, size: {hex(dsram_data_size)}")
            # print(f"dsram-bss: size: {hex(dsram_bss_size)}")
            # print(f"dsram-stask: size: {hex(dsram_stack_size)}")
            # print(f"flash-offset: {hex(flash_offset)}")


            # ipsram-text ipsram-rodata ---------------------------------------------
            ipsram_flash_offset = flash_offset
            ipsram_size = 0
            ipsram_addr = 0
            ipsram_text_size =  section_conf.get_size('ipsram_text')
            ipsram_text_addr = section_conf.get_start_addr('ipsram_text')
            ipsram_addr = ipsram_text_addr
            # fill ipsram-text
            if ipsram_text_size > 0:
                section = elf.get_section_by_name("ipsram_text")
                data = section.data()
                if len(data) <= ipsram_text_size:
                    wf.write(data)
                    flash_offset += len(data)
                    ipsram_size += len(data)              
                else:
                    raise Exception(f"error: ipsram text len")

            # ipsram-rodata
            ipsram_rodata_addr = section_conf.get_start_addr('ipsram_rodata')
            ipsram_rodata_size =  section_conf.get_size('ipsram_rodata')
            # fill pad data
            ipsram_pad_len = ipsram_rodata_addr - (ipsram_addr + ipsram_size)
            # print(f"ipsram_rodata_addr: {hex(ipsram_rodata_addr)}, ipsram_addr: {hex(ipsram_addr)}, ipsram_size: {hex(ipsram_size)}")
            # print(f"ipsram_pad_len : {hex(ipsram_pad_len)}")
            if ipsram_pad_len > 0:
                padding = bytearray([0xFF] * ipsram_pad_len)
                wf.write(padding)
                flash_offset += ipsram_pad_len
                ipsram_size += ipsram_pad_len
                print(f"Fill data for isram-rodata: {ipsram_pad_len}")
            elif ipsram_pad_len < 0:
                raise Exception(f"error: ipram rodata start address less ipram text end address")
            # fill rodata
            if ipsram_rodata_size > 0:
                section = elf.get_section_by_name("ipsram_rodata")
                data = section.data()
                if len(data) <= ipsram_rodata_size:
                    wf.write(data)
                    flash_offset += len(data)
                    ipsram_size += len(data)              
                else:
                    raise Exception(f"error: iram rodata len")

            if ipsram_size > 0:
                # remap
                ipsram_text_remap_size = ipsram_size
                if ipsram_text_remap_size % 4096 > 0 :
                    ipsram_text_remap_size += (4096 - ipsram_text_remap_size % 4096)
                remap_list.append({'type' : 'psram-ibus',
                                'offset' : 0,
                                'run-address' : ipsram_addr,
                                'size' : ipsram_text_remap_size})
                # move
                move_list.append({'subimg-offset': ipsram_flash_offset,
                        'run-address': ipsram_addr,
                        'size' :  ipsram_size})
            # print(f"ipsram-text: adddr: {hex(ipsram_text_addr)}, size: {hex(ipsram_text_size)}")
            # print(f"ipsram-rodata: adddr: {hex(ipsram_rodata_addr)}, size: {hex(ipsram_rodata_size)}")
            # print(f"flash-offset: {hex(flash_offset)}")


            # dsram-data + dsram-stack -----------------------------------
            dpram_addr = 0
            dpsram_size = 0
            dpram_flash_offset = flash_offset

            dpsram_data_addr = section_conf.get_start_addr('dpsram_data')
            dpsram_data_size =  section_conf.get_size('dpsram_data')
            dpram_addr = dpsram_data_addr
            if dpsram_data_size > 0:
                section = elf.get_section_by_name("dpsram_data")
                data = section.data()
                if len(data) <= dpsram_data_size:
                    wf.write(data)
                    flash_offset += len(data)
                    dpsram_size += len(data)              
                else:
                    raise Exception(f"error: dpsram data len")

            dpsram_bss_size =  section_conf.get_size('dpsram_bss')
            dpsram_stack_size =  section_conf.get_size('dpsram_stack')
            dpsram_size += dpsram_bss_size + dpsram_stack_size
            # remap
            dpsram_remap_size = dpsram_size
            if dpsram_remap_size > 0:
                if dpsram_remap_size % 4096 > 0 :
                    dpsram_remap_size += (4096 - dpsram_remap_size % 4096)
                remap_list.append({'type' : 'psram-dbus',
                                'offset' : 0,
                                'run-address' : dpram_addr,
                                'size' : dpsram_remap_size})

            # move
            if dpsram_data_size > 0:
                move_list.append({'subimg-offset': dpram_flash_offset,
                        'run-address': dpram_addr,
                        'size' :  dpsram_data_size})
            # print(f"dpsram-data: adddr: {hex(dpsram_data_addr)}, size: {hex(dpsram_data_size)}")
            # print(f"dpsram-bss: size: {hex(dpsram_bss_size)}")
            # print(f"dpsram-stask: size: {hex(dpsram_stack_size)}")
            # print(f"flash-offset: {hex(flash_offset)}")


            # ixip ------------------------------------------------
            ixip_remap_size = flash_offset
            if ixip_remap_size % 4096 > 0 :
                ixip_remap_size += (4096 - ixip_remap_size % 4096)
            remap_list.append({'type' : 'xip',
                                'offset' : 0,
                                'run-address' : flash_addr,
                                'size' : ixip_remap_size}) 
            bin_size = flash_offset
            # print(f"flash-offset: {hex(flash_offset)}")


    subimg_list = []
    if bin_size > 32:
        if os.path.exists(out_file_path):
            os.remove(out_file_path)

        subimg_list.append({'name' : sub_img_cfg['name'],
                            'cpu-id' : cpu_id,
                            'img-id' : img_id,
                            'version' : "0.0.0",
                            'start-run' : start_run,
                            'enter-point' : enter_point,
                            'move-address' : move_list,
                            'remap-address' : remap_list,
                            'in-file' : in_file,
                            'out-file' : out_file,
                            'store-dir' : store_dir,
                            'merge' : True,
                            'pack' : True})
    else :
        img_path = os.path.normpath(os.path.join(root_dir, 'libs', store_dir, out_file))
        if os.path.exists(out_file_path):
            os.remove(out_file_path)

        if os.path.exists(img_path):
            shutil.copy(img_path, out_file_path)
            subimg_list = Analysis_IMG.get_img_info(out_file_path)
            subimg_list[0]['pack'] = False
            subimg_list[0]['merge'] = True
            subimg_list[0]['out-file'] = out_file_path
        else :
            subimg_list.append({'merge': False, 'pack' : False})


    return subimg_list


def get_mmw_img_info_from_elf(sub_img_cfg, elf_path, root_dir, build_dir, store_dir, section_conf):

    in_file =  sub_img_cfg['name'] + '.bin'
    out_file = sub_img_cfg['name'] + '.img'
    in_file_path = os.path.normpath(os.path.join(build_dir, in_file)) 
    out_file_path = os.path.normpath(os.path.join(build_dir, out_file))

    if os.path.exists(in_file_path) and os.path.getsize(in_file_path) > 0:
        os.remove(in_file_path)
        if os.path.exists(out_file_path):
            os.remove(out_file_path)

    remap_list = []
    move_list = []
    bin_size = 0

    start_run = False
    enter_point = 0
    img_id = IMG_ID_MMW
    cpu_id = 'cpusf'

    with open(elf_path, 'rb') as f:
        elf = ELFFile(f)
        with open(in_file_path, 'wb') as wf:
            flash_addr = section_conf.get_start_addr('mmwhead_pad')
            flash_offset = 0
            # section = elf.get_section_by_name("head_pad")
            # data = section.data()
            # wf.write(data)
            # section_size = section.data_size
            head_pad_size = section_conf.get_size('mmwhead_pad')
            flash_offset += head_pad_size
            # print(f"head-pad : size {head_pad_size} off {flash_offset}")


            # ixip-text ixip-rodata ---------------------------------------------------
            ixip_text_size =  section_conf.get_size('mmwixip_text')
            ixip_text_addr = section_conf.get_start_addr('mmwixip_text')
            if (flash_addr + flash_offset) != ixip_text_addr:
                raise Exception("error: mmwixip text")
                
            if ixip_text_size > 0:
                section = elf.get_section_by_name("mmwixip_text")
                data = section.data()
                if len(data) <= ixip_text_size:
                    wf.write(data)
                    flash_offset += len(data)                    
                else:
                    raise Exception(f"error: mmwixip text len")
            # print(f"ixip-text : size {ixip_text_size} off {flash_offset}")

            # ixip-rodata
            ixip_rodata_size =  section_conf.get_size('mmwixip_rodata')
            ixip_rodata_addr = section_conf.get_start_addr('mmwixip_rodata')
            ixip_pad_len = ixip_rodata_addr - (flash_addr + flash_offset)
            # fill pad data
            if ixip_pad_len > 0:
                padding = bytearray([0xFF] * ixip_pad_len)
                wf.write(padding)
                flash_offset += ixip_pad_len
                print(f"Fill data for ixip-text: {ixip_pad_len}")
            elif ixip_pad_len < 0:
                raise Exception(f"error: mmwixip rodata start address less mmwixip text end address")
            # fill ixip-rodata
            if ixip_rodata_size > 0:
                section = elf.get_section_by_name("mmwixip_rodata")
                data = section.data()
                if len(data) <= ixip_rodata_size:
                    wf.write(data)
                    flash_offset += len(data)                    
                else:
                    raise Exception(f"error: ixip rodata len")
            # print(f"ixip-text: adddr: {hex(ixip_text_addr)}, size: {hex(ixip_text_size)}")
            # print(f"ixip-rodata: adddr: {hex(ixip_rodata_addr)}, size: {hex(ixip_rodata_size)}")
            # print(f"flash-offset: {hex(flash_offset)}")


            # isram-text + dsram-rodata -----------------------------------------------
            iram_addr = 0
            isram_size = 0
            iram_flash_offset = flash_offset

            # fill iram-text
            isram_text_addr = section_conf.get_start_addr('mmwisram_text')
            isram_text_size =  section_conf.get_size('mmwisram_text')
            iram_addr = isram_text_addr
            if isram_text_size > 0:
                section = elf.get_section_by_name("mmwisram_text")
                data = section.data()
                if len(data) <= isram_text_size:
                    wf.write(data)
                    flash_offset += len(data)
                    isram_size += len(data)              
                else:
                    raise Exception(f"error: mmwiram text len")
            
            # iram-rodata
            isram_rodata_addr = section_conf.get_start_addr('mmwisram_rodata')
            isram_rodata_size =  section_conf.get_size('mmwisram_rodata')
            # fill pad data
            iram_pad_len = isram_rodata_addr - (iram_addr + isram_size)
            # print(f"isram_rodata_addr: {hex(isram_rodata_addr)}, iram_addr: {hex(iram_addr)}, isram_size: {hex(isram_size)}")
            # print(f"iram_pad_len : {hex(iram_pad_len)}")
            if iram_pad_len > 0:
                padding = bytearray([0xFF] * iram_pad_len)
                wf.write(padding)
                flash_offset += iram_pad_len
                isram_size += iram_pad_len
                print(f"Fill data for isram-rodata: {iram_pad_len}")
            elif iram_pad_len < 0:
                # print(f'isram_rodata_addr: {hex(isram_rodata_addr)}')
                # print(f'flash_addr: {hex(flash_addr)}')
                # print(f'flash_offset: {hex(flash_offset)}')
                raise Exception(f"error: iram rodata start address less iram text end address")
            # fill rodata
            if isram_rodata_size > 0:
                section = elf.get_section_by_name("mmwisram_rodata")
                data = section.data()
                if len(data) <= isram_rodata_size:
                    wf.write(data)
                    flash_offset += len(data)
                    isram_size += len(data)              
                else:
                    raise Exception(f"error: iram rodata len")
            # remap -ram
            if isram_size > 0:
                isram_remap_size = isram_size
                if isram_remap_size % 128 > 0 :
                    isram_remap_size += (128 - isram_remap_size % 128)
                remap_list.append({'type' : 'sram-ibus-cpusf',
                                'offset' : 0,
                                'run-address' : iram_addr,
                                'size' : isram_remap_size})
                enter_point = iram_addr   
                # move
                move_list.append({'subimg-offset': iram_flash_offset,
                        'run-address': iram_addr,
                        'size' :  isram_size})
            # print(f"isram-text: adddr: {hex(isram_text_addr)}, size: {hex(isram_text_size)}")
            # print(f"isram-rodata: adddr: {hex(isram_rodata_addr)}, size: {hex(isram_rodata_size)}")
            # print(f"flash-offset: {hex(flash_offset)}")


            # dram-data, dram-bss, dram-stack -------------------------------------------
            dram_addr = 0
            dram_flash_offset = flash_offset

            dsram_data_addr = section_conf.get_start_addr('mmwdsram_data')
            dsram_data_size =  section_conf.get_size('mmwdsram_data')
            dram_addr = dsram_data_addr
            if dsram_data_size > 0:
                section = elf.get_section_by_name("mmwdsram_data")
                data = section.data()
                if len(data) <= dsram_data_size:
                    wf.write(data)
                    flash_offset += len(data)            
                else:
                    raise Exception(f"error: dram data len")

            dsram_bss_size =  section_conf.get_size('mmwdsram_bss')
            dsram_bss_addr = section_conf.get_start_addr('mmwdsram_bss')
            # remap
            dsram_remap_size = dsram_bss_addr + dsram_bss_size - dsram_data_addr
            if dsram_remap_size > 0:
                if dsram_remap_size % 128 > 0 :
                    dsram_remap_size += (128 - dsram_remap_size % 128)
                remap_list.append({'type' : 'sram-dbus-cpusf',
                                'offset' : 0,
                                'run-address' : dram_addr,
                                'size' : dsram_remap_size})
            # move
            if dsram_data_size > 0:
                move_list.append({'subimg-offset': dram_flash_offset,
                        'run-address': dram_addr,
                        'size' :  dsram_data_size})
            # print(f"dsram-data: adddr: {hex(dsram_data_addr)}, size: {hex(dsram_data_size)}")
            # print(f"dsram-bss: size: {hex(dsram_bss_size)}")
            # print(f"dsram-stask: size: {hex(dsram_stack_size)}")
            # print(f"flash-offset: {hex(flash_offset)}")


            # ixip ------------------------------------------------
            ixip_remap_size = flash_offset
            if ixip_remap_size % 4096 > 0 :
                ixip_remap_size += (4096 - ixip_remap_size % 4096)
            remap_list.append({'type' : 'xip',
                                'offset' : 0,
                                'run-address' : flash_addr,
                                'size' : ixip_remap_size}) 

            bin_size = flash_offset

    subimg_list = []
    if bin_size > 32:
        print(f'bin size: {bin_size} ({bin_size / 1024} KB)')
        if os.path.exists(out_file_path):
            os.remove(out_file_path)

        subimg_list.append({'name' : sub_img_cfg['name'],
                            'cpu-id' : cpu_id,
                            'img-id' : img_id,
                            'version' : "0.0.0",
                            'start-run' : start_run,
                            'enter-point' : enter_point,
                            'move-address' : move_list,
                            'remap-address' : remap_list,
                            'in-file' : in_file,
                            'out-file' : out_file,
                            'store-dir' : store_dir,
                            'merge' : True,
                            'pack' : True})
    else :
        img_path = os.path.normpath(os.path.join(root_dir, 'libs', store_dir, out_file))
        if os.path.exists(out_file_path):
            os.remove(out_file_path)

        if os.path.exists(img_path):
            shutil.copy(img_path, out_file_path)
            subimg_list = Analysis_IMG.get_img_info(out_file_path)
            subimg_list[0]['pack'] = False
            subimg_list[0]['merge'] = True
            subimg_list[0]['out-file'] = out_file_path
        else :
            subimg_list.append({'merge': False, 'pack' : False})

    return subimg_list


def get_mmw_alg_img_info_from_elf(sub_img_cfg, elf_path, root_dir, build_dir, store_dir, section_conf):

    in_file =  sub_img_cfg['name'] + '.bin'
    out_file = sub_img_cfg['name'] + '.img'
    in_file_path = os.path.normpath(os.path.join(build_dir, in_file))
    out_file_path = os.path.normpath(os.path.join(build_dir, out_file))

    if os.path.exists(in_file_path) and os.path.getsize(in_file_path) > 0:
        os.remove(in_file_path)
        if os.path.exists(out_file_path):
            os.remove(out_file_path)

    remap_list = []
    move_list = []
    bin_size = 0

    start_run = False
    enter_point = 0
    img_id = IMG_ID_MMW_ALG
    cpu_id = 'cpusf'

    with open(elf_path, 'rb') as f:
        elf = ELFFile(f)
        with open(in_file_path, 'wb') as wf:
            flash_addr = section_conf.get_start_addr('mmwalghead_pad')
            flash_offset = 0
            # section = elf.get_section_by_name("head_pad")
            # data = section.data()
            # wf.write(data)
            # section_size = section.data_size
            head_pad_size = section_conf.get_size('mmwalghead_pad')
            flash_offset += head_pad_size
            # print(f"head-pad : size {head_pad_size} off {flash_offset}")


            # ixip-text ixip-rodata ---------------------------------------------------
            ixip_text_size =  section_conf.get_size('mmwalgixip_text')
            ixip_text_addr = section_conf.get_start_addr('mmwalgixip_text')
            if (flash_addr + flash_offset) != ixip_text_addr:
                raise Exception("error: mmwalgixip text")

            if ixip_text_size > 0:
                section = elf.get_section_by_name("mmwalgixip_text")
                data = section.data()
                if len(data) <= ixip_text_size:
                    wf.write(data)
                    flash_offset += len(data)
                else:
                    raise Exception(f"error: mmwalgixip text len")
            # print(f"ixip-text : size {ixip_text_size} off {flash_offset}")

            # ixip-rodata
            ixip_rodata_size =  section_conf.get_size('mmwalgixip_rodata')
            ixip_rodata_addr = section_conf.get_start_addr('mmwalgixip_rodata')
            ixip_pad_len = ixip_rodata_addr - (flash_addr + flash_offset)
            # fill pad data
            if ixip_pad_len > 0:
                padding = bytearray([0xFF] * ixip_pad_len)
                wf.write(padding)
                flash_offset += ixip_pad_len
                print(f"Fill data for ixip-text: {ixip_pad_len}")
            elif ixip_pad_len < 0:
                raise Exception(f"error: mmwalgixip rodata start address less mmwalgixip text end address")
            # fill ixip-rodata
            if ixip_rodata_size > 0:
                section = elf.get_section_by_name("mmwalgixip_rodata")
                data = section.data()
                if len(data) <= ixip_rodata_size:
                    wf.write(data)
                    flash_offset += len(data)
                else:
                    raise Exception(f"error: ixip rodata len")
            # print(f"ixip-text: adddr: {hex(ixip_text_addr)}, size: {hex(ixip_text_size)}")
            # print(f"ixip-rodata: adddr: {hex(ixip_rodata_addr)}, size: {hex(ixip_rodata_size)}")
            # print(f"flash-offset: {hex(flash_offset)}")


            # isram-text + dsram-rodata -----------------------------------------------
            iram_addr = 0
            isram_size = 0
            iram_flash_offset = flash_offset

            # fill iram-text
            isram_text_addr = section_conf.get_start_addr('mmwalgisram_text')
            isram_text_size =  section_conf.get_size('mmwalgisram_text')
            iram_addr = isram_text_addr
            if isram_text_size > 0:
                section = elf.get_section_by_name("mmwalgisram_text")
                data = section.data()
                if len(data) <= isram_text_size:
                    wf.write(data)
                    flash_offset += len(data)
                    isram_size += len(data)
                else:
                    raise Exception(f"error: mmwalgiram text len")

            # iram-rodata
            isram_rodata_addr = section_conf.get_start_addr('mmwalgisram_rodata')
            isram_rodata_size =  section_conf.get_size('mmwalgisram_rodata')
            # fill pad data
            iram_pad_len = isram_rodata_addr - (iram_addr + isram_size)
            # print(f"isram_rodata_addr: {hex(isram_rodata_addr)}, iram_addr: {hex(iram_addr)}, isram_size: {hex(isram_size)}")
            # print(f"iram_pad_len : {hex(iram_pad_len)}")
            if iram_pad_len > 0:
                padding = bytearray([0xFF] * iram_pad_len)
                wf.write(padding)
                flash_offset += iram_pad_len
                isram_size += iram_pad_len
                print(f"Fill data for isram-rodata: {iram_pad_len}")
            elif iram_pad_len < 0:
                # print(f'isram_rodata_addr: {hex(isram_rodata_addr)}')
                # print(f'flash_addr: {hex(flash_addr)}')
                # print(f'flash_offset: {hex(flash_offset)}')
                raise Exception(f"error: iram rodata start address less iram text end address")
            # fill rodata
            if isram_rodata_size > 0:
                section = elf.get_section_by_name("mmwalgisram_rodata")
                data = section.data()
                if len(data) <= isram_rodata_size:
                    wf.write(data)
                    flash_offset += len(data)
                    isram_size += len(data)
                else:
                    raise Exception(f"error: iram rodata len")
            # remap -ram
            if isram_size > 0:
                isram_remap_size = isram_size
                if isram_remap_size % 128 > 0 :
                    isram_remap_size += (128 - isram_remap_size % 128)
                remap_list.append({'type' : 'sram-ibus-cpusf',
                                'offset' : 0,
                                'run-address' : iram_addr,
                                'size' : isram_remap_size})
                enter_point = iram_addr
                # move
                move_list.append({'subimg-offset': iram_flash_offset,
                        'run-address': iram_addr,
                        'size' :  isram_size})
            # print(f"isram-text: adddr: {hex(isram_text_addr)}, size: {hex(isram_text_size)}")
            # print(f"isram-rodata: adddr: {hex(isram_rodata_addr)}, size: {hex(isram_rodata_size)}")
            # print(f"flash-offset: {hex(flash_offset)}")


            # dram-data, dram-bss, dram-stack -------------------------------------------
            dram_addr = 0
            dram_flash_offset = flash_offset

            dsram_data_addr = section_conf.get_start_addr('mmwalgdsram_data')
            dsram_data_size =  section_conf.get_size('mmwalgdsram_data')
            dram_addr = dsram_data_addr
            if dsram_data_size > 0:
                section = elf.get_section_by_name("mmwalgdsram_data")
                data = section.data()
                if len(data) <= dsram_data_size:
                    wf.write(data)
                    flash_offset += len(data)
                else:
                    raise Exception(f"error: dram data len")

            # remap
            dsram_bss_size =  section_conf.get_size('mmwalgdsram_bss')
            dsram_bss_addr = section_conf.get_start_addr('mmwalgdsram_bss')
            # remap
            dsram_remap_size = dsram_bss_addr + dsram_bss_size - dsram_data_addr
            if dsram_remap_size > 0:
                if dsram_remap_size % 128 > 0 :
                    dsram_remap_size += (128 - dsram_remap_size % 128)
                remap_list.append({'type' : 'sram-dbus-cpusf',
                                'offset' : 0,
                                'run-address' : dram_addr,
                                'size' : dsram_remap_size})
            # move
            if dsram_data_size > 0:
                move_list.append({'subimg-offset': dram_flash_offset,
                        'run-address': dram_addr,
                        'size' :  dsram_data_size})
            # print(f"dsram-data: adddr: {hex(dsram_data_addr)}, size: {hex(dsram_data_size)}")
            # print(f"dsram-bss: size: {hex(dsram_bss_size)}")
            # print(f"dsram-stask: size: {hex(dsram_stack_size)}")
            # print(f"flash-offset: {hex(flash_offset)}")


            # ixip ------------------------------------------------
            ixip_remap_size = flash_offset
            if ixip_remap_size % 4096 > 0 :
                ixip_remap_size += (4096 - ixip_remap_size % 4096)
            remap_list.append({'type' : 'xip',
                                'offset' : 0,
                                'run-address' : flash_addr,
                                'size' : ixip_remap_size})

            bin_size = flash_offset

    subimg_list = []
    if bin_size > 32:
        print(f'bin size: {bin_size} ({bin_size / 1024} KB)')
        if os.path.exists(out_file_path):
            os.remove(out_file_path)

        subimg_list.append({'name' : sub_img_cfg['name'],
                            'cpu-id' : cpu_id,
                            'img-id' : img_id,
                            'version' : "0.0.0",
                            'start-run' : start_run,
                            'enter-point' : enter_point,
                            'move-address' : move_list,
                            'remap-address' : remap_list,
                            'in-file' : in_file,
                            'out-file' : out_file,
                            'store-dir' : store_dir,
                            'merge' : True,
                            'pack' : True})
    else :
        img_path = os.path.normpath(os.path.join(root_dir, 'libs', store_dir, out_file))
        if os.path.exists(out_file_path):
            os.remove(out_file_path)

        if os.path.exists(img_path):
            shutil.copy(img_path, out_file_path)
            subimg_list = Analysis_IMG.get_img_info(out_file_path)
            subimg_list[0]['pack'] = False
            subimg_list[0]['merge'] = True
            subimg_list[0]['out-file'] = out_file_path
        else :
            subimg_list.append({'merge': False, 'pack' : False})

    return subimg_list


def get_boot_img_info_from_elf(boot_img_cfg, elf_path, root_dir, build_dir, store_dir, section_conf):

    if (section_conf.get_size('boot_isram_text') > 0) and (section_conf.get_size('boot_ixip_text') > 0) :
        in_file =  boot_img_cfg['name'] + '_isram.bin'
        in_ixip_file = boot_img_cfg['name'] + '_ixip.bin'
        out_file = boot_img_cfg['out-file']
        in_sram_file_path = os.path.normpath(os.path.join(build_dir, in_file))
        if os.path.exists(in_sram_file_path):
            os.remove(in_sram_file_path)
        in_ixip_file_path = os.path.normpath(os.path.join(build_dir, in_ixip_file))
        if os.path.exists(in_ixip_file_path):
            os.remove(in_ixip_file_path)
        out_file_path = os.path.normpath(os.path.join(build_dir, store_dir, out_file))
        if os.path.exists(out_file_path):
            os.remove(out_file_path)

        with open(elf_path, 'rb') as f:
            elf = ELFFile(f)
            with open(in_sram_file_path, 'wb') as wf:
                # isram-text
                isram_text_size =  section_conf.get_size('boot_isram_text')
                isram_text_run_addr = section_conf.get_start_addr('boot_isram_text')
                boot_img_cfg['load-ram-address'] = isram_text_run_addr
                boot_img_cfg['enter-point'] = isram_text_run_addr
                if isram_text_size > 0:
                    section = elf.get_section_by_name("boot_isram_text")
                    data = section.data()
                    wf.write(data)
                else:
                    raise Exception("bootloader isram_text must > 0 !!!")
    
            with open(in_ixip_file_path, 'wb') as wf:
                # ixip-text
                ixip_text_size =  section_conf.get_size('boot_ixip_text')
                # ixip_text_run_addr = section_conf.get_start_addr('boot_ixip_text')
                if ixip_text_size > 0:
                    section = elf.get_section_by_name("boot_ixip_text")
                    data = section.data()
                    wf.write(data)

                # ixip-rodata
                ixip_rodata_size =  section_conf.get_size('boot_ixip_rodata')
                # ixip_rodata_run_addr = section_conf.get_start_addr('boot_ixip_rodata')
                if ixip_rodata_size > 0:
                    section = elf.get_section_by_name("boot_ixip_rodata")
                    data = section.data()
                    wf.write(data)

                # dsram-data
                dsram_data_size =  section_conf.get_size('boot_dsram_data')
                # dsram_data_run_addr = section_conf.get_start_addr('boot_dsram_data')
                if dsram_data_size > 0:
                    section = elf.get_section_by_name("boot_dsram_data")
                    data = section.data()
                    wf.write(data)

        boot_img_cfg['img-id'] = IMG_ID_BOOT
        boot_img_cfg['in-file'] = in_file
        boot_img_cfg['pack'] = True
        boot_img_cfg['merge'] = True

    else:
        current_img_path = os.path.normpath(os.path.join(build_dir, boot_img_cfg['out-file']))
        out_file_path = os.path.normpath(os.path.join(root_dir, "libs", store_dir, boot_img_cfg['out-file']))
        shutil.copy(out_file_path, current_img_path)
        boot_img_cfg['pack'] = False
        boot_img_cfg['merge'] = True


def get_sym_from_elf(sub_img_cfg, root_dir, build_dir, store_dir):

    list_file_name =  sub_img_cfg['name'] + '.list'
    sym_file_name = sub_img_cfg['name'] + '.sym'
    lib_path = os.path.normpath(os.path.join(root_dir, "libs", store_dir)) 
    list_file_path = os.path.normpath(os.path.join(lib_path, list_file_name)) 
    sym_file_path = os.path.normpath(os.path.join(lib_path, sym_file_name))

    sym_path = os.path.join(build_dir, '*.symbols')
    sym_file = glob.glob(sym_path, recursive=True)
    input_sym_file = sym_file[0]

    if not (os.path.exists(input_sym_file) and os.path.getsize(input_sym_file) > 0):
        raise Exception("symbols file not exists!!!")

    if not (os.path.exists(list_file_path) and os.path.getsize(list_file_path) > 0):
        raise Exception(f"list file not exists!!! {list_file_path}")

    if os.path.exists(sym_file_path) and os.path.getsize(sym_file_path) > 0:
        os.remove(sym_file_path)

    with open(input_sym_file, 'r') as frsym:
        func_sym_list = frsym.readlines()
        with open(list_file_path, 'r') as frlist:
            func_list = frlist.readlines()
            with open(sym_file_path, 'w') as fwsym:
                is_comment = 0
                is_sym_type = 0
                dst_sym_type_a = ''
                dst_sym_type_b = ''
                type_str = ''
                for func in func_list:
                    if func.find("/*") != -1:
                        is_comment = 1
                
                    if is_comment == 0:
                        type_start = func.find("[")
                        if type_start != -1:
                            type_end = func.find("]")
                            type_str = func[type_start+1:type_end]
                            if type_str.find('|') != -1:
                                dst_sym_type_a, dst_sym_type_b = type_str.split('|')
                            else:
                                dst_sym_type_a = type_str
                            is_sym_type = 1

                    if is_sym_type == 1:
                        is_sym_type = 0
                        write_str = "/* " + type_str + " */ \n"
                        fwsym.writelines(write_str)
                    elif is_comment == 0 and re.findall(r"\w+", func):
                        notfound = 1
                        for func_sym in func_sym_list:
                            addr, _, others = func_sym.partition(' ')
                            sym_type, _, sym_value = others.partition(' ')
                            if sym_value == func:
                                if (sym_type == dst_sym_type_a or sym_type == dst_sym_type_b):
                                    sym_value = sym_value.rstrip('\n')
                                    func_sym_dst = sym_value + ' = ' + '0x' + addr + ';' + "\n"
                                    fwsym.writelines(func_sym_dst)
                                    notfound = 0
                                    break
                                else:
                                    print('sym type err: ' + sym_type + '(' + type_str + '), ' + func)
                                    sys.exit(1)

                        if (notfound):
                            print('sym not found: ' + func)
                            #sys.exit(1)
                    else:
                        fwsym.writelines(func)

                    if func.find("*/") != -1:
                        is_comment = 0
                
                print('symbols generate done.')


def get_section_info(elf_path, section_list):
    with open(elf_path, 'rb') as f:
        elf = ELFFile(f)
        symtab = elf.get_section_by_name('.symtab')

        for item in section_list:
            start_addr_found = False
            end_addr_found = False
            for symbol in symtab.iter_symbols():
                if ('__s_' + item["name"]) == symbol.name:
                    item['start-addr'] = int(symbol['st_value'])
                    start_addr_found = True
                elif ('__e_' + item["name"]) == symbol.name:
                    item['end-addr'] = int(symbol['st_value'])
                    end_addr_found = True
                
                if end_addr_found and start_addr_found:
                    break
   
    return section_list


def get_config_info(elf_path, config_list):
    with open(elf_path, 'rb') as f:
        elf = ELFFile(f)
        section = elf.get_section_by_name(".info")
        symtab = elf.get_section_by_name('.symtab')
        data = section.data()

        for item in config_list:
            for symbol in symtab.iter_symbols():
                if item["name"] == symbol.name.upper():
                    start = int(symbol['st_value'])
                    size = int(symbol['st_size'])
                    value = data[start : start + size]
                    if len(value) == 1 or len(value) == 2: 
                        item["value"] = int.from_bytes(value, byteorder='little')
                    else:
                        item["value"] = value.decode('utf-8').rstrip('\x00')
                    break
 
    return config_list

def get_version_info(elf_path, config_list):
    with open(elf_path, 'rb') as f:
        elf = ELFFile(f)
        section = elf.get_section_by_name(".info")
        symtab = elf.get_section_by_name('.symtab')
        data = section.data()

        for item in config_list:
            for symbol in symtab.iter_symbols():
                if item["name"] == symbol.name.upper():
                    start = int(symbol['st_value'])
                    size = int(symbol['st_size'])
                    value = data[start : start + size]
                    item["value"] = int.from_bytes(value, byteorder='little')
                    break

    return config_list

class Build_Info():
    def __init__(self, cfg_micro_info_list = None):
        self.micro_list = cfg_micro_info_list.copy() if cfg_micro_info_list else []
    
    def get(self, Micro):
        for item in self.micro_list:
            if item["name"] == Micro:
                return item["value"]
        
        return None

    def getvalue(self, Micro):
        for item in self.micro_list:
            if item["name"] == Micro:
                return item["value"]

        return None
        
    def getboolean(self, Micro):
        for item in self.micro_list:
            if item["name"] == Micro:
                if item["value"] == 1:
                    return True
                else:
                    return False

        return False


class Section_Info():
    def __init__(self, cfg_info_list = None):
        self.micro_list = cfg_info_list.copy() if cfg_info_list else []

    def get_start_addr(self, Micro):
        for item in self.micro_list:
            if item["name"] == Micro:
                return item["start-addr"]

        return 0

    def get_end_addr(self, Micro):
        for item in self.micro_list:
            if item["name"] == Micro:
                return item["end-addr"]

        return 0

    def get_size(self, Micro):
        for item in self.micro_list:
            if item["name"] == Micro:
                return (item["end-addr"] - item["start-addr"])

        return 0


class Generate_YAML():

    def generate_img_cfg(img_base_cfg_yaml, root_dir, build_dir):

        # read img_cfg.yaml mould
        cfg_file = open(img_base_cfg_yaml, 'r', encoding="utf-8")
        cfg_data = cfg_file.read()
        cfg_file.close()
        img_cfg = yaml.safe_load(cfg_data)

        base_conf = img_cfg['base-conf']    

        # clear merge img remap-address and sub img 
        sub_img_list = copy.copy(img_cfg['sub-imgs'])
        img_cfg['sub-imgs'] = []
        sub_img_conf = img_cfg['sub-imgs']

        path = os.path.join(build_dir, '*.elf') 
        elf_file = glob.glob(path, recursive=True)

        get_config_info(elf_file[0], config_list)
        get_section_info(elf_file[0], section_list)

        build_conf = Build_Info(config_list)
        section_conf = Section_Info(section_list)


        # debug_info = img_cfg['z-dbg-info']
        # debug_info.append(section_list)


        if build_conf.getboolean('CONFIG_IMG_BOOT'):
            boot_img_cfg = img_cfg['boot-img']
            store_dir = build_conf.getvalue('CONFIG_IMG_BOOT_PATH_STR')
            if store_dir == "":
                store_dir = boot_img_cfg['store-dir']
            boot_name = build_conf.get('CONFIG_BOOT_NAME')
            if boot_name != "":
                boot_img_cfg['out-file'] = boot_name + ".img"
            get_boot_img_info_from_elf(boot_img_cfg, elf_file[0], root_dir, build_dir, store_dir, section_conf)
            if boot_img_cfg['pack'] :
                img_cfg['merge-img'] = []
                version_value = get_version_info_from_elf('CONFIG_IMG_BOOT', elf_file[0], root_dir)
                boot_img_cfg['version'] = version_value['v']
                tmp_boot_cfg = copy.copy(img_cfg['boot-img'])
                tmp_boot_cfg['name'] = boot_name
                get_sym_from_elf(tmp_boot_cfg, root_dir, build_dir, store_dir)
        else:
            boot_img_cfg = img_cfg['boot-img']
            boot_img_cfg['pack'] = False
            boot_img_cfg['merge'] = False

        if build_conf.getboolean('CONFIG_IMG_FLASH_LIST'):
            flash_list_img = img_cfg['flash-list-img']
            flash_list_img['pack'] = False
            flash_list_img['merge'] = True
            store_dir = build_conf.getvalue('CONFIG_IMG_FLASH_LIST_PATH_STR')
            if store_dir != "":
                flash_list_img['store-dir'] = store_dir
            boot_name = build_conf.get('CONFIG_FLASH_LIST_NAME')
            if boot_name != "":
                flash_list_img['out-file'] = boot_name + ".img"
            
            current_img_path = os.path.normpath(os.path.join(build_dir, flash_list_img['out-file']))
            out_file_path = os.path.normpath(os.path.join(root_dir, "libs", store_dir, flash_list_img['out-file']))
            shutil.copy(out_file_path, current_img_path)
            flash_list_img['pack'] = False
            flash_list_img['merge'] = True
 

        if build_conf.getboolean('CONFIG_IMG_CUSTOM_CFG'):
            print("CONFIG_IMG_CUSTOM_CFG")

        for sub_img in sub_img_list:
            sub_img_info = None
            if sub_img['img-id'] == IMG_ID_SYS0 and build_conf.getboolean("CONFIG_IMG_SYS0") == 1:
                store_dir = build_conf.get('CONFIG_IMG_SYS_PATH_STR')
                if store_dir == "":
                    store_dir = boot_img_cfg['store-dir']
                name = build_conf.get('CONFIG_SYS0_NAME')
                if name != "":
                    sub_img['name'] = name
                sub_img_info = get_img_info_from_elf(sub_img, elf_file[0], "CONFIG_IMG_SYS0", root_dir, build_dir, store_dir, section_conf)
                if sub_img_info != None:
                    if sub_img_info[0]['pack'] == True:
                        version_value = get_version_info_from_elf('CONFIG_IMG_SYS0', elf_file[0], root_dir)
                        sub_img_info[0]['version'] = version_value['v']
 
            elif sub_img['img-id'] == IMG_ID_SYS1 and build_conf.getboolean("CONFIG_IMG_SYS1") == 1:
                store_dir = build_conf.get('CONFIG_IMG_SYS_PATH_STR')
                if store_dir == "":
                    store_dir = boot_img_cfg['store-dir']
                name = build_conf.get('CONFIG_SYS1_NAME')
                if name != "":
                    sub_img['name'] = name
                sub_img_info = get_img_info_from_elf(sub_img, elf_file[0], "CONFIG_IMG_SYS1", root_dir, build_dir, store_dir, section_conf)
                if sub_img_info != None:
                    if sub_img_info[0]['pack'] == True:
                        version_value = get_version_info_from_elf('CONFIG_IMG_SYS1', elf_file[0], root_dir)
                        sub_img_info[0]['version'] = version_value['v']

            elif sub_img['img-id'] == IMG_ID_MMW and build_conf.getboolean("CONFIG_IMG_MMW") == 1:
                store_dir = build_conf.get('CONFIG_IMG_MMW_PATH_STR')
                if store_dir == "":
                    store_dir = boot_img_cfg['store-dir']
                name = build_conf.get('CONFIG_MMW_NAME')
                if name != "":
                    sub_img['name'] = name
                sub_img_info = get_mmw_img_info_from_elf(sub_img, elf_file[0], root_dir, build_dir, store_dir, section_conf)
                if sub_img_info != None:
                    if sub_img_info[0]['pack'] == True:
                        mmw_dir = os.path.normpath(os.path.join(root_dir, "..", "radar_firmware")) 
                        version_value = get_version_info_from_elf('CONFIG_IMG_MMW', elf_file[0], root_dir)
                        sub_img_info[0]['version'] = version_value['v']
                if build_conf.getboolean("CONFIG_IMG_MMW_DEVELOP") == 1:
                    get_sym_from_elf(sub_img, root_dir, build_dir, store_dir)
                else:
                    if sub_img_info[0]['pack'] == True:
                        sub_img_info[0]['pack'] = False

            elif sub_img['img-id'] == IMG_ID_MMW_ALG and build_conf.getboolean("CONFIG_IMG_MMW_ALG") == 1:
                store_dir = build_conf.get('CONFIG_IMG_MMW_ALG_PATH_STR')
                if store_dir == "":
                    store_dir = boot_img_cfg['store-dir']
                name = build_conf.get('CONFIG_MMW_ALG_NAME')
                if name != "":
                    sub_img['name'] = name
                sub_img_info = get_mmw_alg_img_info_from_elf(sub_img, elf_file[0], root_dir, build_dir, store_dir, section_conf)
                if sub_img_info != None:
                    if sub_img_info[0]['pack'] == True:
                        version_value = get_version_info_from_elf('CONFIG_IMG_MMW_ALG', elf_file[0], root_dir)
                        sub_img_info[0]['version'] = version_value['v']
                if build_conf.getboolean("CONFIG_IMG_MMW_ALG_DEVELOP") == 1:
                    get_sym_from_elf(sub_img, root_dir, build_dir, store_dir)
                else:
                    if sub_img_info[0]['pack'] == True:
                        sub_img_info[0]['pack'] = False

            elif sub_img['img-id'] == IMG_ID_MPW and build_conf.getboolean("CONFIG_IMG_MPW") == 1:
                store_dir = build_conf.get('CONFIG_IMG_MPW_PATH_STR')
                if store_dir == "":
                    store_dir = boot_img_cfg['store-dir']
                name = build_conf.get('CONFIG_MPW_NAME')
                if name != "":
                    sub_img['name'] = name
                sub_img_info = get_img_info_from_elf(sub_img, elf_file[0], "CONFIG_IMG_MPW", root_dir, build_dir, store_dir, section_conf)
                if sub_img_info != None:
                    if sub_img_info[0]['pack'] == True:
                        version_value = get_version_info_from_elf('CONFIG_IMG_MPW', elf_file[0], root_dir)
                        sub_img_info[0]['version'] = version_value['v']

            else:
                continue

            if sub_img_info is not None and len(sub_img_info) > 0:
                for value in sub_img_info:
                    sub_img_conf.append(value)

        if len(img_cfg['merge-img']) > 0:
            merge_img_conf = img_cfg['merge-img']
            del merge_img_conf['remap-address']
            merge_img_conf['img-id'] = 1
            merge_img_conf['cpu-id'] = 'cpusf'
            merge_img_conf['name'] = build_conf.get('CONFIG_BOARD_NAME')
            merge_img_conf['in-file'] = "merge.img"
            merge_img_conf['out-file'] = "merge.img"
            version_value = get_version_info_from_elf('CONFIG_IMG_MERGE', elf_file[0], root_dir)
            merge_img_conf['r_version'] = version_value['r']
            merge_img_conf['m_version'] = version_value['m']
            merge_img_conf['version'] = version_value['v']


        # generate yaml
        yaml.add_representer(int, represent_hex)
        update_yml = yaml.dump(img_cfg, default_flow_style=False, indent=4)
        with open(img_base_cfg_yaml, 'w') as file:
            file.write(update_yml)

