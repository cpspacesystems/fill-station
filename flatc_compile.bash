#! /bin/bash

set -e

SCRIPT_PATH="$(readlink -f "${BASH_SOURCE[0]}")"
SCRIPT_DIR="$(dirname -- "${SCRIPT_PATH}")"

echo "compiling all flatbuffer schemas in ${SCRIPT_DIR}/src/flatbuffers/*.fbs files to ${SCRIPT_DIR}/src/flatbuffers/gen/"

cd "${SCRIPT_DIR}"

mkdir -p "${SCRIPT_DIR}/src/flatbuffers/gen/"

cd "${SCRIPT_DIR}/src/flatbuffers/gen/"
"${SCRIPT_DIR}/flatc" --cpp --cpp-std c++17 --cpp-static-reflection "${SCRIPT_DIR}/src/flatbuffers"/*.fbs