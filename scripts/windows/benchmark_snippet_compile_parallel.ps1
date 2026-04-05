Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

. "$PSScriptRoot/common.ps1"

$snippet = ""
$all = $false
$iterations = 3
$jobs = 1
$optLevel = "O3"
$cxxflags = ""
$csvPath = ""
$verbose = $false
$positionals = New-Object System.Collections.Generic.List[string]

for ($i = 0; $i -lt $args.Count; $i++) {
    switch ($args[$i]) {
        "--snippet" { $i++; $snippet = $args[$i] }
        "--all" { $all = $true }
        "--iterations" { $i++; $iterations = [int]$args[$i] }
        "--jobs" { $i++; $jobs = [int]$args[$i] }
        "--opt-level" { $i++; $optLevel = $args[$i] }
        "--cxxflags" { $i++; $cxxflags = $args[$i] }
        "--csv" { $i++; $csvPath = $args[$i] }
        "--verbose" { $verbose = $true }
        "-h" { $positionals.Add("--help") | Out-Null }
        "--help" { $positionals.Add("--help") | Out-Null }
        default { $positionals.Add($args[$i]) | Out-Null }
    }
}

if ($positionals.Contains("--help")) {
    @"
Usage:
  scripts/windows/benchmark_snippet_compile_parallel.ps1 [options]
  scripts/windows/benchmark_snippet_compile_parallel.ps1 <snippet_folder|all> [iterations]
"@ | Write-Output
    exit 0
}

if (-not $snippet -and -not $all -and $positionals.Count -ge 1) {
    if ($positionals[0] -eq "all") {
        $all = $true
    } else {
        $snippet = $positionals[0]
    }
}
if ($positionals.Count -ge 2 -and $iterations -eq 3) {
    $iterations = [int]$positionals[1]
}

if (-not $snippet -and -not $all) {
    throw "specify --snippet <name> or --all"
}

$parallelArgs = @()
if ($snippet) { $parallelArgs += @("--snippet", $snippet) }
if ($all) { $parallelArgs += "--all" }
$parallelArgs += @("--iterations", "$iterations", "--opt-level", $optLevel)
if ($cxxflags) { $parallelArgs += @("--cxxflags", $cxxflags) }
if ($csvPath) { $parallelArgs += @("--csv", $csvPath) }
if ($verbose) { $parallelArgs += "--verbose" }

Write-Warning "The PowerShell parallel benchmark currently reuses the serial benchmark implementation. The --jobs argument is accepted for CLI compatibility but not yet applied."

& "$PSScriptRoot/benchmark_snippet_compile.ps1" @parallelArgs
exit $LASTEXITCODE
