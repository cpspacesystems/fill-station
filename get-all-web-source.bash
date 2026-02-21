#! /bin/bash

set -e

SCRIPT_PATH="$(readlink -f "${BASH_SOURCE[0]}")"
SCRIPT_DIR="$(dirname -- "${SCRIPT_PATH}")"

cd "${SCRIPT_DIR}"

echo "Running all web-source getter scripts in ${SCRIPT_DIR}."
read -p "Press [Enter] key to continue..."

find . -type f -name "web-source-*.bash" -exec bash {} \;