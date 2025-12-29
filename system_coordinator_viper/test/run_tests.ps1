# System Coordinator VIPER Test Runner
# This script compiles and runs all tests in the test directory.
# Exit code 0 if all tests pass, non-zero otherwise.

$ErrorActionPreference = "Stop"
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$projectRoot = Join-Path $scriptDir ".."
$testDir = Join-Path $projectRoot "test"

Write-Host "=== System Coordinator VIPER Test Suite ===" -ForegroundColor Cyan

# Change to test directory
Push-Location $testDir

# Build and run each test
$testFiles = @(
    "test_scv_entity.c",
    "test_scv_interactor.c",
    "test_scv_presenter.c",
    "test_scv_router.c",
    "test_scv_integration.c",
    "test_scv_user_scenarios.c",
    "test_scv_comprehensive.c"
)

$failed = @()
$passed = 0

foreach ($testFile in $testFiles) {
    $testName = [System.IO.Path]::GetFileNameWithoutExtension($testFile)
    $exeName = "$testName.exe"
    $sourcePath = $testFile

    Write-Host "`n--- Building $testName ---" -ForegroundColor Yellow

    # Determine dependencies based on test file
    $deps = @()
    if ($testName -eq "test_scv_entity") {
        $deps = @(
            "../src/scv_entity.c",
            "../src/scv_event_bus.c"
        )
    } elseif ($testName -eq "test_scv_interactor") {
        $deps = @(
            "../src/scv_interactor.c",
            "../src/scv_entity.c",
            "../src/scv_event_bus.c"
        )
    } elseif ($testName -eq "test_scv_presenter") {
        $deps = @(
            "../src/scv_presenter.c",
            "../src/scv_entity.c",
            "../src/scv_interactor.c",
            "../src/scv_event_bus.c"
        )
    } elseif ($testName -eq "test_scv_router") {
        $deps = @(
            "../src/scv_router.c",
            "../src/scv_entity.c",
            "../src/scv_interactor.c",
            "../src/scv_presenter.c",
            "../src/scv_view.c",
            "../src/scv_event_bus.c"
        )
    } else {
        # Integration and comprehensive tests need all sources
        $deps = @(
            "../src/scv.c",
            "../src/scv_view.c",
            "../src/scv_presenter.c",
            "../src/scv_interactor.c",
            "../src/scv_entity.c",
            "../src/scv_router.c",
            "../src/scv_event_bus.c"
        )
    }

    # State machine sources
    $stateMachineSrcs = @(
        "../../state_machine_extended/src/mealy_machine.c",
        "../../state_machine_extended/src/parallel_fsm.c",
        "../../state_machine_extended/src/moore_hierarchical.c",
        "../../state_machine_extended/src/efsm_protocol.c",
        "../../state_machine_extended/src/acceptor_regex.c",
        "../../state_machine_extended/src/fsm_os_adapter.c",
        "../../os_abstraction_layer/build/os_abstract_stub.c"
    )

    # Build command
    $includePaths = @(
        "-I../include",
        "-I../../state_machine_extended/src",
        "-I../../auto-gen/util",
        "-I../../viper_architecture/include",
        "-I../../platform_abstraction_layer/include",
        "-I../../os_abstraction_layer/include"
    ) -join " "

    $compileCmd = "gcc -Wall -Wextra -std=c99 -DMYTRACE_NO_EXECINFO=1 -include stddef.h $includePaths $sourcePath $($deps -join ' ') $($stateMachineSrcs -join ' ') -o $exeName -lm"
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

