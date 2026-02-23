#! /bin/bash

# name="freertos"
# version="v202411.00"
# URL="https://github.com/FreeRTOS/FreeRTOS/releases/download/202411.00/FreeRTOSv202411.00.zip"

# set -e

# SCRIPT_PATH="$(readlink -f "${BASH_SOURCE[0]}")"
# SCRIPT_DIR="$(dirname -- "${SCRIPT_PATH}")"

# cd "${SCRIPT_DIR}"

# if [ -d "./freertos" ]; then
#     echo "${name}@${version} already exists in ${SCRIPT_DIR}, skipping." 
#     exit
# fi

# echo "downloading ${name}@${version} to ${SCRIPT_DIR}"

# wget -O "freertos.zip" "${URL}"

# unzip ./freertos.zip -d ./freertos

# rm ./freertos.zip
# mv ./freertos/FreeRTOSv202411.00/* ./freertos