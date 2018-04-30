#!/bin/bash

# remove any previously existing hook softlinks
find .git/hooks -type l -exec rm {} \;

# link all scripts in .hooks to the hooks directory
find .hooks -type f -exec ln -sf ../../{} .git/hooks/ \;
