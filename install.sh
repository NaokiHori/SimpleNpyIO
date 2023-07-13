#!/bin/bash

# https://github.com/NaokiHori/SimpleNpyIO

# Description
#
# This script installs SimpleNpyIO library to your machine,
#   so that you can use them without including
#   the source src/snpyio.c and the header include/snpyio.h
#   to your project.
#
# Usage (install)
#
#   $ bash install.sh install
#
#   This will create
#     a dynamic library libsnpyio.so
#     a header file     snpyio.h
#   to a specified place (default: ~/.local)
#
# Usage (uninstall)
#
#   $ bash install.sh uninstall
#
#   This will remove
#     a dynamic library libsnpyio.so
#     a header file     snpyio.h
#   from a specified place (default: ~/.local)
#
# Confirm that you have a proper permission.

# installation parameters
prefix=~/.local
dynlib=${prefix}/lib/libsnpyio.so
header=${prefix}/include/snpyio.h
cflags="-std=c99 -O3 -Wall -Wextra"

# check number of arguments is 1
if [ "$#" -ne 1 ]; then
  echo "Give one of \"install\" or \"uninstall\""
  exit 1
fi
arg="${1}"

# check the argument is "install" or "uninstall", and process
if [ "${arg}" = "install" ]; then
  echo "Install to ${prefix}"
  # check place to install
  dname=$(dirname ${dynlib})
	if [ ! -e "${dname}" ]; then
    echo "Create directory: ${dname}"
		mkdir -p "${dname}"
	fi
  dname=$(dirname ${header})
	if [ ! -e "${dname}" ]; then
    echo "Create directory: ${dname}"
		mkdir -p "${dname}"
	fi
  # create dynamic library
  echo "Dynamic library ${dynlib} is created"
	cc -Iinclude ${cflags} -fPIC -shared src/snpyio.c -o ${dynlib}
  # copy header
  echo "Header ${header} is created"
  cp include/snpyio.h ${header}
elif [ "${arg}" = "uninstall" ]; then
  echo "Uninstall from ${prefix}"
  # remove dynamic library
  if [ -e ${dynlib} ]; then
    echo "Dynamic library ${dynlib} is removed"
    rm ${dynlib}
  fi
  # remove header
  if [ -e ${header} ]; then
    echo "Header ${header} is removed"
    rm ${header}
  fi
else
  echo "Give one of \"install\" or \"uninstall\""
  exit 1
fi
