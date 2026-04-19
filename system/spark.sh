#!/bin/sh

# Spark - CerebelOS Init System
# Version: 0.0.1a

STARTUP_DIR="/etc/spark/startup"
SHUTDOWN_DIR="/etc/spark/shutdown"

log() {
    echo "[Spark] $1"
}

run_scripts() {
    dir=$1
    if [ -d "$dir" ]; then
        for script in "$dir"/*.sh; do
            if [ -x "$script" ]; then
                log "Running $script"
                "$script"
            fi
        done
    fi
}

do_shutdown() {
    log "Shutting down..."
    run_scripts "$SHUTDOWN_DIR"
    log "Goodbye!"
    /bin/busybox poweroff -f
}

do_reboot() {
    log "Rebooting..."
    run_scripts "$SHUTDOWN_DIR"
    /bin/busybox reboot -f
}

trap do_shutdown TERM
trap do_reboot HUP

log "Starting CerebelOS..."
run_scripts "$STARTUP_DIR"
log "Boot complete!"

while true; do
    /bin/busybox getty 0 ttyAMA0
done
