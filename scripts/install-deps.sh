#!/usr/bin/env bash
# .github/scripts/install-deps.sh
# Install system dependencies for building NeriPlayer Qt on Ubuntu.
set -euo pipefail

sudo apt-get update
sudo apt-get install -y --no-install-recommends \
    build-essential cmake ninja-build pkg-config \
    qt6-base-dev qt6-base-dev-tools qt6-websockets-dev \
    libspdlog-dev libsqlite3-dev libssl-dev \
    libgl1-mesa-dev
