#!/bin/bash

OS_NAME=$1
STATUS=0;

failure(){
    if [[ ! -z "$1" ]]; then
        echo $1;
    fi
    exit 0;
}

if [[ "$OS_NAME" == "osx" ]]; then 

    brew install cryptopp       || failure "couldn't install cryptopp!"; 
    rm '/usr/local/include/c++' || failure "couldn't remove symlink!";
    brew install gcc5           || failure "couldn't install gcc5!";
    eval "$(pyenv init -)"      || failure "couldn't init pyenv!";
    pyenv install 3.5.2         || failure "couldn't install python 3.5!";
    pyenv rehash                || failure "couldn't rehash after python 3.5!";

    pip install cpp-coveralls   || failure "couldn't install cpp-coveralls!";
    pyenv rehash                || failure "couldn't rehash after coveralls!";
    
elif [[ "$OS_NAME" == "linux" ]]; then

    pip install --user cpp-coveralls || failure "couldn't install cpp-coveralls!";

fi
    
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )";
DIR=${DIR%/}/data;

mkdir -p ${DIR};
export RECHAIN_HOME=${DIR};
