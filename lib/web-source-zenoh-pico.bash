# #! /bin/bash

# name="zenoh-pico"
# version="1.7.2"
# GIT="https://github.com/eclipse-zenoh/zenoh-pico"

# set -e

# SCRIPT_PATH="$(readlink -f "${BASH_SOURCE[0]}")"
# SCRIPT_DIR="$(dirname -- "${SCRIPT_PATH}")"

# cd "${SCRIPT_DIR}"

# if [ -d "./zenoh-pico" ]; then
#     echo "${name}@${version} already exists in ${SCRIPT_DIR}, skipping." 
#     exit
# fi

# echo "downloading ${name}@${version} to ${SCRIPT_DIR}"

# git clone --branch "${version}" --depth 1 "${GIT}" "${name}"

# echo "patching library.json"