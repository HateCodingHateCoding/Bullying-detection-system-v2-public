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

MIGC = 0x43495350 
TLV_INFO_SIZE = 4

CPU_ID_MASK  = 0x3    # 0-1 bit
IMG_ID_MASK  = 0xF0   # 4 - 7 bit
MA_MASK      = 0x100  # 8bit
SR_MASK      = 0x200  # 9bit start-run
EPT_MASK     = 0x400  # ENTER_POINT
ENC_MASK     = 0x800  #PAYLOAD_ENCRYPT
RA_MASK      = 0x1000 #REMAP_ADDR
NAME_MASK    = 0x2000 #Img name
ORDER_MASK   = 0x4000 #SUBIMG_ORDER_AUTO
MI_MASK      = 0x8000 #MERGED_IMAGE

CPU_ID_POS   = 0
IMG_ID_POS   = 4
MA__POS      = 8
SR_POS       = 9
EPT_POS      = 10
ENC_POS      = 11
RA_POS       = 12
NAME_POS     = 13
ORDER_POS    = 14
MI_POS       = 15

STRUCT_ENDIAN_DICT = {
        'little': '<',
        'big':    '>'
}

def convert_array_to_int(array):
        result = 0
        for i in range(len(array)):
            result |= array[i] << (8 * i)
            
        return result

class Analysis_IMG():
    def get_img_info(img_file):
        try:
            with open(img_file, "rb") as f:
                b = f.read()
        except FileNotFoundError:
            raise Exception("Image file not found ({})".format(img_file))

        header = struct.unpack('IIHHIIBBHI', b[:28]) 
        if header[0] != MIGC :
            return None
        else: 
            header_size = header[2]
            prot_size = header[3]
            payload_len = header[4]
            attr = header[5]
            
            version = str(header[6]) + "." + str(header[7]) + "." + str(header[8]) + "+" + str(header[9])
            img_id = (attr & IMG_ID_MASK) >> IMG_ID_POS
            if (attr & CPU_ID_MASK) == 1:
                cpu_id = "cpus"
            elif (attr & CPU_ID_MASK) == 2:
                cpu_id = "cpuf"
            elif (attr & CPU_ID_MASK) == 3:
                cpu_id = "cpusf"
            else :
                raise Exception(f'Cpu id error, current is: {hex(attr)}, must is 0x01(cpus)/0x02(cpuf)/0x03(cpusf)')

            if (attr & SR_MASK) >> SR_POS:
                start_run = True
            else:
                start_run = False

            if header_size > 32:# merge img
                prot_off = 32
            else: # subimg 
                prot_off = header_size + payload_len

            prot_end = prot_off + prot_size
            prot_off += TLV_INFO_SIZE

            remap_list = []
            move_addr_list= []

            while prot_off < prot_end:
                tlv_type, tlv_len = struct.unpack('HH',
                                    b[prot_off:(prot_off + TLV_INFO_SIZE)])
                prot_off += TLV_INFO_SIZE
                tlv_data = b[prot_off:(prot_off + tlv_len)]

                if tlv_type == 0xA1: # name
                    name = str(tlv_data )#[item.decode('utf-8') for item in tlv_data] #
                elif tlv_type == 0xD1: # enter point
                    enter_point = convert_array_to_int(tlv_data[:4])
                elif tlv_type == 0xD2: # move_addr
                    subimg_offset = convert_array_to_int(tlv_data[:4]) 
                    subimg_run = convert_array_to_int(tlv_data[4:8])
                    subimg_size = convert_array_to_int(tlv_data[-4:])
                    move_addr_list.append({'subimg-offset' : subimg_offset,
                                            'run-address' : subimg_run,
                                            'size' : subimg_size})
                elif tlv_type == 0xB1: # remap_add
                    if tlv_data[0] == 0x11 : # 'cpus-ibus'
                        remap_type = 'sram-ibus-cpus'
                    elif tlv_data[0] == 0x12: # 'cpus-ibus'
                        remap_type = 'sram-ibus-cpuf'
                    elif tlv_data[0] == 0x13: # 'cpusf-ibus'
                        remap_type = 'sram-ibus-cpusf'
                    elif tlv_data[0] == 0x21: # 'cpus-dbus'
                        remap_type = 'sram-dbus-cpus'
                    elif tlv_data[0] == 0x22: # 'cpus-dbus'
                        remap_type = 'sram-dbus-cpuf'
                    elif tlv_data[0] == 0x23: # 'cpus-dbus'
                        remap_type = 'sram-dbus-cpusf'
                    elif tlv_data[0] == 0x40: # xip
                        remap_type = 'xip'
                    elif tlv_data[0] == 0x81: # psram-ibus
                        remap_type = 'psram-ibus'
                    elif tlv_data[0] == 0x82: # psram-dbus
                        remap_type = 'psram-dbus'
                    else :
                        raise Exception(f'Remap type error, current is: {hex(tlv_data[0])}')

                    remap_offset = convert_array_to_int(tlv_data[4:8])
                    remap_run_addr = convert_array_to_int(tlv_data[8:12])
                    remap_size = convert_array_to_int(tlv_data[12:])
                    remap_list.append({'type' : remap_type,
                                        'offset' : remap_offset,
                                        'run-address' : remap_run_addr,
                                        'size' : remap_size})
                        
                prot_off += tlv_len

            if (attr & EPT_MASK) >> EPT_POS: 
                EP = enter_point
            else:
                EP = 0
            
            if (attr & NAME_MASK) >> NAME_POS: 
                subimg_name = name
            else:
                subimg_name = "none"

            subimg_list = []
            subimg_list.append({'name' : subimg_name,
                                'cpu-id' : cpu_id,
                                'img-id' : img_id,
                                'version' : version,
                                'start-run' : start_run,
                                'enter-point' : EP,
                                'move-address' : move_addr_list,
                                'remap-address' : remap_list,
                                'in-file' : "none",
                                'out-file' : "none",
                                'pack' : True,
                                'merge' : True,})

            return subimg_list


    def flash_list_import(input_img, output_yaml):
        flash_list = []

        try:
            with open(input_img, "rb") as f:
                b = f.read()
        except FileNotFoundError:
            raise Exception("Image file not found ({})".format(input_img))

        # print('get_flash_list_info')

        header = struct.unpack('IIHHIIBBHI', b[:28]) 
        if header[0] != MIGC :
            return None
        else: 
            header_size = header[2]
            prot_size = header[3]
            payload_len = header[4]
            attr = header[5]

            img_id = (attr & IMG_ID_MASK) >> IMG_ID_POS
            # flash list
            if img_id != 14:
                return None
            
            if header_size != 32:
                return None

            list_header = struct.unpack('IBBBB', b[header_size:header_size + 8]) 
            if list_header[0] != 0x6C706673:
                return None

            item_list = []
            flash_list_bin = b[header_size + 8:]
            for i in range(list_header[4]):
                flash_item = struct.unpack('IIBBBBBBBB', flash_list_bin[i * 16: i * 16 + 16])
                item_list.append({'flashSize': flash_item[0],
                                'spiFreqMax': flash_item[1],
                                'spiFormatMax': flash_item[2],
                                'qerType': flash_item[3],
                                'sleepSupport': flash_item[4],
                                'sleepLatency': flash_item[5],
                                'resumeLatency': flash_item[6],
                                'chipId-0': hex(flash_item[7]),
                                'chipId-1': hex(flash_item[8]), 
                                'chipId-2': hex(flash_item[9])
                            })


            flash_list = {'version' : {'iv_major': header[6],
                            'iv_minor': header[7],
                            'iv_revision': header[8],
                            'iv_build_num': header[9]
                            },

                            'info': {'item_size': list_header[3],
                                'item_cnt': list_header[4]
                            },

                            'list': item_list,
                        }

            # print(flash_list)
            with open(output_yaml, 'w') as file:
                yaml.dump(flash_list, file, sort_keys=False)

        return flash_list
    


    def flash_list_export(input_yaml, output_bin):
        # print(input_yaml)
        # print(output_img)

        try:
            with open(input_yaml, "rb") as file:
                flash_list_yaml = yaml.safe_load(file)

                flash_version = flash_list_yaml['version']
                flash_info = flash_list_yaml['info']
                flash_list = flash_list_yaml['list']

                hdr_fmt = ('<' +
                    # Flash_ListHead_t
                    'I' +                        # magic      uint32
                    'B' +                        # vMajor     uint8
                    'B' +                        # vMinor     uint8
                    'B' +                        # itemSize   uint8
                    'B'                          # itemCnt    uint8
                    )  # }
                list_header = struct.pack(hdr_fmt,
                        0x6C706673,                 # magic
                        1,                          # vMajor
                        0,                          # vMajor
                        flash_info['item_size'],    # item_size
                        flash_info['item_cnt'],     # item_cnt
                        )
                flash_buff = bytearray(list_header)

                list_fmt = ('<' +
                    # Flash_ListHead_t
                    'I' +                        # flashSize      uint32
                    'I' +                        # spiFreqMax     uint32
                    'B' +                        # spiFormatMax   uint8
                    'B' +                        # qerType        uint8
                    'B' +                        # sleepSupport   uint8
                    'B' +                        # sleepLatency   uint8
                    'B' +                        # resumeLatency  uint8
                    'B' +                        # chipId         uint8 * 3
                    'B' +
                    'B'
                    )  # }

                for idx in range(flash_info['item_cnt']):
                    list_item = struct.pack(list_fmt,
                            flash_list[idx]['flashSize'],        # flashSize
                            flash_list[idx]['spiFreqMax'],       # spiFreqMax
                            flash_list[idx]['spiFormatMax'],     # spiFormatMax
                            flash_list[idx]['qerType'],          # qerType
                            flash_list[idx]['sleepSupport'],     # sleepSupport
                            flash_list[idx]['sleepLatency'],     # sleepLatency
                            flash_list[idx]['resumeLatency'],    # resumeLatency
                            int(flash_list[idx]['chipId-0'], 16),
                            int(flash_list[idx]['chipId-1'], 16),
                            int(flash_list[idx]['chipId-2'], 16)           # chipId
                            )
                    flash_buff[8 + idx * flash_info['item_size'] : ] = bytearray(list_item)

                    with open(output_bin, "wb") as binfile:
                        binfile.write(flash_buff)

        except FileNotFoundError:
            raise Exception("Image file not found ({})".format(input_yaml))


        return flash_version
