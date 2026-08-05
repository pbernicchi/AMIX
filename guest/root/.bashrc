# /.bashrc - interactive bash settings for root on AMIX.
# The Delete-key fix itself lives in /.inputrc (readline). This just makes
# canonical-mode programs (cat, read, non-readline input) treat DEL as erase too.
stty erase '^?' 2>/dev/null
