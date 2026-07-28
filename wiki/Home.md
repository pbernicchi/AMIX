# Amiga 3000UX / AMIX under FS-UAE

Running Amiga UNIX (AMIX) System V Release 4, version 2.1 patched to 2.1p2a,
on an emulated Amiga 3000UX under FS-UAE 3.2.35 on macOS.

    $ uname -a
    UNIX_System_V amix 4.0 2.1c 0800430 Amiga (Unlimited) m68k

## Pages

1. [Sources and media](01-Sources-and-Media.md)
2. [FS-UAE configuration](02-FS-UAE-Configuration.md)
3. [Installing AMIX](03-Installation.md)
4. [Patching to 2.1p2a](04-Patching.md)
5. [Networking and DNS](05-Networking-and-DNS.md)
6. [X11 and the A2410](06-X11-and-the-A2410.md)
7. [Moving files in](07-Moving-Files-In.md)
8. [Installing software](08-Installing-Software.md)
9. [Troubleshooting](09-Troubleshooting.md)

## Attribution

The installation procedure, the distribution media and the SVR4 package
collection all come from the **Amiga Unix Wiki**, <https://www.amigaunix.com/>,
which is the reference for anything AMIX-specific. Pages here cite it where it
is the source.

Where this setup departs from that procedure — emulator settings, the SCSI bus
limits, the file-transfer route, the networking caveats on macOS — those are
findings from this build and are **not** attributable to the wiki. They are
marked as such on each page.
