Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

param(
    [Parameter(Mandatory = $true, Position = 0)]
    [string]$SnippetFolder,
    [Parameter(Position = 1)]
    [string]$StdinFile = ""
)

. "$PSScriptRoot/common.ps1"

$root = Get-NxppRoot
$snippetRoot = Join-Path $root "snippet"
$logRoot = Join-Path $root "logs\snippet"
$timestamp = New-NxppTimestamp
$caseDir = Join-Path $snippetRoot $SnippetFolder
$runDir = Join-Path $logRoot "${SnippetFolder}_$timestamp"
$tmpDir = New-NxppTempDirectory -Prefix "nxpp_snippet"

try {
    if (-not (Test-Path $caseDir)) {
        throw "snippet folder not found: $caseDir"
    }

    New-Item -ItemType Directory -Path $runDir -Force | Out-Null
    $logFile = Join-Path $runDir "run.log"
    $compiler = Get-NxppCompiler
    $python = Get-NxppPythonPath
    $stdinPath = Resolve-NxppSnippetInput -SnippetName $SnippetFolder -UserInputPath $StdinFile -TempDirectory $tmpDir

    function Write-Log([string]$Message) {
        $Message | Tee-Object -FilePath $logFile -Append
    }

    $cppFiles = Get-ChildItem -Path $caseDir -Filter *.cpp -File | Sort-Object Name
    $pyFiles = Get-ChildItem -Path $caseDir -Filter *.py -File | Sort-Object Name
    if (($cppFiles.Count + $pyFiles.Count) -lt 2) {
        throw "need at least two runnable implementations in $caseDir"
    }

    $labels = New-Object System.Collections.Generic.List[string]
    $outputs = New-Object System.Collections.Generic.List[string]

    Write-Log "NXPP snippet implementation test"
    Write-Log "Snippet: $SnippetFolder"
    Write-Log "Case dir: $caseDir"
    Write-Log "Log dir: $runDir"
    Write-Log "Python: $python"
    if ($stdinPath) {
        Write-Log "stdin: $stdinPath"
    } else {
        Write-Log "stdin: <none>"
    }

    foreach ($sourceFile in $cppFiles) {
        $localName = $sourceFile.Name
        $stem = [System.IO.Path]::GetFileNameWithoutExtension($localName)
        $binFile = Join-Path $tmpDir "$stem.exe"
        $compileErr = Join-Path $runDir "$localName.compile.err"
        $stdoutFile = Join-Path $runDir "$localName.out"
        $stderrFile = Join-Path $runDir "$localName.err"

        Write-Log ""
        Write-Log "Compiling $localName"
        Write-Log "CMD: $compiler -std=c++20 -Wall -Wextra -pedantic -O3 $($sourceFile.FullName) -I$root -o $binFile"

        $compileResult = Invoke-NxppTimedCommand -FilePath $compiler -Arguments @("-std=c++20", "-Wall", "-Wextra", "-pedantic", "-O3", $sourceFile.FullName, "-I$root", "-o", $binFile) -StdErrPath $compileErr
        if ($compileResult.ExitCode -ne 0) {
            throw "compilation failed for $localName"
        }
        Write-Log ("TIME compile {0}: {1:0.00}s" -f $localName, $compileResult.Seconds)
        Write-Log "compile stderr: $compileErr"

        Write-Log ""
        Write-Log "Running $localName"
        $runResult = Invoke-NxppTimedCommand -FilePath $binFile -StdOutPath $stdoutFile -StdErrPath $stderrFile -StdInPath $stdinPath
        if ($runResult.ExitCode -ne 0) {
            throw "execution failed for $localName"
        }
        Write-Log ("TIME {0}: {1:0.00}s" -f $localName, $runResult.Seconds)
        Write-Log "stdout: $stdoutFile"
        Write-Log "stderr: $stderrFile"

        $labels.Add($localName) | Out-Null
        $outputs.Add($stdoutFile) | Out-Null
    }

    foreach ($sourceFile in $pyFiles) {
        $localName = $sourceFile.Name
        $stdoutFile = Join-Path $runDir "$localName.out"
        $stderrFile = Join-Path $runDir "$localName.err"

        Write-Log ""
        Write-Log "Running $localName"
        $pyCommand, $pyArgs = if ($python -like "* -3") {
            @("py", @("-3", "-B", $sourceFile.FullName))
        } else {
            @($python, @("-B", $sourceFile.FullName))
        }
        $runResult = Invoke-NxppTimedCommand -FilePath $pyCommand -Arguments $pyArgs -StdOutPath $stdoutFile -StdErrPath $stderrFile -StdInPath $stdinPath
        if ($runResult.ExitCode -ne 0) {
            throw "execution failed for $localName"
        }
        Write-Log ("TIME {0}: {1:0.00}s" -f $localName, $runResult.Seconds)
        Write-Log "stdout: $stdoutFile"
        Write-Log "stderr: $stderrFile"

        $labels.Add($localName) | Out-Null
        $outputs.Add($stdoutFile) | Out-Null
    }

    Write-Log ""
    Write-Log "Comparing outputs"

    $allOk = $true
    for ($i = 0; $i -lt $labels.Count; $i++) {
        for ($j = $i + 1; $j -lt $labels.Count; $j++) {
            $leftLabel = $labels[$i]
            $rightLabel = $labels[$j]
            $leftOutput = Get-Content -Raw -Path $outputs[$i]
            $rightOutput = Get-Content -Raw -Path $outputs[$j]
            $diffFile = Join-Path $runDir ("diff_{0}_vs_{1}.diff" -f (Sanitize-NxppName $leftLabel), (Sanitize-NxppName $rightLabel))

            if ($leftOutput -eq $rightOutput) {
                Write-Log "OK compare: $leftLabel matches $rightLabel"
            } else {
                $allOk = $false
                $leftOutput | Set-Content -Path "$diffFile.left"
                $rightOutput | Set-Content -Path "$diffFile.right"
                @(
                    "--- $leftLabel"
                    "+++ $rightLabel"
                    "Outputs differ. See $($diffFile.left) and $($diffFile.right)."
                ) | Set-Content -Path $diffFile
                Write-Log "FAIL compare: $leftLabel differs from $rightLabel"
                Write-Log "Saved diff: $diffFile"
            }
        }
    }

    Write-Log ""
    Write-Log "Artifacts saved in: $runDir"

    if ($allOk) {
        Write-Log "RESULT: PASS"
        exit 0
    }

    Write-Log "RESULT: FAIL"
    exit 1
}
finally {
    if (Test-Path $tmpDir) {
        Remove-Item -Recurse -Force $tmpDir
    }
}
