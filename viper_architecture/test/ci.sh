#!/bin/bash
# CI script for VIPER architecture tests
set -e

echo "Building VIPER tests..."
make clean
make all

echo "Running tests..."
./test_viper_module
./test_integration_todo
./test_pal_simulation
./test_viper_view
./test_viper_interactor
./test_viper_entity
./test_viper_presenter

echo "All tests passed!"
