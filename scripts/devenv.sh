#!/bin/sh

## This file should not be set to +x as it should NOT be executed directly!

## To use this script you should do:
## source ./scripts/devenv.sh
## from the c client directory. This script assumes that you are executing from 
## that directory and have a folder tree like this:
## 
## root 
## -- lib-c/
## -- -- checked out sources of lib-c
## -- client-c/ <- executing in this directory
## -- -- Makefile
## -- -- scripts/
## -- -- ...

E4LIBPARENT="$(dirname $PWD)"
E4LIBDIR="$E4LIBPARENT/lib-c/"

echo "E4LIBDIR=$E4LIBDIR"
