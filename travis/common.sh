#!/bin/bash

function verify_image()
{
  # track the exit code for this platform
  local exit_code=0
  local build_stdout

  local directory=$PWD
  local filename="/src/WiFiSensor/WiFiSensor.ino"
  local sketch="$directory/$filename"

  echo "$PWD/src/WiFiSensor/WiFiSensor.ino"
  build_stdout=$(arduino --verify $sketch 2>&1)

  # echo output if the build failed
  if [ $? -ne 0 ]; then
    # heavy X
    echo -e "\xe2\x9c\x96"
    echo -e "----------------------------- DEBUG OUTPUT -----------------------------\n"
    echo "$build_stdout"
    echo -e "\n------------------------------------------------------------------------\n"

    # mark as fail
    exit_code=1
  else
    # heavy checkmark
    echo -e "\xe2\x9c\x93"
  fi
  # done
  return $exit_code
}

function build_image()
{
  # track the exit code for this platform
  local exit_code=0
  local build_stdout
  build_stdout=$(arduino --verify $sketch 2>&1)

  # echo output if the build failed
  if [ $? -ne 0 ]; then
    # heavy X
    echo -e "\xe2\x9c\x96"
    echo -e "----------------------------- DEBUG OUTPUT -----------------------------\n"
    echo "$build_stdout"
    echo -e "\n------------------------------------------------------------------------\n"

    # mark as fail
    exit_code=1
  else
    # heavy checkmark
    echo -e "\xe2\x9c\x93"
  fi
  # done
  return $exit_code
}
