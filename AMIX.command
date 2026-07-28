#!/bin/sh
# Launch the Amiga 3000UX / AMIX config under FS-UAE.
# Double-click in Finder, or run from a shell.

FSUAE="/Applications/FS-UAE.app/Contents/MacOS/fs-uae"
CONFIG="$HOME/Documents/FS-UAE/Configurations/AMIX-A3000UX.fs-uae"
LOG="$HOME/Documents/FS-UAE/Cache/Logs/fs-uae.log.txt"

# Two instances would open a3000ux.hdf read-write at the same time.
if pgrep -f "MacOS/fs-uae" >/dev/null 2>&1; then
	echo "FS-UAE is already running -- refusing to start a second instance."
	echo "Both would hold a3000ux.hdf read-write and corrupt the filesystem."
	exit 1
fi

[ -x "$FSUAE" ]  || { echo "Not executable: $FSUAE";  exit 1; }
[ -r "$CONFIG" ] || { echo "Not readable:  $CONFIG"; exit 1; }

# The A2065 dumps every packet in full hex; the log hits ~1.5 GB in two hours.
: > "$LOG" 2>/dev/null

echo "Starting AMIX from $CONFIG"
echo
echo "  Boot takes ~70s, or ~145s with the tape attached."
echo "  Run 'shutdown -i0' inside AMIX before closing FS-UAE."
echo

"$FSUAE" "$CONFIG"

# 'MMU enabled' is the reliable boot signal. The SCSI overflow flood in the
# log is noise and appears during healthy boots too -- never diagnose with it.
echo
if [ "$(grep -c 'MMU enabled' "$LOG" 2>/dev/null)" -gt 0 ]; then
	echo "FS-UAE exited. Kernel had started (MMU enabled seen in log)."
else
	echo "FS-UAE exited. NO 'MMU enabled' in the log -- the kernel never"
	echo "started. Check for a second hardfile on the SCSI bus, or"
	echo "uae_cpu_speed = max having crept back into the config."
fi
