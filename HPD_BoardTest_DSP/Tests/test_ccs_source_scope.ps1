$ErrorActionPreference = 'Stop'

$projectRoot = Split-Path -Parent $PSScriptRoot
[xml]$description = Get-Content -LiteralPath (Join-Path $projectRoot '.project') -Raw
if ($description.projectDescription.name -ne 'HPD_BoardTest_DSP') {
    throw 'CCS project name must match HPD_BoardTest_DSP.'
}
$repositoryRoot = Split-Path -Parent $projectRoot
if (!(Test-Path -LiteralPath (Join-Path $repositoryRoot 'UpperComputer/CMakeLists.txt')) -or
    (Test-Path -LiteralPath (Join-Path $projectRoot 'UpperComputer'))) {
    throw 'The Qt project must be beside the DSP project, not inside it.'
}
[xml]$project = Get-Content -LiteralPath (Join-Path $projectRoot '.cproject') -Raw
$excludedFolders = @('node_modules', 'outputs', 'UpperComputer', 'platforms', 'tools', 'Tests')
$configurations = $project.SelectNodes('//configuration[sourceEntries]')

foreach ($name in @('Debug', 'Release')) {
    $configuration = @($configurations | Where-Object { $_.GetAttribute('name') -eq $name })
    if ($configuration.Count -ne 1) {
        throw "Expected one CCS $name configuration."
    }

    $entries = $configuration[0].SelectNodes('sourceEntries/entry')
    $rootEntries = @($entries | Where-Object { $_.GetAttribute('name') -eq '' })
    if ($rootEntries.Count -ne 1) {
        throw "Expected one root source entry in CCS $name."
    }

    $exclusions = $rootEntries[0].GetAttribute('excluding').Split('|')
    foreach ($folder in $excludedFolders) {
        if ($exclusions -cnotcontains $folder) {
            throw "CCS $name must exclude $folder from DSP builds."
        }
        foreach ($entry in $entries) {
            $path = $entry.GetAttribute('name').Replace('\', '/')
            if ($path -eq $folder -or $path.StartsWith("$folder/")) {
                throw "CCS $name re-includes excluded PC sources: $path"
            }
        }
    }

    if ($exclusions -cnotcontains 'CMD/2837xD_RAM_lnk_cpu1.cmd') {
        throw "CCS $name must not link the RAM and Flash command files together."
    }
}

Write-Output 'ccs_source_scope: Debug and Release exclude PC tooling and host sources'
