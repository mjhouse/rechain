#!/bin/bash

OS_NAME=$1
STATUS=0;

if [[ "$OS_NAME" == "osx" ]]; then 

    brew install cryptopp       || STATUS=1; 
    rm '/usr/local/include/c++' || STATUS=1;
    brew install gcc5           || STATUS=1;
    eval "$(pyenv init -)"      || STATUS=1;
    pyenv install 3.5.2         || STATUS=1;
    pyenv rehash                || STATUS=1;

    pip install cpp-coveralls   || STATUS=1;
    pyenv rehash                || STATUS=1;
    
    if [[ "$STATUS" == "1" ]]; then
        exit STATUS;
    fi

elif [[ "$OS_NAME" == "linux" ]]; then

    pip install --user cpp-coveralls || exit 1;

fi
    
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )";
DIR=${DIR%/}/data;

mkdir -p ${DIR};
export RECHAIN_HOME=${DIR};
