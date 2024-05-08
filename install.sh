#!/bin/bash

# 静态库文件
LIBRARY_FILE="./lib/libmircorpc.a"
# 头文件目录
INCLUDE_DIR="./rpc"

chmod +x install.sh
# 安装静态库到/usr/local/lib
sudo cp $LIBRARY_FILE /usr/local/lib/

sudo cp -r $INCLUDE_DIR /usr/local/include/

sudo find /usr/local/include/rpc -type f -name "*.cc" -delete


# 更新动态链接库缓存
sudo ldconfig

echo "安装完成"
