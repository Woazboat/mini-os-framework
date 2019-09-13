#!/usr/bin/env bash

echo "$1"

(
    echo o      # Create a new empty DOS partition table
    echo p

    echo n      # Add a new partition
    echo p      # Primary partition
    echo 1      # Partition number
    echo 2048   # First sector (start = 1MiB)
    #echo 22527  # Last sector (20480 sectors = 10MiB)
    #echo 206848 # Last sector (204800 sectors = 100MiB)
    echo # +100M use rest of disk

    echo a      # Make partition bootable

    echo t      # Change partition type
    echo 83     # Change to Linux file system

    echo w     # Write changes
) | fdisk "$1"
