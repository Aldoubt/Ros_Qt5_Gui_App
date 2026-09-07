#!/usr/bin/env bash
set -e
APP_DIR="$(cd "$(dirname "$0")" && pwd)"
APP_BIN="${APP_DIR}/agt_robot_hmi"
APP_LIB_DIR="${APP_DIR}/lib"
# In a colcon workspace the launcher is kept in the source tree while the
# executable and channel plugins are installed under install/.
if [ ! -x "${APP_BIN}" ]; then
  WORKSPACE_DIR="$(cd "${APP_DIR}/../../../../.." && pwd)"
  APP_BIN="${WORKSPACE_DIR}/install/agt_robot_hmi/bin/agt_robot_hmi"
  APP_LIB_DIR="${WORKSPACE_DIR}/install/agt_robot_hmi/bin/lib"
fi
if [ ! -x "${APP_BIN}" ]; then
  echo "agt_robot_hmi executable not found: ${APP_BIN}" >&2
  exit 1
fi
DISPLAY_VALUE="${DISPLAY:-:0}"
XAUTHORITY_VALUE="${XAUTHORITY:-}"
QT_PLATFORM="${QT_QPA_PLATFORM:-xcb}"
CONFIG_DIR="${AGT_HMI_CONFIG_DIR:-$PWD}"
# Avoid incompatible core20 libraries injected by VS Code/Snap.
exec env -i HOME="${HOME}" USER="${USER:-$(id -un)}" PATH="/usr/bin:/bin" DISPLAY="${DISPLAY_VALUE}" XAUTHORITY="${XAUTHORITY_VALUE}" QT_QPA_PLATFORM="${QT_PLATFORM}" AGT_HMI_CONFIG_DIR="${CONFIG_DIR}" bash -lc 'source /opt/ros/humble/setup.bash; export LD_LIBRARY_PATH="'"${APP_LIB_DIR}"':/opt/ros/humble/lib:/lib/x86_64-linux-gnu:/usr/lib/x86_64-linux-gnu"; cd "$AGT_HMI_CONFIG_DIR"; exec "'"${APP_BIN}"'"'
