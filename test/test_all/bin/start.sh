#!/bin/bash

FILE_NAME=$1

PROJECT_NAME=$(basename ${FILE_NAME})

CURRENT_PATH=$(cd $(dirname $0); pwd)

PROJECT_ROOT_PATH=$(cd $(dirname $0); cd ..; pwd)

PROJECT_BIN_FILE="${CURRENT_PATH}"/"${PROJECT_NAME}"

PROJECT_CONF_FILE="../conf/rpc.xml"

# echo "FILE_NAME: ${FILE_NAME}"
# echo "PROJECT_NAME: ${PROJECT_NAME}"
# echo "CURRENT_PATH: ${CURRENT_PATH}"
# echo "PROJECT_ROOT_PATH: ${PROJECT_ROOT_PATH}"
# echo "PROJECT_BIN_FILE: ${PROJECT_BIN_FILE}"
# echo "PROJECT_CONF_FILE: ${PROJECT_CONF_FILE}"
if [ -z "$1" ]; then
  echo "Please input executable binary file!"
  exit -1
fi

if [ ! -e ${PROJECT_BIN_FILE} ]; then
  echo "Run rpc server error, file: ${PROJECT_BIN_FILE} does not exist, please check file"
  exit -1
fi

if [ ! -e ${PROJECT_CONF_FILE} ]; then
  echo "Run rpc error, config file: ${PROJECT_CONF_FILE} does not exist, please check config file"
  exit -1
fi

if [ ! -x ${PROJECT_BIN_FILE} ]; then
  echo "Changing permissions to make the binary executable: ${PROJECT_BIN_FILE}"
  chmod +x ${PROJECT_BIN_FILE}
fi

sh close.sh ${PROJECT_NAME}

nohup ./${PROJECT_NAME} ${PROJECT_CONF_FILE} &> ${PROJECT_ROOT_PATH}/log/${PROJECT_NAME}.nohup_log &
echo "Started rpc server with config file: ${PROJECT_CONF_FILE}"
echo "Log file: ${PROJECT_ROOT_PATH}/log/${PROJECT_NAME}.nohup_log"