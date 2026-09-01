#!/usr/bin/env bash
set -e
APP_DIR="$(cd "$(dirname "$0")" && pwd)"
DISPLAY_VALUE="${DISPLAY:-:0}"
XAUTHORITY_VALUE="${XAUTHORITY:-}"
QT_PLATFORM="${QT_QPA_PLATFORM:-xcb}"
# Avoid incompatible core20 libraries injected by VS Code/Snap.
exec env -i HOME="${HOME}" USER="${USER:-$(id -un)}" PATH="/usr/bin:/bin" DISPLAY="${DISPLAY_VALUE}" XAUTHORITY="${XAUTHORITY_VALUE}" QT_QPA_PLATFORM="${QT_PLATFORM}" bash -lc 'source /opt/ros/humble/setup.bash; export LD_LIBRARY_PATH="'"${APP_DIR}"'/lib:/opt/ros/humble/lib:/lib/x86_64-linux-gnu:/usr/lib/x86_64-linux-gnu"; exec "'"${APP_DIR}"'/agt_robot_hmi"'
