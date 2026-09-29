Write-Host "Building Lego Sorter hardware executable..."

cmake -S . -B build
cmake --build build

Write-Host "Done."