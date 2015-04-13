#!/bin/bash

if [ -z "$EMPI_HOME" ]; then
  echo Please set \$EMPI_HOME.
  exit 1
fi

java -cp $EMPI_HOME/lib-server/eSalsaMPI-server-0.2.jar esalsa.Server $@

#--clusters 2 2>&1 | tee out
