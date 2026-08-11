param(
    [Parameter(Mandatory = $true)]
    [string]$OutputDirectory
)

$ErrorActionPreference = 'Stop'
$scriptDirectory = Split-Path -Parent $MyInvocation.MyCommand.Path
$sourceFile = Join-Path $scriptDirectory 'CanBridge32\CanBridge32.cs'
$compiler = 'C:\Windows\Microsoft.NET\Framework\v4.0.30319\csc.exe'

if (-not (Test-Path -LiteralPath $sourceFile)) {
    throw "CanBridge32 source is missing: $sourceFile"
}
if (-not (Test-Path -LiteralPath $compiler)) {
    throw "32-bit .NET Framework compiler is missing: $compiler"
}

New-Item -ItemType Directory -Force -Path $OutputDirectory | Out-Null
& $compiler /nologo /target:exe /platform:x86 "/out:$(Join-Path $OutputDirectory 'HpdCanBridge32.exe')" $sourceFile
if ($LASTEXITCODE -ne 0) {
    throw "CanBridge32 build failed with exit code $LASTEXITCODE"
}
