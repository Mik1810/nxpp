param(
    [Parameter(Mandatory = $true, Position = 0)]
    [string]$SnippetFolder,
    [Parameter(Position = 1)]
    [string]$StdinFile = ""
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

. "$PSScriptRoot/common.ps1"

$root = Get-NxppRoot
$snippetRoot = Join-Path $root "snippet"
$caseDir = Join-Path $snippetRoot $SnippetFolder
$logDir = Join-Path $caseDir "logs"
$logFile = Join-Path $logDir "run_all.log"
$tmpDir = New-NxppTempDirectory -Prefix "nxpp_snippet_log"

try {
    if (-not (Test-Path $caseDir)) {
        throw "snippet folder not found: $caseDir"
    }

    $python = Get-NxppPythonPath
    $compiler = Get-NxppCompiler
    $stdinPath = Resolve-NxppSnippetInput -SnippetName $SnippetFolder -UserInputPath $StdinFile -TempDirectory $tmpDir

    $cppFiles = Get-ChildItem -Path $caseDir -Filter *.cpp -File | Sort-Object Name
    $pyFiles = Get-ChildItem -Path $caseDir -Filter *.py -File | Sort-Object Name
    if (($cppFiles.Count + $pyFiles.Count) -eq 0) {
        throw "no runnable snippet files found in $caseDir"
    }

    New-Item -ItemType Directory -Path $logDir -Force | Out-Null
    if (Test-Path $logFile) {
        Remove-Item -Force $logFile
    }

    foreach ($sourceFile in $cppFiles) {
        $stem = [System.IO.Path]::GetFileNameWithoutExtension($sourceFile.Name)
        $binFile = Join-Path $tmpDir "$stem.exe"
        $compileResult = Invoke-NxppTimedCommand -FilePath $compiler -Arguments @("-std=c++20", "-Wall", "-Wextra", "-pedantic", "-O3", $sourceFile.FullName, "-I$root", "-o", $binFile)
        if ($compileResult.ExitCode -ne 0) {
            throw "compilation failed for $($sourceFile.Name)"
        }
        $runResult = Invoke-NxppTimedCommand -FilePath $binFile -StdInPath $stdinPath
        if ($runResult.ExitCode -ne 0) {
            throw "execution failed for $($sourceFile.Name)"
        }
        Add-Content -Path $logFile -Value $runResult.StdOut
        Add-Content -Path $logFile -Value ""
    }

    foreach ($sourceFile in $pyFiles) {
        $pyCommand, $pyArgs = if ($python -like "* -3") {
            @("py", @("-3", "-B", $sourceFile.FullName))
        } else {
            @($python, @("-B", $sourceFile.FullName))
        }
        $runResult = Invoke-NxppTimedCommand -FilePath $pyCommand -Arguments $pyArgs -StdInPath $stdinPath
        if ($runResult.ExitCode -ne 0) {
            throw "execution failed for $($sourceFile.Name)"
        }
        Add-Content -Path $logFile -Value $runResult.StdOut
        Add-Content -Path $logFile -Value ""
    }

    Get-Content -Path $logFile
}
finally {
    if (Test-Path $tmpDir) {
        Remove-Item -Recurse -Force $tmpDir
    }
}
