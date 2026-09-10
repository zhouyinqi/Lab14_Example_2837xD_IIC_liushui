$ErrorActionPreference = 'Stop'

$projectRoot = Split-Path -Parent $PSScriptRoot
$buildDir = Join-Path $env:TEMP 'HPD_BoardTest_DSP_HostTests'
$gcc = 'C:\msys64\ucrt64\bin\gcc.exe'

& (Join-Path $PSScriptRoot 'test_ccs_source_scope.ps1')

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
    (Join-Path $projectRoot 'Source\Board_Adc_Test.c') `
    (Join-Path $projectRoot 'Source\Board_Can_Test.c') `
    (Join-Path $projectRoot 'Source\Board_Dido_Test.c') `
    (Join-Path $projectRoot 'Source\Board_Di_Test.c') `
    (Join-Path $projectRoot 'Source\Board_Emif_Test.c') `
    (Join-Path $projectRoot 'Source\Board_Ethernet_Test.c') `
    (Join-Path $projectRoot 'Source\Board_Fpga_Test.c') `
    (Join-Path $projectRoot 'Source\Board_Hdo_Test.c') `
    (Join-Path $projectRoot 'Source\Board_I2c_Test.c') `
    (Join-Path $projectRoot 'Source\Board_Pwm_Test.c') `
    (Join-Path $projectRoot 'Source\Board_Profile.c') `
    (Join-Path $projectRoot 'Source\Board_W5500_Test.c') `
    (Join-Path $projectRoot 'Source\Board_Sci_Test.c') `
    (Join-Path $projectRoot 'Source\Board_Spi_Test.c') `
    (Join-Path $projectRoot 'Source\Board_Test.c') `
    (Join-Path $projectRoot 'Source\Board_Gpio_Test.c') `
    (Join-Path $projectRoot 'Source\Board_System_Test.c') `
    -x c `
    (Join-Path $PSScriptRoot 'test_board_test.c.host') `
    -o $testExe

if ($LASTEXITCODE -ne 0) {
    throw "board_test compilation failed: exit code $LASTEXITCODE"
}

& $testExe
if ($LASTEXITCODE -ne 0) {
    throw "board_test execution failed: exit code $LASTEXITCODE"
}

$profileTestSource = Join-Path $PSScriptRoot 'test_board_profile.c.host'
$profileExe = Join-Path $buildDir 'test_board_profile.exe'
& $gcc `
    -std=c89 `
    -Wall `
    -Wextra `
    -Werror `
    -I (Join-Path $projectRoot 'include') `
    (Join-Path $projectRoot 'Source\Board_Profile.c') `
    -x c `
    $profileTestSource `
    -o $profileExe

if ($LASTEXITCODE -ne 0) {
    throw "board_profile compilation failed: exit code $LASTEXITCODE"
}

& $profileExe
if ($LASTEXITCODE -ne 0) {
    throw "board_profile execution failed: exit code $LASTEXITCODE"
}
