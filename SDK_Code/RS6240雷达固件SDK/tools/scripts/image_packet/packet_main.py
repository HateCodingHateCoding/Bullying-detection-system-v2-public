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
import copy
import re

from image.img_analysis import Analysis_IMG
from image.img_yaml_create import Generate_YAML

SIGN_DESCRIPTION = '''\
'''
SIGN_EPILOG = '''\
imgtool
-------
To build a signed binary you can load with MCUboot using imgtool.'''

ALIGN_SIZE = 0x1000
MCUBOOT_ALIGN_SIZE = 0x10000


# arrange sub img position in SRAM
def arrange_img(sub_img_list, base_conf):
    subimg_sram_final = []
    fix_sram_list = []
    unfix_sram_list = []

    img_list = copy.copy(sub_img_list)
    for index, subimg in enumerate(img_list):
        for remap in subimg['remap-address']:
            sub_remap = copy.copy(remap)
            if sub_remap['type'] == 'sram-ibus-cpus' or sub_remap['type'] == 'sram-ibus-cpuf' or sub_remap['type'] == 'sram-ibus-cpusf' or\
                sub_remap['type'] == 'sram-dbus-cpus' or sub_remap['type'] == 'sram-dbus-cpuf' or sub_remap['type'] == 'sram-dbus-cpusf':
                if sub_remap['offset'] != 0:
                    if sub_remap['type'] == 'sram-dbus-cpus' or sub_remap['type'] == 'sram-dbus-cpuf' or sub_remap['type'] == 'sram-dbus-cpusf':
                        sub_remap['offset'] = sub_remap['offset'] & 0x00FFFFFF
                    fix_sram_list.append({'index' : index,
                                        'type' : sub_remap['type'],
                                        'offset' : sub_remap['offset'],
                                        'size' : sub_remap['size']})
                else:
                    unfix_sram_list.append({'index' : index,
                                        'type' : sub_remap['type'],
                                        'offset' : sub_remap['offset'],
                                        'size' : sub_remap['size']})
    
    fix_sram_seq_list = sorted(fix_sram_list, key=lambda x: x["offset"], reverse=False)

    # unfix_sram_seq_list = sorted(unfix_sram_list, key=lambda x: x["size"], reverse=True)
    unfix_sram_seq_list = []
    for unfix_sram in unfix_sram_list:
        if unfix_sram['type'] == 'sram-ibus-cpus' or unfix_sram['type'] == 'sram-ibus-cpuf' or unfix_sram['type'] == 'sram-ibus-cpusf':
            unfix_sram_seq_list.append(unfix_sram)
    # for unfix_sram in unfix_sram_list:
    #     if unfix_sram['type'] == 'sram-dbus-cpus' or unfix_sram['type'] == 'sram-dbus-cpuf' or unfix_sram['type'] == 'sram-dbus-cpusf':
    #         unfix_sram_seq_list.append(unfix_sram)
    for unfix_sram in unfix_sram_list:
        if unfix_sram['type'] == 'sram-dbus-cpusf':
            unfix_sram_seq_list.append(unfix_sram)

    sram_base_addr = base_conf['sram-base-addr']
    sram_start_addr = sram_base_addr
    
    # get the img max size in sram 
    read_img_sram_max_size = re.findall(r'\d+', base_conf['sram-max-size'])
    img_sram_max_size = int(read_img_sram_max_size[0]) * 1024
    sram_max_addr = sram_base_addr + img_sram_max_size

    sram_final_seq_list = []
    if len(fix_sram_seq_list) > 0:
        for index, fix_sram in enumerate(fix_sram_seq_list):

            if ((fix_sram['offset'] < sram_max_addr) and (fix_sram['offset'] + fix_sram['size'] > sram_max_addr)):
                overflow_size = fix_sram['offset'] + fix_sram['size'] - sram_max_addr
                raise Exception(f"img sram overflow. {fix_sram['type']}, over size ({overflow_size}({hex(overflow_size)})) bytes. "
                    f"start addr: {hex(fix_sram['offset'])}, size: {hex(fix_sram['size'])}.")

            inset_size = int(fix_sram['offset']) - sram_start_addr
            if inset_size >= 0:
                while inset_size > 0:
                    insert_img = False
                    for idx in range(len(unfix_sram_seq_list)):
                        if inset_size > unfix_sram_seq_list[idx]['size']:
                            unfix_sram_seq_list[idx]['offset'] = sram_start_addr
                            sram_start_addr += unfix_sram_seq_list[idx]['size']
                            inset_size -= unfix_sram_seq_list[idx]['size']
                            sram_final_seq_list.append(unfix_sram_seq_list[idx])
                            unfix_sram_seq_list.pop(idx)
                            insert_img = True
                            break
                    if insert_img == False:
                        break
            else:
                raise Exception(f"memory overlap. {index}, {fix_sram_seq_list[index-1]['type']}, start addr: {hex(fix_sram_seq_list[index-1]['offset'])}, end addr: {hex(sram_start_addr)}, "
                    f"next: {fix_sram['type']}, start addr: {hex(fix_sram['offset'])}.")
            
            align_128byte = int(fix_sram['size']) % 128
            if align_128byte != 0:
                align_128byte = 128 - align_128byte
            sram_start_addr = int(fix_sram['offset']) + int(fix_sram['size']) + align_128byte
            # Since then 'size' has been used to indicate whether remap is required
            if (base_conf['fix-sram-map'] == False):
                fix_sram['size'] = 0
            sram_final_seq_list.append(fix_sram)
        
    # arrange the img of SRAM mapping
    for unfix_sram in unfix_sram_seq_list:
        unfix_sram['offset'] = sram_start_addr
        sram_start_addr = sram_start_addr + unfix_sram['size']
        sram_final_seq_list.append(unfix_sram)

    for unfix_sram in unfix_sram_list:
        if unfix_sram['type'] == 'sram-dbus-cpus':
            unfix_sram['offset'] = sram_start_addr
            sram_start_addr = sram_start_addr + unfix_sram['size']
            sram_final_seq_list.append(unfix_sram)

    for unfix_sram in unfix_sram_list:
        if unfix_sram['type'] == 'sram-dbus-cpuf':
            unfix_sram['offset'] = sram_start_addr
            sram_start_addr = sram_start_addr + unfix_sram['size']
            sram_final_seq_list.append(unfix_sram)


    # write back SRAM offset
    sram_last_addr = 0
    for sram_final in sram_final_seq_list:
        index = sram_final['index']
        img_remap = sub_img_list[index]['remap-address']
        for idx in range(len(img_remap)):
            if img_remap[idx]['type'] == sram_final['type']:
                img_remap[idx]['offset'] = sram_final['offset']
                img_remap[idx]['size'] = sram_final['size']
                sram_last_addr += sram_final['size']

                # if ((sram_final['offset'] < sram_max_addr) and (sram_final['offset'] + sram_final['size'] > sram_max_addr)):
                #     overflow_size = sram_final['offset'] + sram_final['size'] - sram_max_addr
                #     raise Exception(f"img overflow sram ({overflow_size}({hex(overflow_size)})) bytes")
                break


    # for subimg in img_list:
    #     print(f"{subimg['name']} : {subimg['version']}")

    # sram_total_size_str = base_conf['sram-max-size']
    # sram_total_size_str = sram_total_size_str.replace('k', '')
    # sram_total_size = int(sram_total_size_str) * 1024
    # # sram_last_addr_k = sram_last_addr / 1024
    # # sram_used_percent = (sram_last_addr / sram_total_size) * 100
    # sram_used = {}
    # sram_used['used'] = sram_last_addr
    # sram_used['total'] = sram_total_size
    # sram_used['percent'] = sram_used_percent
    # print(f"sram used size: {sram_last_addr_k:.2f}k ({sram_used:.2f}%). total size: {sram_total_size_str}k")
    
    return sram_last_addr


# arrange sub img position in flash
def arrange_img_flash(sub_img_list, base_conf):
    fix_sub_img_cfg_list = []
    unfix_sub_img_cfg_list = []
    img_list = copy.copy(sub_img_list)
    for index, subimg in enumerate(img_list):
        for remap in subimg['remap-address']:
            sub_remap = copy.copy(remap)
            if sub_remap['type'] == 'xip':
                img_file_size = os.path.getsize(subimg['out-file'])
                if img_file_size % ALIGN_SIZE != 0:
                    img_file_size = img_file_size + (ALIGN_SIZE - img_file_size % ALIGN_SIZE)
                sub_remap['size'] = img_file_size
                if sub_remap['offset'] != 0:
                    fix_sub_img_cfg_list.append({'index' : index,
                                        'offset' : sub_remap['offset'],
                                        'size' : sub_remap['size']})
                else:
                    unfix_sub_img_cfg_list.append({'index' : index,
                                        'offset' : sub_remap['offset'],
                                        'size' : sub_remap['size']})
    
    subimg_fix_seq_list = sorted(fix_sub_img_cfg_list, key=lambda x: x["offset"], reverse=False)
    subimg_unfix_seq_list = sorted(unfix_sub_img_cfg_list, key=lambda x: x["size"], reverse=True)

    start_addr = ALIGN_SIZE 

    subimg_final_seq_list = []
    if len(subimg_fix_seq_list) > 0:
        for index, fix_flash in enumerate(subimg_fix_seq_list):
            inset_size = int(fix_flash['offset']) - start_addr
            if inset_size >= 0:
                while inset_size > 0:
                    insert_img = False
                    for idx in range(len(subimg_unfix_seq_list)):
                        if inset_size > subimg_unfix_seq_list[idx]['size']:
                            subimg_unfix_seq_list[idx]['offset'] = start_addr
                            start_addr += subimg_unfix_seq_list[idx]['size']
                            inset_size -= subimg_unfix_seq_list[idx]['size']
                            subimg_final_seq_list.append(subimg_unfix_seq_list[idx])
                            subimg_unfix_seq_list.pop(idx)
                            insert_img = True
                            break
                    if insert_img == False:
                        break
            else:
                raise Exception(f"Flash offset error {img_list[fix_flash['index']]['name']}")
            
            align_4kbyte = int(fix_flash['size']) % ALIGN_SIZE
            if align_4kbyte != 0:
                align_4kbyte = ALIGN_SIZE - align_4kbyte
            start_addr = int(fix_flash['offset']) + int(fix_flash['size']) + align_4kbyte
            fix_flash['size'] = 0
            subimg_final_seq_list.append(fix_flash)
    
    # arrange the img of FLASH mapping
    for unfix_flash in subimg_unfix_seq_list:
        unfix_flash['offset'] = start_addr
        start_addr = start_addr + unfix_flash['size']
        subimg_final_seq_list.append(unfix_flash)

    # write back flash offset and gen a final sub img list
    subimg_final_list = []
    for sub_final in subimg_final_seq_list:
        index = sub_final['index']
        img_remap = sub_img_list[index]['remap-address']
        for idx in range(len(img_remap)):
            if img_remap[idx]['type'] == 'xip':
                img_remap[idx]['offset'] = sub_final['offset']
                img_remap[idx]['size'] = sub_final['size']
                break
        subimg_final_list.append(sub_img_list[index])

    return subimg_final_list

def packet_mergeimg(signer, tool_path, build_dir, base_conf, merge_cfg, sub_img_list, formats):
    # create mergeimg.bin file
    mergeimg_file = os.path.dirname(sub_img_list[0]['out-file']) / pathlib.Path(merge_cfg['in-file']) 

    merge_cfg['subimg-load'] = []
    subimg_load_list = merge_cfg['subimg-load']

    subimg_list = []
    # arrange sub img position in sram
    img_list = copy.copy(sub_img_list)
    sram_used = arrange_img(img_list, base_conf)
    subimg_list = arrange_img_flash(img_list, base_conf)

    merge_img_remap_list = []

    # copy the subimg content to mergeimg.bin
    with open(mergeimg_file, 'wb') as wf:
        for index, sub_img in enumerate(subimg_list):
            with open(sub_img['out-file'], 'rb') as rf:
                content = rf.read()
                wf.write(content)

                subimg_load_offset = 0
                for remap in sub_img['remap-address']:
                    sub_remap = copy.copy(remap)
                    if sub_remap['type'] == 'xip':
                        subimg_load_offset = sub_remap['offset']
                        subimg_load_list.append({'offset' : subimg_load_offset})
                    
                    if sub_remap['size'] != 0:
                        merge_img_remap_list.append(sub_remap)

                next_subimg_load_offset = 0
                if index != len(subimg_list) - 1:
                    for next_remap in subimg_list[index + 1]['remap-address']:
                        if next_remap['type'] == 'xip':
                            next_subimg_load_offset = next_remap['offset']

                if (next_subimg_load_offset > 0) and (next_subimg_load_offset > (subimg_load_offset + len(content))):
                    pad_len = next_subimg_load_offset - (subimg_load_offset + len(content))
                    padding = bytearray([0xFF] * pad_len)
                    wf.write(padding)

    if len(merge_img_remap_list) > 0:
        if 'remap-address' not in merge_cfg:
            merge_cfg['remap-address'] = []

        psram_offset = 0
        for value in merge_img_remap_list:
            if value['type'] == 'psram-ibus' or value['type'] == 'psram-dbus':
                value['offset'] = psram_offset
                psram_offset += value['size']
            merge_cfg['remap-address'].append(value)

    # The size of merge img header is 4K,include header size、protect size、nomal size and padding size.
    base_conf['head-size'] = ALIGN_SIZE
    signer.sign(tool_path, build_dir, base_conf, merge_cfg, formats, "merge_img")
    # restore header size
    base_conf['head-size'] = 32
    return sram_used

# The numerical values in the yaml file are displayed in hex
def represent_hex(dumper, data):
        return dumper.represent_scalar('tag:yaml.org,2002:int', hex(data))

# Copy img to storage location
# def copy_img_to_storage_location(base_path, build_info, subimg):
#     if subimg['img-id'] == 5:
#         img_base_path = build_info.get('CONFIG_MMW_IMG_PATH')
#     elif subimg['img-id'] == 8:
#         img_base_path = build_info.get('CONFIG_MPW_IMG_PATH')
#     elif subimg['img-id'] == 2:
#         img_base_path = build_info.get('CONFIG_SYS0_IMG_PATH')
#     elif subimg['img-id'] == 3:
#         img_base_path = build_info.get('CONFIG_SYS1_IMG_PATH')
#     else:
#         return

#     img_storage_location_path = os.path.normpath(os.path.join(base_path, img_base_path, subimg['name'] + '.img'))
#     shutil.copy(subimg['out-file'], img_storage_location_path)

class Build_Info():
    def __init__(self, cfg_micro_info_list = None):
        self.micro_list = cfg_micro_info_list.copy() if cfg_micro_info_list else []
    
    def get(self, Micro):
        for item in self.micro_list:
            if item["name"] == Micro:
                return "libs\\" + item["value"]
        
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

class Packet():
    def do_run(root_dir, build_dir, img_cfg_template):    
        output_dir = os.path.normpath(os.path.join(build_dir, '../output'))
        if os.path.exists(output_dir):
            for item in os.listdir(output_dir):
                item_path = os.path.join(output_dir, item)
                if os.path.isfile(item_path):
                    os.remove(item_path)
        else :
            os.makedirs(output_dir)

        img_cfg_file = os.path.normpath(os.path.join(build_dir, '../output\\img_cfg.yml'))
        shutil.copy(img_cfg_template, img_cfg_file)

        Generate_YAML.generate_img_cfg(img_cfg_file, root_dir, build_dir)

        cfg_file = open(img_cfg_file, 'r', encoding="utf-8")
        cfg_data = cfg_file.read()
        cfg_file.close()
        img_cfg = yaml.safe_load(cfg_data)
        base_conf = img_cfg['base-conf']

        tool_path =  os.path.normpath(os.path.join(root_dir, 'tools\\scripts\\image_packet\\image\\imgtool.py'))
        signer = ImgtoolSigner()
        formats = []
        formats.append('bin')


        # packet subimage
        sub_img_list = []
        sub_img_cfg = img_cfg['sub-imgs']
        if len(sub_img_cfg) > 0:
            for img in sub_img_cfg:
                subimg = copy.copy(img)
                if subimg['pack']:
                    img_path = os.path.join(root_dir, "libs", subimg['store-dir'], subimg['out-file'])
                    subimg['in-file'] =  build_dir / pathlib.Path(subimg['in-file'])
                    subimg['out-file'] =  build_dir / pathlib.Path(subimg['out-file'])
                    if not subimg['out-file'].exists():
                        signer.sign(tool_path, build_dir, base_conf, subimg, formats)
                        shutil.copy(subimg['out-file'], img_path)
                        if subimg['merge']:
                            sub_img_list.append(subimg)
                    else:
                        print(f"packet subimage {subimg['name']} error")
                elif subimg['merge']:
                    sub_img_list.append(subimg)
                    # img_path = os.path.join(root_dir, "libs", subimg['store-dir'], subimg['out-file'])
                    # input_img_path = pathlib.Path(img_path)
                    # subimg['out-file'] =  build_dir / pathlib.Path(subimg['out-file'])
                    # if input_img_path.exists():
                    #     shutil.copy(img_path, subimg['out-file'])
                    #     sub_img_list.append(subimg)

        # packet merge img
        sram_used = 0
        if len(sub_img_list) > 0:
            merge_img_cfg = copy.copy(img_cfg['merge-img'])

            merge_img_cfg['in-file'] = build_dir / pathlib.Path(merge_img_cfg['in-file'])
            merge_img_cfg['out-file'] = build_dir / pathlib.Path(merge_img_cfg['out-file'])
            merge_sram_used = packet_mergeimg(signer, tool_path, build_dir, base_conf, merge_img_cfg, sub_img_list, formats)
            sram_used = merge_sram_used
            if 'remap-address' in merge_img_cfg:
                img_cfg['merge-img']['remap-address'] = merge_img_cfg['remap-address']
                img_cfg['merge-img']['subimg-load'] = merge_img_cfg['subimg-load'] 
 

        # brom packet
        if img_cfg['boot-img']['pack']:
            boot_img_cfg = copy.copy(img_cfg['boot-img'])
            boot_img_store_dir = os.path.join(root_dir, "libs", boot_img_cfg['store-dir'])
            img_path = os.path.normpath(os.path.join(boot_img_store_dir, boot_img_cfg['out-file']))
            boot_img_cfg['in-file'] =  build_dir / pathlib.Path(boot_img_cfg['in-file'])
            boot_img_cfg['out-file'] =  output_dir / pathlib.Path(boot_img_cfg['out-file'])
            signer.sign(tool_path, build_dir, base_conf, boot_img_cfg, formats, "boot_img")
            with open(boot_img_cfg['out-file'], 'ab') as bwf:
                boot_ixip_path = os.path.normpath(os.path.join(build_dir, boot_img_cfg['name'] + '_ixip.bin'))
                with open(boot_ixip_path, 'rb') as brf:
                    # The front part of the ixip file is filled with iram's data 0xFF
                    content = brf.read()
                    bwf.write(content)
                        # Copy mcuboot.img to the libs storage location
            shutil.copy(boot_img_cfg['out-file'], img_path)

            img_size_value = os.path.getsize(boot_img_cfg["out-file"])
            img_max_size = base_conf['bootloader-section-size']
            used_percent = (img_size_value / img_max_size) * 100
            print(f'\n{os.path.basename(boot_img_cfg["out-file"])}')
            print(f'version : {boot_img_cfg["version"]}')
            print(f'size : {img_size_value} ({img_size_value / 1024:.2f} KB / {img_max_size / 1024:.2f} KB) {used_percent:.2f}%')
            print('\n')

        # brom + merge
        if (len(img_cfg['merge-img']) > 0) and img_cfg['boot-img']['merge']:
            output_img_file = build_dir / pathlib.Path("../output", img_cfg['merge-img']['name'] + '.img') 
            with open(output_img_file, 'wb') as wf:
                # write boot img
                boot_img_path = build_dir / pathlib.Path(img_cfg['boot-img']['out-file'])
                with open(boot_img_path, 'rb') as rf:
                    content = rf.read()
                    wf.write(content)
                    # for the minboot img 32k alignment, pad 0xFF
                    pad_len = (base_conf['bootloader-section-size'] - len(content)) % len(content)
                    if pad_len > 0:
                        padding = bytearray([0xFF] * pad_len)
                        wf.write(padding) 

                # write merge img
                with open(merge_img_cfg['out-file'], 'rb') as rf:
                    content = rf.read()
                    wf.write(content)
                # 4 byte alignment, pad 0xFF
                    data_len = len(content)
                    pad_len = data_len % 4
                    if pad_len > 0:
                        padding = bytearray([0xFF] * pad_len)
                        wf.write(padding) 

                # flash-list img
                if img_cfg['flash-list-img']['merge']:
                    flash_list_img_path =  build_dir / pathlib.Path(img_cfg['flash-list-img']['out-file'])
                    with open(flash_list_img_path, 'rb') as rf:
                        content = rf.read()
                        wf.write(content)
                        data_len = len(content)
                        pad_len = data_len % 4
                        if pad_len > 0:
                            pad_len = 4 - pad_len
                            padding = bytearray([0xFF] * pad_len)
                            wf.write(padding)

                # mmw customer-cfg img
                if img_cfg['mmw-customer-cfg-img']['merge']:
                    mmw_customer_img_path =  build_dir / pathlib.Path(img_cfg['mmw-customer-cfg-img']['out-file'])
                    with open(mmw_customer_img_path, 'rb') as rf:
                        content = rf.read()
                        wf.write(content)
                        data_len = len(content)
                        pad_len = data_len % 4
                        if pad_len > 0:
                            pad_len = 4 - pad_len
                            padding = bytearray([0xFF] * pad_len)
                            wf.write(padding)

            img_size_value = os.path.getsize(output_img_file)
            img_max_size = base_conf['app-section-max-size']
            if img_size_value > img_max_size:
                raise Exception(f"image size overflow : img: {img_size_value / 1024: .2f}KB, "  +
                                f"max: {img_max_size / 1024: .2f}KB, " + 
                                f"over: {img_size_value - img_max_size} Bytes {(img_size_value - img_max_size)/1024: .2f}KB")
            sram_total_size_str = base_conf['sram-max-size']
            sram_total_size_str = sram_total_size_str.replace('k', '')
            sram_total_size = int(sram_total_size_str) * 1024
            sram_last_addr_k = sram_used / 1024
            sram_used_percent = (sram_used / sram_total_size) * 100
            if sram_used > sram_total_size:
                raise Exception(f"sram size overflow : used: {sram_used / 1024: .2f}KB, "  +
                                f"max: {sram_total_size / 1024: .2f}KB, " + 
                                f"over: {sram_used - sram_total_size} Bytes {(sram_used - sram_total_size)/1024: .2f}KB")
            print(f'\noutput image : {os.path.basename(output_img_file)}')
            print(f'version : R{img_cfg['merge-img']["r_version"]}-M{img_cfg['merge-img']["m_version"]}-V{img_cfg['merge-img']["version"]}')
            print(f'image size (flash): {img_size_value} ({img_size_value / 1024:.2f} KB)')
            print(f"sram size: {sram_last_addr_k:.2f}k ({sram_used_percent:.2f}%). total size: {sram_total_size_str}k")
            print('\n')

        yaml.add_representer(int, represent_hex)
        update_yml = yaml.dump(img_cfg, default_flow_style=False, indent=4)
        with open(img_cfg_file, 'w') as file:
            file.write(update_yml)


        return


class Signer(abc.ABC):
    '''Common abstract superclass for signers.

    To add support for a new tool, subclass this and add support for
    it in the Sign.do_run() method.'''

    @abc.abstractmethod
    def sign(self, command, tool_path, build_dir, base_conf, conf, formats, img_type):
        '''Abstract method to perform a signature; subclasses must implement.

        :param command: the Sign instance
        :param tool_path: the Sign tool path
        :param build_dir: the build directory
        :param base_conf: the base config
        :param conf: sub image config
        :param img_type: image type, sub_img, merge_img, boot_img
        :param formats: list of formats to generate ('bin', 'hex')
        '''


class ImgtoolSigner(Signer):

    def sign(self, tool_path, build_dir, base_conf, conf, formats, img_type="sub_img"):
        if not formats:
            return

        imgtool = self.find_imgtool(str(tool_path))

        if 'bin' in formats:
            in_bin =conf['in-file']
            if not in_bin.is_file():
                raise Exception(f"no unsigned .bin found at {in_bin}")
            out_bin =conf['out-file']
        else:
            in_bin = None

        # Base sign command.
        sign_base = imgtool + ['sign',
                               '--align', str(4)]

        # base config ---------
        # head size
        if img_type == 'boot_img':
            base_conf['head-size'] = 64
        if base_conf['head-size']:
            sign_base.extend(['--header-size', str(base_conf['head-size']), '--pad-header'])

        # sub image config ---------
        # version
        if conf['version']:
            sign_base.extend(['--version', str(conf['version'])])

        if img_type == 'merge_img':
            if conf['r_version']:
                sign_base.extend(['--rversion', str(conf['r_version'])])

            if conf['m_version']:
                sign_base.extend(['--mversion', str(conf['m_version'])])

        if conf['name']:
            sign_base.extend(['--name', str(conf['name'])])

        # merge img is no move address, no start-run, no enter point
        if img_type == 'sub_img':
            # cpu-id
            if conf['cpu-id'] == 'cpus':
                sign_base.extend(['--cpu-id', '0x1'])
            elif conf['cpu-id'] == 'cpuf':
                sign_base.extend(['--cpu-id', '0x2'])
            elif conf['cpu-id'] == 'cpusf' or conf['cpu-id'] == 'cpufs':
                sign_base.extend(['--cpu-id', '0x3'])
            else:
                raise Exception(f"cpu-id error ({conf['cpu-id']}), must is 'cpus', 'cpuf', 'cpusf' or 'cpufs'")

            # img-id
            if conf['img-id'] in range(0, 15):
                sign_base.extend(['--img-id', hex(conf['img-id'])])
            else:
                raise Exception(f"img-id error ({conf['img-id']}), must is from '0' to '15'")

            # enter point
            if conf['enter-point']:
                sign_base.extend(['--enter-point', hex(conf['enter-point'])])

            # start run
            if conf['start-run']:
                sign_base.extend(['--start-run'])

            # move address
            if conf['move-address']:
                for ma_cfg in conf['move-address']:
                    sign_base.extend(['--move-addr', hex(ma_cfg['subimg-offset']), hex(ma_cfg['run-address']), hex(ma_cfg['size'])])
        
        if img_type == 'merge_img':
            # cpu-id
            if conf['cpu-id'] == 'cpus':
                sign_base.extend(['--cpu-id', '0x1'])
            elif conf['cpu-id'] == 'cpuf':
                sign_base.extend(['--cpu-id', '0x2'])
            elif conf['cpu-id'] == 'cpusf' or conf['cpu-id'] == 'cpufs':
                sign_base.extend(['--cpu-id', '0x3'])
            else:
                raise Exception(f"cpu-id error ({conf['cpu-id']}), must is 'cpus', 'cpuf', 'cpusf' or 'cpufs'")

            # img-id
            if conf['img-id'] in range(0, 15):
                sign_base.extend(['--img-id', hex(conf['img-id'])])
            else:
                raise Exception(f"img-id error ({conf['img-id']}), must is from '0' to '15'")

            # Enable mergeed_img
            sign_base.extend(['--merged-img'])
            # subimg order auto
            # if not conf['subimg-order-auto']:
            #     sign_base.extend(['--subimg-order-auto'])
            # sub img in merge img offset and size
            for m_cfg in conf['subimg-load']: 
                sign_base.extend(['--subimg-load', hex(m_cfg['offset'])])

        if img_type == 'data_img':
            # cpu-id
            if conf['cpu-id'] == 'cpus':
                sign_base.extend(['--cpu-id', '0x1'])
            elif conf['cpu-id'] == 'cpuf':
                sign_base.extend(['--cpu-id', '0x2'])
            elif conf['cpu-id'] == 'cpusf' or conf['cpu-id'] == 'cpufs':
                sign_base.extend(['--cpu-id', '0x3'])
            else:
                raise Exception(f"cpu-id error ({conf['cpu-id']}), must is 'cpus', 'cpuf', 'cpusf' or 'cpufs'")

            # img-id
            if conf['img-id'] in range(0, 15):
                sign_base.extend(['--img-id', hex(conf['img-id'])])
            else:
                raise Exception(f"img-id error ({conf['img-id']}), must is from '0' to '15'")


        # remap address
        if img_type != 'boot_img' and 'remap-address' in conf:
            for ra_cfg in conf['remap-address']:
                remap_type = 0x0
                if ra_cfg['type'] == 'sram-ibus-cpus':
                    remap_type = 0x11
                elif ra_cfg['type'] == 'sram-ibus-cpuf':
                    remap_type = 0x12
                elif ra_cfg['type'] == 'sram-ibus-cpusf':
                    remap_type = 0x13
                elif ra_cfg['type'] == 'sram-dbus-cpus':
                    remap_type = 0x21
                elif ra_cfg['type'] == 'sram-dbus-cpuf':
                    remap_type = 0x22
                elif ra_cfg['type'] == 'sram-dbus-cpusf':
                    remap_type = 0x23
                elif ra_cfg['type'] == 'xip':
                    remap_type = 0x40
                elif ra_cfg['type'] == 'psram-ibus':
                    remap_type = 0x81
                elif ra_cfg['type'] == 'psram-dbus':
                    remap_type = 0x82
                else:
                    raise Exception(f'remap type error:   {ra_cfg["type"]}')

                sign_base.extend(['--remap-addr', hex(remap_type),
                                                hex(ra_cfg['offset']),
                                                hex(ra_cfg['run-address']),
                                                hex(ra_cfg['size'])])
  
        if img_type == 'boot_img':
            if conf['load-ram-address']:
                sign_base.extend(['--move-addr', hex(conf['load-ram-address']), hex(conf['load-ram-address']), hex(conf['load-ram-address'])])

            if conf['enter-point']:
                sign_base.extend(['--enter-point', hex(conf['enter-point'])])

            sign_base.extend(['--bootloader-section-size', hex(base_conf['bootloader-section-size'])])
            sign_base.extend(['--data-storage-size', hex(base_conf['data-storage-size'])])
            sign_base.extend(['--sram-base-addr', hex(base_conf['sram-base-addr'])])

            read_sram_max_size = re.findall(r'\d+', base_conf['sram-max-size'])
            sram_max_size = int(read_sram_max_size[0]) * 1024
            sign_base.extend(['--sram-max-size', hex(sram_max_size)])

            read_flash_max_size = re.findall(r'\d+', base_conf['flash-max-size'])
            flash_max_size = int(read_flash_max_size[0]) * 1024
            sign_base.extend(['--flash-max-size', hex(flash_max_size)])
            sign_base.extend(['--format', 'brom'])
        else :
            sign_base.extend(['--format', 'mcuboot'])

        if in_bin:
            sign_bin = sign_base + [in_bin, out_bin]
            subprocess.check_call(sign_bin)

    @staticmethod
    def find_imgtool(tool_path):
        if tool_path:
            imgtool = tool_path
            if not os.path.isfile(imgtool):
                raise Exception(f'tool-path {imgtool}: no such file')
        else:
            imgtool = shutil.which('imgtool') or shutil.which('imgtool.py')
            if not imgtool:
                raise Exception('imgtool not found; either install it',
                        '(e.g. "pip3 install imgtool") or provide --tool-path')

        if platform.system() == 'Windows' and imgtool.endswith('.py'):
            # Windows users may not be able to run .py files
            # as executables in subprocesses, regardless of
            # what the mode says. Always run imgtool as
            # 'python path/to/imgtool.py' instead of
            # 'path/to/imgtool.py' in these cases.
            return [sys.executable, imgtool]

        return [imgtool]

