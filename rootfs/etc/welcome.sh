#!/bin/sh

# Get terminal width for dashes
COLS=$(stty size 2>/dev/null | cut -d' ' -f2)
COLS=${COLS:-40}
DASH=$(printf '%*s' "$COLS" | tr ' ' '-')

# System info
OS="CerebelOS"
VERSION="0.0.3a"
KERNEL=$(uname -r)
UPTIME=$(uptime | sed 's/.*up //' | sed 's/,.*//')
SHELL="/bin/sh"
DE="None"
CPU=$(grep 'Model name\|Hardware' /proc/cpuinfo 2>/dev/null | head -1 | cut -d: -f2 | sed 's/^ //')
CPU=${CPU:-"QEMU Virtual CPU"}
MEM_TOTAL=$(grep MemTotal /proc/meminfo | awk '{printf "%.0fMB", $2/1024}')
MEM_FREE=$(grep MemAvailable /proc/meminfo | awk '{printf "%.0fMB", $2/1024}')
BOOT_TIME=$(dmesg | grep "Freeing unused" | tail -1 | grep -o '\[[^]]*\]' | tr -d '[]' | awk '{printf "%.2fs", $1}')

# Disks
DISKS=$(df -h | grep '^/dev/' | awk '{print $1" "$2" used: "$3}')

clear
echo "$DASH"
echo "Welcome to CerebelOS!"
echo "$DASH"
echo ""
echo "OS:       $OS"
echo "Version:  $VERSION"
echo "Boot time:$BOOT_TIME"
echo "Kernel:   $KERNEL"
echo "Uptime:   $UPTIME"
echo "Shell:    $SHELL"
echo "DE:       $DE"
echo "CPU:      $CPU"
echo "Memory:   $MEM_FREE free / $MEM_TOTAL total"
echo ""
echo "$DISKS" | while read line; do
    echo "Disk:     $line"
done
echo ""
echo "$DASH"
echo "Remember! If it works, don't touch it!"
echo "$DASH"
echo ""
