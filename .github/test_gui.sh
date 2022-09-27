export PKG_CONFIG_PATH=$(dirname `find /nix/* -name '*.pc'` | sed -z 's/\n/:/g;s/:$/\n/')
make gui/interface_rotate

