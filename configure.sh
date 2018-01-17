#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )";
DIR=${DIR%/}/data;

mkdir -p ${DIR};
export RECHAIN_HOME=${DIR};
