#!/bin/bash


# ReChain: The distributed research journal
# Copyright (C) 2018  Michael House
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Contact: michaelhouse@gmx.us


# --------------------------------------------

MEM_HOME="data/memcheck/";
LOG="data/memcheck/valgrind.log";
SUPP="data/valgrind.supp";
RESULT=0;

# test file info
TESTDATA="TEST FILE";
TESTFILE="data/memcheck/testfile.txt";
TESTHASH="EF43C9ADC8AEEC6910B4373BA0D9FBF28048ED53EC631AB9FD6FA8AD00A8A3A0";

# command-line args for valgrind
SETTINGS="--leak-check=full                 \
          --show-reachable=yes              \
          --error-limit=no                  \
          --gen-suppressions=all            \
          --track-origins=yes               \
          --suppressions=${SUPP}            \
         ";

# --------------------------------------------

# --------------------------------------------
function setup(){

    echo "  building rechain";
    make clean 1>/dev/null;
    make debug 1>/dev/null;

    echo "  creating memcheck directory";
    if [ ! -d "$MEM_HOME" ]; then
        mkdir $MEM_HOME;
    fi

    echo "  exported new rechain home";
    export RECHAIN_HOME=${MEM_HOME};

    echo "  creating test file";
    echo $TESTDATA > $TESTFILE;
}

function teardown(){
    echo "  destroying memcheck directory";
    rm -rf $MEM_HOME;

    echo "  cleaning obj/bin directories";
    make clean 1>/dev/null;
}

function test(){
    COMMANDS=$1;
    TITLE=$2;

    echo "--------- ${TITLE}:" >> ${LOG}; 
    valgrind ${SETTINGS} --error-exitcode=2 ./bin/rechain ${COMMANDS} &>> ${LOG};
    echo "----------------------------" >> ${LOG}; 

    RESULT=$?;
}

# --------------------------------------------
# check if valgrind is installed before profiling 
if command -v valgrind >/dev/null 2>&1; then

    echo "Running valgrind against rechain"

    setup;

    # ----------------------------------------
    # These tests will have to be updated when 
    # command-line options change. Not all
    # commands need to be run here, just the 
    # larger, significant ones (no '--version'
    # or '--help' test runs).
    # ----------------------------------------

    # publish a record
    echo "  running valgrind publish test"
    test "--publish ${TESTFILE}" "PUBLISH TEST";    

    # mine a block
    echo "  running valgrind mining test"
    test "--mine" "MINING TEST";    
    
    # validate the chain 
    echo "  running valgrind check test"
    test "--check" "CHECK TEST";    

    # sign a record 
    echo "  running valgrind signing test"
    test "--sign ${TESTHASH}" "SIGNING TEST";    


    if [ "$RESULT" -eq "2" ]; then
        echo "Valgrind found some issues:";
        cat $LOG;
        exit 1;
    else
        echo "Valgrind found no issues";
        teardown;
        echo "Exit"
        exit 0;
    fi

else
    echo "Valgrind is NOT installed. Unable to run memcheck!";
fi
