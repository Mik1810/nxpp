#!/usr/bin/env bash
set -euo pipefail

yay -S --noconfirm --needed nxpp

test -f /usr/include/nxpp.hpp
test -f /usr/lib/cmake/nxpp/nxppConfig.cmake

echo "aur-nxpp-ok"
