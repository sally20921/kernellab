# Usage

## Build Project

        $ make

## Find physical address

        $ sudo su
        # cd /sys/kernel/debug/ptree
        # echo [pid] >> input
        # cat ptree

## Clean Project

        $ make clean
