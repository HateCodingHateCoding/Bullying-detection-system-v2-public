:: psdf env path config

@echo off

pushd "%~dp0"
@set ENV_PATH=%CD%
popd

@set PATH=%ENV_PATH%;%PATH%
:: python
@set PATH=%ENV_PATH%\..\python\WPy64-31321b1\python;%PATH%
@set PATH=%ENV_PATH%\..\python\WPy64-31321b1\python\Scripts;%PATH%


pushd "%ENV_PATH%\.."

python scripts\image_packet\flash_list.py -mode export -path libs/spiflash -input spiflash_list.yaml -output spiflash_list.img

pause

popd
