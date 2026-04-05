Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

param(
    [switch]$Smoke
)

. "$PSScriptRoot/common.ps1"

$root = Get-NxppRoot
$buildDir = Join-Path $root ".tmp\tests-windows"
New-Item -ItemType Directory -Path $buildDir -Force | Out-Null

$compiler = Get-NxppCompiler
$flagsOverride = $env:CXXFLAGS
$sources = Get-NxppTestSources -Smoke:$Smoke

$totalTests = 0
$passedTests = 0
$overallStatus = 0

foreach ($sourceFile in $sources) {
    $binaryName = [System.IO.Path]::GetFileNameWithoutExtension($sourceFile)
    $displayName = [System.IO.Path]::GetFileName($sourceFile)
    $testBin = Join-Path $buildDir "$binaryName.exe"
    $outputFile = Join-Path $buildDir "$binaryName.out"

    Write-Output "Testing $displayName"

    $compileResult = Invoke-NxppCompileSource -SourceFile $sourceFile -OutputFile $testBin -Optimization "O0" -Compiler $compiler -FlagsOverride $flagsOverride
    if ($compileResult.ExitCode -ne 0) {
        if ($compileResult.StdErr) {
            $compileResult.StdErr.TrimEnd() | Write-Output
        }
        $overallStatus = 1
        Write-Output ""
        continue
    }

    $runResult = Invoke-NxppTimedCommand -FilePath $testBin -StdOutPath $outputFile -StdErrPath "$outputFile.err"
    if ($runResult.StdOut) {
        $runResult.StdOut.TrimEnd() | Write-Output
    }
    if ($runResult.StdErr) {
        $runResult.StdErr.TrimEnd() | Write-Output
    }

    $counts = Get-NxppPassFailCounts -OutputText ($runResult.StdOut + [Environment]::NewLine + $runResult.StdErr)
    $totalTests += $counts.Total
    $passedTests += $counts.Passed

    if ($runResult.ExitCode -ne 0) {
        $overallStatus = 1
    }

    Write-Output ""
}

if ($overallStatus -eq 0) {
    Write-Output "[TEST] All tests passed ($passedTests/$totalTests) | PASS"
    exit 0
}

Write-Output "[TEST] All tests passed ($passedTests/$totalTests) | FAIL"
exit 1
