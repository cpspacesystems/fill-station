#! /bin/bash

name="expected-lite"
version="0.10.0"
URL="https://github.com/nonstd-lite/expected-lite/releases/download/v0.10.0/expected.hpp"

set -e

SCRIPT_PATH="$(readlink -f "${BASH_SOURCE[0]}")"
SCRIPT_DIR="$(dirname -- "${SCRIPT_PATH}")"

cd "${SCRIPT_DIR}"

if [ -f "./expected.hpp" ]; then
    echo "${name}@${version} already exists in ${SCRIPT_DIR}, skipping." 
    exit
fi

echo "downloading ${name}@${version} to ${SCRIPT_DIR}"

wget -O "expected.hpp" "${URL}"