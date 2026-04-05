Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

. "$PSScriptRoot/common.ps1"

$root = Get-NxppRoot
$scriptPath = Join-Path $root "scripts/build_single_header.py"
$status = Invoke-NxppPython -Arguments @($scriptPath) + $args
exit $status
