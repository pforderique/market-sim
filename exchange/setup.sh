#!/bin/bash

echo "🔧 Checking for SQLite development library..."

if ! dpkg -s libsqlite3-dev &> /dev/null; then
  echo "📦 libsqlite3-dev not found. Installing..."
  sudo apt update && sudo apt install -y libsqlite3-dev
else
  echo "✅ libsqlite3-dev already installed"
fi

echo "🛠️  Building exchange server..."
make server
