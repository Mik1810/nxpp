Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

. "$PSScriptRoot/common.ps1"

$snippet = ""
$all = $false
$iterations = 3
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
  scripts/windows/benchmark_snippet_compile.ps1 [options]
  scripts/windows/benchmark_snippet_compile.ps1 <snippet_folder|all> [iterations]
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
if (-not $cxxflags) {
    $cxxflags = "-Wall -Wextra -pedantic -$optLevel"
}

$root = Get-NxppRoot
$snippetRoot = Join-Path $root "snippet"
$compiler = Get-NxppCompiler
$tmpDir = New-NxppTempDirectory -Prefix "nxpp_bench"

try {
    if ($csvPath) {
        New-Item -ItemType Directory -Path (Split-Path -Parent $csvPath) -Force | Out-Null
        "snippet,iterations,baseline_file,nxpp_file,baseline_mean_s,baseline_median_s,nxpp_mean_s,nxpp_median_s,overhead_mean_pct,overhead_median_pct" | Set-Content -Path $csvPath
    }

    function Invoke-CompileBenchmark([string]$sourceFile, [string]$outputFile) {
        $argsList = @("-std=c++20") + ($cxxflags -split '\s+') + @($sourceFile, "-I$root", "-o", $outputFile)
        return (Invoke-NxppTimedCommand -FilePath $compiler -Arguments $argsList)
    }

    function Invoke-One([string]$snippetName) {
        $caseDir = Join-Path $snippetRoot $snippetName
        if (-not (Test-Path $caseDir)) {
            throw "snippet folder not found: $caseDir"
        }

        $pair = Get-NxppSnippetCppPairs -CaseDirectory $caseDir -SnippetName $snippetName
        if (-not $pair) {
            Write-Output "SKIP $snippetName: missing baseline or nxpp C++ file"
            return
        }

        $baselineTimes = New-Object System.Collections.Generic.List[double]
        $nxppTimes = New-Object System.Collections.Generic.List[double]

        if ($verbose) {
            Write-Output "NXPP compile benchmark"
            Write-Output "Snippet: $snippetName"
            Write-Output "Case dir: $caseDir"
            Write-Output "Iterations: $iterations"
            Write-Output "Baseline: $([System.IO.Path]::GetFileName($pair.Baseline))"
            Write-Output "NXPP: $([System.IO.Path]::GetFileName($pair.Nxpp))"
            Write-Output "CXXFLAGS: $cxxflags"
            Write-Output ""
        }

        for ($run = 1; $run -le $iterations; $run++) {
            $baselineBin = Join-Path $tmpDir "${snippetName}_baseline_${run}.exe"
            $nxppBin = Join-Path $tmpDir "${snippetName}_nxpp_${run}.exe"
            $baselineResult = Invoke-CompileBenchmark -sourceFile $pair.Baseline -outputFile $baselineBin
            $nxppResult = Invoke-CompileBenchmark -sourceFile $pair.Nxpp -outputFile $nxppBin
            if ($baselineResult.ExitCode -ne 0) { throw "compilation failed for $($pair.Baseline)" }
            if ($nxppResult.ExitCode -ne 0) { throw "compilation failed for $($pair.Nxpp)" }

            $baselineTimes.Add($baselineResult.Seconds) | Out-Null
            $nxppTimes.Add($nxppResult.Seconds) | Out-Null

            if ($verbose) {
                $overhead = Format-NxppPercent (Get-NxppPercentOverhead -Baseline $baselineResult.Seconds -Candidate $nxppResult.Seconds)
                Write-Output ("Run {0}/{1}: baseline={2}s nxpp={3}s overhead={4}" -f $run, $iterations, ("{0:0.00}" -f $baselineResult.Seconds), ("{0:0.00}" -f $nxppResult.Seconds), $overhead)
            } else {
                Write-Output ("SNIPPET: {0} | RUN: {1}/{2}" -f $snippetName, $run, $iterations)
            }
        }

        $baselineMean = Get-NxppMean $baselineTimes
        $baselineMedian = Get-NxppMedian $baselineTimes
        $nxppMean = Get-NxppMean $nxppTimes
        $nxppMedian = Get-NxppMedian $nxppTimes
        $overheadMean = Get-NxppPercentOverhead -Baseline $baselineMean -Candidate $nxppMean
        $overheadMedian = Get-NxppPercentOverhead -Baseline $baselineMedian -Candidate $nxppMedian

        if ($verbose) {
            Write-Output ""
            Write-Output "Summary"
            Write-Output ("  baseline mean:   {0}s" -f (Format-NxppSeconds $baselineMean))
            Write-Output ("  baseline median: {0}s" -f (Format-NxppSeconds $baselineMedian))
            Write-Output ("  nxpp mean:       {0}s" -f (Format-NxppSeconds $nxppMean))
            Write-Output ("  nxpp median:     {0}s" -f (Format-NxppSeconds $nxppMedian))
            Write-Output ("  overhead mean:   {0}" -f (Format-NxppPercent $overheadMean))
            Write-Output ("  overhead median: {0}" -f (Format-NxppPercent $overheadMedian))
            Write-Output ""
        }

        if ($csvPath) {
            "{0},{1},{2},{3},{4},{5},{6},{7},{8},{9}" -f `
                $snippetName, `
                $iterations, `
                [System.IO.Path]::GetFileName($pair.Baseline), `
                [System.IO.Path]::GetFileName($pair.Nxpp), `
                (Format-NxppSeconds $baselineMean), `
                (Format-NxppSeconds $baselineMedian), `
                (Format-NxppSeconds $nxppMean), `
                (Format-NxppSeconds $nxppMedian), `
                ($(if ($null -eq $overheadMean) { "n/a" } else { "{0:N2}" -f $overheadMean })), `
                ($(if ($null -eq $overheadMedian) { "n/a" } else { "{0:N2}" -f $overheadMedian })) `
                | Add-Content -Path $csvPath
        }
    }

    if ($all) {
        Get-ChildItem -Path $snippetRoot -Directory | Sort-Object Name | ForEach-Object {
            Invoke-One $_.Name
        }
    } else {
        Invoke-One $snippet
    }

    if ($csvPath) {
        Write-Output "CSV written to: $csvPath"
    }
}
finally {
    if (Test-Path $tmpDir) {
        Remove-Item -Recurse -Force $tmpDir
    }
}
