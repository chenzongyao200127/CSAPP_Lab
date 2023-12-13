#!/bin/bash

# Script to find an unused TCP port.

# Define starting port range.
PORT_START=4500

# Define maximum port range.
PORT_MAX=65000

# Initialize port with the starting port value.
port=${PORT_START}

# Start a loop to search for an unused port.
while [ TRUE ] 
do
  # Retrieve a list of used ports.
  portsinuse=`netstat --numeric-ports --numeric-hosts -a --protocol=tcpip | grep tcp | \
    cut -c21- | cut -d':' -f2 | cut -d' ' -f1 | grep -E "[0-9]+" | uniq | tr "\n" " "`

  # Check if the current port is in use.
  echo "${portsinuse}" | grep -wq "${port}"
  
  # If the current port is in use, increment the port number. 
  if [ "$?" == "0" ]; then
    # If the port number reaches the maximum limit, exit with error code -1.
    if [ $port -eq ${PORT_MAX} ]
    then
      exit -1
    fi
    # Increment the port number by one.
    port=`expr ${port} + 1`
  else
    # If the port is not in use, output the port number and exit with success code 0.
    echo $port
    exit 0
  fi
done
