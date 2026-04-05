Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

param(
    [string]$HeaderUnderTest = ""
)

. "$PSScriptRoot/common.ps1"

$root = Get-NxppRoot
$buildDir = Join-Path $root ".tmp\large-graph-compare-windows"
New-Item -ItemType Directory -Path $buildDir -Force | Out-Null

$compiler = Get-NxppCompiler
$flagsOverride = if ($env:CXXFLAGS) { $env:CXXFLAGS } else { "" }
$sourceFile = Join-Path $root "tests/test_large_graph_compare.cpp"
$testBin = Join-Path $buildDir "test_large_graph_compare.exe"
$outputFile = Join-Path $buildDir "test_large_graph_compare.out"
$definitions = @{}

if ($HeaderUnderTest) {
    Write-Output "Testing test_large_graph_compare.cpp against $HeaderUnderTest"
    $definitions["NXPP_HEADER_UNDER_TEST"] = "`"$HeaderUnderTest`""
} else {
    Write-Output "Testing test_large_graph_compare.cpp"
}

$compileResult = Invoke-NxppCompileSource -SourceFile $sourceFile -OutputFile $testBin -Optimization "O2" -Compiler $compiler -FlagsOverride $flagsOverride -Definitions $definitions
if ($compileResult.ExitCode -ne 0) {
    if ($compileResult.StdErr) {
        $compileResult.StdErr.TrimEnd() | Write-Output
    }
    Write-Output ""
    Write-Output "[TEST] Large-graph comparison passed | FAIL"
    exit 1
}

$runResult = Invoke-NxppTimedCommand -FilePath $testBin -StdOutPath $outputFile -StdErrPath "$outputFile.err"
if ($runResult.StdOut) {
    $runResult.StdOut.TrimEnd() | Write-Output
}
if ($runResult.StdErr) {
    $runResult.StdErr.TrimEnd() | Write-Output
}

Write-Output ""
if ($runResult.ExitCode -eq 0) {
    Write-Output "[TEST] Large-graph comparison passed | PASS"
    exit 0
}

Write-Output "[TEST] Large-graph comparison passed | FAIL"
exit 1
