#!/bin/bash

if [ -z "$1" ];then
  echo "Please input rpc server name!"
  exit -1
fi

FILE_NAME=$1
PROJECT_NAME=$(basename ${FILE_NAME})
CURRENT_PATH=$(cd $(dirname $0); pwd)
PROJECT_BIN_FILE="${CURRENT_PATH}"/"${PROJECT_NAME}"
PROJECT_CONF_FILE="../conf/rpc.xml"

echo "Shutdown  rpc project, name: ${PROJECT_NAME}, path: ${PROJECT_BIN_FILE}"

# check bin file exist
if [ ! -e ${PROJECT_BIN_FILE} ]
then
  echo "Shutdown rpc rpc eror, file: ${PROJECT_BIN_FILE} not exist, please check file"
  exit -1
fi


proc_list=`ps -elf | grep "${PROJECT_NAME}" | grep -v 'grep' | grep -v 'close.sh' | awk '{print $4}'`
echo "$proc_list"

for pid in $proc_list; do
  bin_path=`ls -l /proc/${pid}/exe | awk '{print $11}'`
  if [ "$bin_path" = "$PROJECT_BIN_FILE" ];then
    echo "kill this proc: ${bin_path}"
    kill -9 ${pid}
  fi
done
