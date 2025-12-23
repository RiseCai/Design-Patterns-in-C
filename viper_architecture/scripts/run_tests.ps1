# VIPER Architecture Test Runner
# This script compiles and runs all tests in the test directory.
# Exit code 0 if all tests pass, non-zero otherwise.

$ErrorActionPreference = "Stop"
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$projectRoot = Join-Path $scriptDir ".."
$testDir = Join-Path $projectRoot "test"

Write-Host "=== VIPER Architecture Test Suite ===" -ForegroundColor Cyan

# Change to test directory
Push-Location $testDir

# Build and run each test
$testFiles = @(
    "simple_test.c",
    "test_viper_module.c",
    "test_viper_view.c",
    "test_viper_router.c",
    "test_integration_todo.c",
    "test_pal_simulation.c"
)

$failed = @()
$passed = 0

foreach ($testFile in $testFiles) {
    $testName = [System.IO.Path]::GetFileNameWithoutExtension($testFile)
    $exeName = "$testName.exe"
    $sourcePath = $testFile

    Write-Host "`n--- Building $testName ---" -ForegroundColor Yellow

    # Determine dependencies
    $deps = @(
        "../src/viper.c",
        "../src/viper_view.c",
        "../src/viper_presenter.c",
        "../src/viper_interactor.c",
        "../src/viper_entity.c",
        "../src/viper_router.c"
    )

    # Special handling for PAL simulation test (requires SDL)
    if ($testName -eq "test_pal_simulation") {
        Write-Host "Skipping $testName (requires SDL2)" -ForegroundColor Gray
        continue
    }

    # Build command
    $includePaths = @(
        "-I../include",
        "-I../../state_machine_extended/src",
        "-I../../platform_abstraction_layer/include",
        "-I../../os_abstraction_layer/include"
    ) -join " "

    $libs = @(
        "../../state_machine_extended/src/mealy_machine.o",
        "../../state_machine_extended/src/moore_hierarchical.o",
        "../../state_machine_extended/src/parallel_fsm.o",
        "../../state_machine_extended/src/efsm_protocol.o",
        "../../state_machine_extended/src/acceptor_regex.o",
        "../../state_machine_extended/src/mealy_ui.o",
        "../../state_machine_extended/src/fsm_os_adapter.o",
        "../../os_abstraction_layer/build/libos_abstract.a"
    ) -join " "

    $compileCmd = "gcc -Wall -Wextra -std=c99 $includePaths $sourcePath $($deps -join ' ') $libs -o $exeName -lm"
    Write-Host "Compiling: $compileCmd" -ForegroundColor Gray

    try {
        Invoke-Expression $compileCmd
        if ($LASTEXITCODE -ne 0) {
            throw "Compilation failed with exit code $LASTEXITCODE"
        }

        Write-Host "Running $exeName..." -ForegroundColor Green
        & ".\$exeName"
        if ($LASTEXITCODE -ne 0) {
            throw "Test execution failed with exit code $LASTEXITCODE"
        }
        $passed++
        Write-Host "$testName PASSED" -ForegroundColor Green
    } catch {
        Write-Host "$testName FAILED: $_" -ForegroundColor Red
        $failed += $testName
    }
}

Pop-Location

# Summary
Write-Host "`n=== Test Summary ===" -ForegroundColor Cyan
Write-Host "Passed: $passed" -ForegroundColor Green
Write-Host "Failed: $($failed.Count)" -ForegroundColor Red
if ($failed.Count -gt 0) {
    Write-Host "Failed tests: $($failed -join ', ')" -ForegroundColor Red
    exit 1
} else {
    Write-Host "All tests passed!" -ForegroundColor Green
    exit 0
}
