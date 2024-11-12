#!/bin/bash
docker start cont_name
docker exec -it cont_name make clean -C/root/Toolchain
docker exec -it cont_name make clean -C/root
docker exec -it cont_name make -C/root/Toolchain
docker exec -it cont_name make -C/root
./run.sh