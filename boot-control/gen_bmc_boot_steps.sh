#!/bin/bash

DIR=$1           #Base directory
CHIP_VERSION=$2  #Chip verison

if [[ $CHIP_VERSION == "p9" ]]
then
  cat $DIR/boot-control/bmc_boot_steps_p9.cpp > $DIR/boot-control/bmc_boot_steps.cpp
  cat $DIR/boot-control/bmc_boot_steps_p9.hpp > $DIR/boot-control/bmc_boot_steps.hpp
else # Default is p10
  cat $DIR/boot-control/bmc_boot_steps_p10.cpp > $DIR/boot-control/bmc_boot_steps.cpp
  cat $DIR/boot-control/bmc_boot_steps_p10.hpp > $DIR/boot-control/bmc_boot_steps.hpp
fi
