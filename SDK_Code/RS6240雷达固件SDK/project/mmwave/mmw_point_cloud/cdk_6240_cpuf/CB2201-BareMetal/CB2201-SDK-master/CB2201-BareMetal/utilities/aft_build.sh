#!/bin/sh
PWD=`pwd`

TOPDIR=$(cd `dirname $0`;pwd)

MK_GENERATED_PATH=$PWD/utilities/generated
MK_GENERATED_HEXS_PATH=$TOPDIR/generated/hexs
MK_GENERATED_IMGS_PATH=$TOPDIR/generated/imgs

echo "[INFO] Generated output files ..."

mkdir -p $MK_GENERATED_PATH

cp -rf $MK_GENERATED_IMGS_PATH $MK_GENERATED_HEXS_PATH $MK_GENERATED_PATH

