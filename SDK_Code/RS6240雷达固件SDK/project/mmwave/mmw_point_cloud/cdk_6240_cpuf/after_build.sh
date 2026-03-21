#!/bin/bash

export CURRENT_PRJ_PATH=${ProjectPath}

find_project_root() {
    local start_dir="${1:-$(pwd)}"
    local current_dir

    # Resolve to absolute path and handle symbolic connections
    current_dir=$(cd -- "$start_dir" && pwd -P) || {
        echo "Error:Path parsing failed,'$start_dir'" >&2
        return 1
    }

    # if input file path, get dir
    if [ ! -d "$currnet_dir" ]; then
        current_dir=$(dirname -- "$current_dir")
    fi

    # Step by step backtracking check
    while [ "$current_dir" != "/" ]; do
        if [ -d "$current_dir/tools" ] && [ -f "$current_dir/tools/psdf.config" ]; then
            echo "$current_dir"
            return 0
        fi
        current_dir=$(dirname -- "$current_dir")
    done

    # finally check root dir
    if [ -d "/tools" ] && [ -f "/tools/psdf.config" ]; then
        echo "/"
        return 0
    fi

    echo "Error: Root directory containing ‘tools/psdf.config’ not found." >&2
    return 1
}

# find sdk root dir
root_dir=$(find_project_root $CURRENT_PRJ_PATH)
status=$?
if [ "$status" -ne 0 ];then
    exit 1
fi
export SDK_ROOT_DIR=${root_dir}

# execute packt img sh
source "$root_dir/tools/scripts/execute_packet.sh"

