export PKG_CONFIG_PATH=$(dirname `find /nix/* -name '*.pc'` | sed -z 's/\n/:/g;s/:$/\n/')
echo $PKG_CONFIG_PATH
make gui/interface_rotate

