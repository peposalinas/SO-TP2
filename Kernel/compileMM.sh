#!/bin/bash

docker start TP2
docker exec -it TP2 bash -c "cd /root/SO-TP2/Kernel && gcc test_util.c MemoryManagerCDT.c test_mm.c -o testeo"
