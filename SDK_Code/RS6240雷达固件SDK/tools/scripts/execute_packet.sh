#!/bin/bash

# generate symbol file
ELF_PATH=`find ${CURRENT_PRJ_PATH}/Obj -name "*.elf"`
ProjectName=$(basename ${ELF_PATH} .elf)
PREFIX=riscv64-unknown-elf-
${PREFIX}nm "${CURRENT_PRJ_PATH}/Obj/${ProjectName}.elf" > "${CURRENT_PRJ_PATH}/Obj/${ProjectName}.symbols"

# packet image
PYTHON_PATH=$SDK_ROOT_DIR/tools/python/WPy64-31321b1/python
if [ ! -d  "${PYTHON_PATH}" ];then
    echo "not found python path !!!"
    exit
fi

PYTHON=$PYTHON_PATH/python.exe
if [ ! -f  "${PYTHON}" ];then
    echo "not found python !!!"
    exit
fi

IMAGE_PACKET_SCRIPT=$SDK_ROOT_DIR/tools/scripts/image_packet/cdk_to_image_packet.py
if [ ! -f  "${IMAGE_PACKET_SCRIPT}" ];then
    echo "not found cdk_to_image_packet script !!!"
    exit
fi

$PYTHON $IMAGE_PACKET_SCRIPT


