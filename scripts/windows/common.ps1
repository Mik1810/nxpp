$script:NxppRoot = $null

function Get-NxppRoot {
    if (-not $script:NxppRoot) {
        $script:NxppRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
    }
    return $script:NxppRoot
}

function Invoke-NxppPython {
    param(
        [Parameter(Mandatory = $true)]
        [string[]]$Arguments
    )

    $root = Get-NxppRoot
    $venvPython = Join-Path $root ".venv\Scripts\python.exe"

    if (Test-Path $venvPython) {
        & $venvPython @Arguments
        return $LASTEXITCODE
    }

    foreach ($candidate in @("python", "python3")) {
        $command = Get-Command $candidate -ErrorAction SilentlyContinue
        if ($command) {
            & $command.Source @Arguments
            return $LASTEXITCODE
        }
    }

    $py = Get-Command py -ErrorAction SilentlyContinue
    if ($py) {
        & $py.Source -3 @Arguments
        return $LASTEXITCODE
    }

    throw "No Python interpreter found."
}

function Get-NxppPythonPath {
    $root = Get-NxppRoot
    $venvPython = Join-Path $root ".venv\Scripts\python.exe"

    if (Test-Path $venvPython) {
        return $venvPython
    }

    foreach ($candidate in @("python", "python3")) {
        $command = Get-Command $candidate -ErrorAction SilentlyContinue
        if ($command) {
            return $command.Source
        }
    }

    $py = Get-Command py -ErrorAction SilentlyContinue
    if ($py) {
        return "$($py.Source) -3"
    }

    throw "No Python interpreter found."
}

function Get-NxppCompiler {
    if ($env:CXX) {
        return $env:CXX
    }

    foreach ($candidate in @("cl", "clang-cl", "clang++", "g++")) {
        $command = Get-Command $candidate -ErrorAction SilentlyContinue
        if ($command) {
            return $command.Source
        }
    }

    throw "No supported C++ compiler found. Set CXX explicitly."
}

function New-NxppTempDirectory {
    param(
        [string]$Prefix = "nxpp"
    )

    $base = Join-Path ([System.IO.Path]::GetTempPath()) ("{0}_{1}" -f $Prefix, [System.Guid]::NewGuid().ToString("N"))
    New-Item -ItemType Directory -Path $base -Force | Out-Null
    return $base
}

function New-NxppTimestamp {
    return (Get-Date -Format "yyyyMMdd_HHmmss")
}

function Resolve-NxppSnippetInput {
    param(
        [Parameter(Mandatory = $true)]
        [string]$SnippetName,
        [string]$UserInputPath = "",
        [string]$TempDirectory = ""
    )

    if ($UserInputPath) {
        if (-not (Test-Path $UserInputPath)) {
            throw "stdin file not found: $UserInputPath"
        }
        return (Resolve-Path $UserInputPath).Path
    }

    if ($SnippetName -eq "2sat") {
        if (-not $TempDirectory) {
            throw "TempDirectory is required for the 2sat auto-input path."
        }
        $autoInput = Join-Path $TempDirectory "$SnippetName.stdin.txt"
        @(
            "2 2"
            "1 2"
            "-1 2"
        ) | Set-Content -Path $autoInput
        return $autoInput
    }

    return ""
}

function Sanitize-NxppName {
    param([Parameter(Mandatory = $true)][string]$Value)
    return (($Value -replace '[\\/\.\s]', '_') -replace '_+', '_')
}

function Invoke-NxppTimedCommand {
    param(
        [Parameter(Mandatory = $true)]
        [string]$FilePath,
        [string[]]$Arguments = @(),
        [string]$StdOutPath = "",
        [string]$StdErrPath = "",
        [string]$StdInPath = ""
    )

    $psi = [System.Diagnostics.ProcessStartInfo]::new()
    $psi.FileName = $FilePath
    foreach ($arg in $Arguments) {
        [void]$psi.ArgumentList.Add($arg)
    }
    $psi.UseShellExecute = $false
    $psi.RedirectStandardOutput = $true
    $psi.RedirectStandardError = $true
    $psi.RedirectStandardInput = [bool]$StdInPath

    $process = [System.Diagnostics.Process]::new()
    $process.StartInfo = $psi

    $stopwatch = [System.Diagnostics.Stopwatch]::StartNew()
    [void]$process.Start()

    if ($StdInPath) {
        $inputText = Get-Content -Raw -Path $StdInPath
        $process.StandardInput.Write($inputText)
        $process.StandardInput.Close()
    }

    $stdOut = $process.StandardOutput.ReadToEnd()
    $stdErr = $process.StandardError.ReadToEnd()
    $process.WaitForExit()
    $stopwatch.Stop()

    if ($StdOutPath) {
        Set-Content -Path $StdOutPath -Value $stdOut -NoNewline
    }
    if ($StdErrPath) {
        Set-Content -Path $StdErrPath -Value $stdErr -NoNewline
    }

    return [PSCustomObject]@{
        ExitCode = $process.ExitCode
        Seconds  = [math]::Round($stopwatch.Elapsed.TotalSeconds, 2)
        StdOut   = $stdOut
        StdErr   = $stdErr
    }
}

function Get-NxppSnippetCppPairs {
    param(
        [Parameter(Mandatory = $true)]
        [string]$CaseDirectory,
        [Parameter(Mandatory = $true)]
        [string]$SnippetName
    )

    $preferredBaseline = Join-Path $CaseDirectory "$SnippetName.cpp"
    $preferredNxpp = Join-Path $CaseDirectory "${SnippetName}_nxpp.cpp"

    $cppFiles = Get-ChildItem -Path $CaseDirectory -Filter *.cpp -File | Sort-Object Name
    $baselineFile = $null
    $nxppFile = $null

    if (Test-Path $preferredBaseline) {
        $baselineFile = (Resolve-Path $preferredBaseline).Path
    }
    if (Test-Path $preferredNxpp) {
        $nxppFile = (Resolve-Path $preferredNxpp).Path
    }

    foreach ($file in $cppFiles) {
        if ($file.Name -like '*_nxpp.cpp') {
            if (-not $nxppFile) {
                $nxppFile = $file.FullName
            }
        } else {
            if (-not $baselineFile) {
                $baselineFile = $file.FullName
            }
        }
    }

    if (-not $baselineFile -or -not $nxppFile) {
        return $null
    }

    return [PSCustomObject]@{
        Baseline = $baselineFile
        Nxpp     = $nxppFile
    }
}

function Get-NxppMean {
    param([double[]]$Values)
    if (-not $Values -or $Values.Count -eq 0) { return 0.0 }
    return (($Values | Measure-Object -Average).Average)
}

function Get-NxppMedian {
    param([double[]]$Values)
    if (-not $Values -or $Values.Count -eq 0) { return 0.0 }
    $sorted = $Values | Sort-Object
    if ($sorted.Count % 2 -eq 1) {
        return $sorted[[int]($sorted.Count / 2)]
    }
    $upper = $sorted.Count / 2
    $lower = $upper - 1
    return (($sorted[$lower] + $sorted[$upper]) / 2.0)
}

function Get-NxppPercentOverhead {
    param(
        [double]$Baseline,
        [double]$Candidate
    )

    if ($Baseline -eq 0) {
        return $null
    }

    return (($Candidate - $Baseline) / $Baseline) * 100.0
}

function Format-NxppSeconds {
    param([double]$Value)
    return "{0:N3}" -f $Value
}

function Format-NxppPercent {
    param([Nullable[double]]$Value)
    if ($null -eq $Value) {
        return "n/a"
    }
    return ("{0:N2}%" -f $Value)
}

function Get-NxppCompilerKind {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Compiler
    )

    $name = [System.IO.Path]::GetFileName($Compiler).ToLowerInvariant()
    if ($name -eq "cl" -or $name -eq "cl.exe") {
        return "msvc"
    }
    if ($name -eq "clang-cl" -or $name -eq "clang-cl.exe") {
        return "clang-cl"
    }
    return "gnu"
}

function Get-NxppDefaultCxxFlags {
    param(
        [Parameter(Mandatory = $true)]
        [string]$CompilerKind,
        [string]$Optimization = "O0"
    )

    switch ($CompilerKind) {
        "msvc" {
            if ($Optimization -eq "O2") {
                return @("/std:c++20", "/W4", "/EHsc", "/O2")
            }
            return @("/std:c++20", "/W4", "/EHsc", "/Od")
        }
        "clang-cl" {
            if ($Optimization -eq "O2") {
                return @("/std:c++20", "/W4", "/EHsc", "/O2")
            }
            return @("/std:c++20", "/W4", "/EHsc", "/Od")
        }
        default {
            if ($Optimization -eq "O2") {
                return @("-std=c++20", "-Wall", "-Wextra", "-pedantic", "-O2")
            }
            return @("-std=c++20", "-Wall", "-Wextra", "-pedantic", "-O0")
        }
    }
}

function Convert-NxppFlagStringToArray {
    param([string]$Flags)

    if (-not $Flags) {
        return @()
    }

    return ($Flags -split '\s+' | Where-Object { $_ -ne "" })
}

function Get-NxppIncludeArgs {
    param(
        [Parameter(Mandatory = $true)]
        [string]$CompilerKind,
        [Parameter(Mandatory = $true)]
        [string[]]$IncludePaths
    )

    $args = New-Object System.Collections.Generic.List[string]
    foreach ($path in $IncludePaths) {
        if ($CompilerKind -eq "msvc" -or $CompilerKind -eq "clang-cl") {
            $args.Add("/I$path") | Out-Null
        } else {
            $args.Add("-I$path") | Out-Null
        }
    }
    return $args.ToArray()
}

function Get-NxppDefineArgs {
    param(
        [Parameter(Mandatory = $true)]
        [string]$CompilerKind,
        [Parameter(Mandatory = $false)]
        [hashtable]$Definitions
    )

    if (-not $Definitions) {
        return @()
    }

    $args = New-Object System.Collections.Generic.List[string]
    foreach ($key in $Definitions.Keys) {
        $value = $Definitions[$key]
        if ($CompilerKind -eq "msvc" -or $CompilerKind -eq "clang-cl") {
            if ($null -eq $value -or $value -eq "") {
                $args.Add("/D$key") | Out-Null
            } else {
                $args.Add("/D${key}=$value") | Out-Null
            }
        } else {
            if ($null -eq $value -or $value -eq "") {
                $args.Add("-D$key") | Out-Null
            } else {
                $args.Add("-D${key}=$value") | Out-Null
            }
        }
    }
    return $args.ToArray()
}

function Get-NxppOutputArgs {
    param(
        [Parameter(Mandatory = $true)]
        [string]$CompilerKind,
        [Parameter(Mandatory = $true)]
        [string]$OutputPath
    )

    if ($CompilerKind -eq "msvc" -or $CompilerKind -eq "clang-cl") {
        return @("/Fe:$OutputPath")
    }
    return @("-o", $OutputPath)
}

function Get-NxppExtraIncludePaths {
    $paths = New-Object System.Collections.Generic.List[string]

    if ($env:NXPP_EXTRA_INCLUDE) {
        foreach ($candidate in ($env:NXPP_EXTRA_INCLUDE -split ';')) {
            if ($candidate) {
                $paths.Add($candidate) | Out-Null
            }
        }
    }

    if ($env:BOOST_ROOT) {
        $paths.Add((Join-Path $env:BOOST_ROOT "include")) | Out-Null
    }

    return ($paths | Select-Object -Unique)
}

function Invoke-NxppCompileSource {
    param(
        [Parameter(Mandatory = $true)]
        [string]$SourceFile,
        [Parameter(Mandatory = $true)]
        [string]$OutputFile,
        [string]$Optimization = "O0",
        [hashtable]$Definitions = @{},
        [string]$Compiler = "",
        [string]$FlagsOverride = ""
    )

    $root = Get-NxppRoot
    $compilerPath = if ($Compiler) { $Compiler } else { Get-NxppCompiler }
    $compilerKind = Get-NxppCompilerKind -Compiler $compilerPath
    $flagArgs = if ($FlagsOverride) { Convert-NxppFlagStringToArray -Flags $FlagsOverride } else { Get-NxppDefaultCxxFlags -CompilerKind $compilerKind -Optimization $Optimization }
    $includePaths = @($root) + (Get-NxppExtraIncludePaths)
    $includeArgs = Get-NxppIncludeArgs -CompilerKind $compilerKind -IncludePaths $includePaths
    $defineArgs = Get-NxppDefineArgs -CompilerKind $compilerKind -Definitions $Definitions
    $outputArgs = Get-NxppOutputArgs -CompilerKind $compilerKind -OutputPath $OutputFile

    $arguments = New-Object System.Collections.Generic.List[string]
    foreach ($item in $flagArgs) { $arguments.Add($item) | Out-Null }
    foreach ($item in $includeArgs) { $arguments.Add($item) | Out-Null }
    foreach ($item in $defineArgs) { $arguments.Add($item) | Out-Null }
    $arguments.Add($SourceFile) | Out-Null
    foreach ($item in $outputArgs) { $arguments.Add($item) | Out-Null }

    return (Invoke-NxppTimedCommand -FilePath $compilerPath -Arguments $arguments.ToArray())
}

function Get-NxppTestSources {
    param([switch]$Smoke)

    $root = Get-NxppRoot
    if ($Smoke) {
        return @(
            (Join-Path $root "tests/test_core.cpp"),
            (Join-Path $root "tests/test_edge_cases.cpp")
        )
    }

    return @(
        (Join-Path $root "tests/test_core.cpp"),
        (Join-Path $root "tests/test_attributes.cpp"),
        (Join-Path $root "tests/test_centrality.cpp"),
        (Join-Path $root "tests/test_edge_cases.cpp"),
        (Join-Path $root "tests/test_flow.cpp"),
        (Join-Path $root "tests/test_remove_node.cpp"),
        (Join-Path $root "tests/test_multigraph.cpp")
    )
}

function Get-NxppPassFailCounts {
    param([string]$OutputText)

    $matches = [regex]::Matches($OutputText, '^\[TEST\].*\| .*?(PASS|FAIL)', [System.Text.RegularExpressions.RegexOptions]::Multiline)
    $passMatches = [regex]::Matches($OutputText, '^\[TEST\].*\| .*?PASS', [System.Text.RegularExpressions.RegexOptions]::Multiline)

    return [PSCustomObject]@{
        Total  = $matches.Count
        Passed = $passMatches.Count
    }
}
