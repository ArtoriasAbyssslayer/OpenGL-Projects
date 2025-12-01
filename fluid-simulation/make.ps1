$BuildDir = "build"
# Default flags
$Clean = $false
$Build = $false
$Regenerate = $false
# Parse command-line arguments
foreach ($arg in $args) {
    switch ($arg.ToLower()) {
        "--clean"      { $Clean = $true }
        "--build"      { $Build = $true }
        "--rebuild" { $Regenerate = $true }
        default        { Write-Host "Unknown argument: $arg"; exit 1 }
    }
}

# Clean
if ($Clean -or $Regenerate) {
    if (Test-Path $BuildDir) {
        Write-Host "Removing build directory..."
        Remove-Item -Recurse -Force $BuildDir
        Write-Host "Build directory removed."
    } elseif ($Clean) {
        Write-Host "Build directory does not exist. Nothing to clean."
        exit
    }
}

# Build / Regenerate
if ($Build -or $Regenerate) {
    if (-not (Test-Path $BuildDir)) {
        Write-Host "Creating build directory..."
        New-Item -ItemType Directory -Path $BuildDir | Out-Null
    }

    Push-Location $BuildDir

    Write-Host "Generating CMake project..."

    # Generate project
    cmake -G "Visual Studio 17 2022" -A x64 `
        -DCMAKE_CXX_FLAGS_RELEASE="/GL /w /W1" `
        -DCMAKE_EXE_LINKER_FLAGS_RELEASE="/LTCG" `
        ..

    Write-Host "Building project (Release / Optimized, using all cores)..."

    # Build with max CPU count
    cmake --build . --config Release -- /maxcpucount

    Pop-Location

    Write-Host "...build finished."
    exit
}

Write-Host "Usage: "
Write-Host "  ./make.ps1 --build       # Generate & build the project"
Write-Host "  ./make.ps1 --clean       # Remove the build directory"
Write-Host "  ./make.ps1 --rebuild  # Clean + Generate + Build"
