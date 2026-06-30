$ErrorActionPreference = 'Stop'

$projectRoot = Split-Path -Parent $PSScriptRoot
$buildDir = Join-Path $env:TEMP 'Lab14_Example_2837xD_IIC_liushui_HostTests'
$gcc = 'C:\msys64\ucrt64\bin\gcc.exe'

if (-not (Test-Path $gcc)) {
    throw "Host GCC not found: $gcc"
}

New-Item -ItemType Directory -Force -Path $buildDir | Out-Null
$testExe = Join-Path $buildDir 'test_board_test.exe'

& $gcc `
    -std=c89 `
    -Wall `
    -Wextra `
    -Werror `
    -DBOARD_TEST_HOST `
    -I (Join-Path $projectRoot 'include') `
    (Join-Path $projectRoot 'Source\Board_Can_Test.c') `
    (Join-Path $projectRoot 'Source\Board_I2c_Test.c') `
    (Join-Path $projectRoot 'Source\Board_Sci_Test.c') `
    (Join-Path $projectRoot 'Source\Board_Spi_Test.c') `
    (Join-Path $projectRoot 'Source\Board_Test.c') `
    (Join-Path $projectRoot 'Source\Board_Gpio_Test.c') `
    (Join-Path $projectRoot 'Source\Board_System_Test.c') `
    -x c `
    (Join-Path $PSScriptRoot 'test_board_test.c.host') `
    -o $testExe

& $testExe
