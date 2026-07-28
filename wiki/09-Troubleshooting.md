# Troubleshooting

## White screen, machine never boots

The single reliable signal is `68030 MMU enabled` in the FS-UAE log. Its
absence means the kernel never started:

    grep -c "MMU enabled" ~/Documents/FS-UAE/Cache/Logs/fs-uae.log.txt

Two known causes, both confirmed by A/B testing:

1. `uae_cpu_speed = max` in the config. Remove it.
2. A second hardfile on the SCSI bus. Remove it. A tape at ID 4 is fine.

**`SCSI command buffer overflow!` is not a diagnosis.** It floods the log —
millions of lines, gigabytes per session — during healthy boots as well as hung
ones. It cost two wrong conclusions here before that was established.

## The log grows without bound

The A2065 dumps every packet in full hex. Expect ~1.5 GB per two hours of
networked uptime. There is no option to disable it; the binary exposes only
`logs_dir`, `log_flush` and `save_log`. Either truncate between sessions:

    : > ~/Documents/FS-UAE/Cache/Logs/fs-uae.log.txt

or, for long unattended runs, redirect it entirely:

    ln -sf /dev/null ~/Documents/FS-UAE/Cache/Logs/fs-uae.log.txt

Keep logging on when debugging — it is what identified both boot failures.

## Config changes appear to have no effect

FS-UAE reads the config **only at launch**. A soft reset inside the guest is
not enough; quit and relaunch.

Worse, the **Launcher keeps its own drive list** and passes it at startup even
when the config file has no matching entry. A config with no `hard_drive_1`
line still had `transfer.hdf` attached at launch, because the Launcher supplied
it. Clear the slot in the GUI, or launch from the config path directly:

    /Applications/FS-UAE.app/Contents/MacOS/fs-uae ~/Documents/FS-UAE/Configurations/AMIX-A3000UX.fs-uae

## Never run two instances

Both would hold `a3000ux.hdf` read-write and corrupt the filesystem.
[`AMIX.command`](../AMIX.command) refuses to start if one is already running.

## Shutting down

    shutdown -i0

`shutdown -i0` halts AMIX but leaves the FS-UAE window open; closing it at that
point is safe. Quitting FS-UAE on a *running* system leaves the root filesystem
dirty and forces an fsck on the next boot.

## Symptoms and causes

| Symptom | Cause |
|---|---|
| `tar: problem reading group entry` then core dump | ustar archive with unknown owner names — rebuild as v7 |
| `setclk: setclk000755: not found` | A tar header was written where a raw binary was expected |
| `/dev/rmt/4nn: cannot open` | `/usr/ucb/mt` appends `n` itself — pass `/dev/rmt/4` |
| `pkgadd ... errno=2` on a file that exists | `-d` needs an absolute path |
| `Installation partially failed`, group `staff` | `groupadd staff` |
| `Entropy collection failed`, PRNGD port 708 | prngd not installed or not running |
| `sshd: no hostkeys available` | Keys generated under the wrong filenames |
| Grey X root window, no window manager | X server started, client did not — suspect `.xserverrc` |
| `telnet localhost` hangs | Check `lo0` is configured and `localhost` is in `/etc/hosts` |
