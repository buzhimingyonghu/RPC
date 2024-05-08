#!/bin/bash

# 静态库文件
LIBRARY_FILE="/usr/local/lib/libmircorpc.a"
# 头文件目录
INCLUDE_DIR="/usr/local/include/rpc"
chmod +x uninstall.sh
# 删除静态库文件
sudo rm $LIBRARY_FILE

# 删除头文件目录及其内容
sudo rm -rf $INCLUDE_DIR

echo "卸载完成"
