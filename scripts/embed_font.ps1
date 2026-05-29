param(
    [Parameter(Mandatory = $true)]
    [string]$InputFile,

    [Parameter(Mandatory = $true)]
    [string]$Output
)

if (!(Test-Path $InputFile)) {
    throw "Font file does not exist: $InputFile"
}

$resolvedInput = (Resolve-Path $InputFile).Path
$bytes = [System.IO.File]::ReadAllBytes($resolvedInput)

if ($bytes.Length -le 0) {
    throw "Font file is empty: $resolvedInput"
}

$outputDir = Split-Path -Parent $Output

if ($outputDir -and !(Test-Path $outputDir)) {
    New-Item -ItemType Directory -Force -Path $outputDir | Out-Null
}

$sb = [System.Text.StringBuilder]::new()

[void]$sb.AppendLine("#pragma once")
[void]$sb.AppendLine("#include <cstddef>")
[void]$sb.AppendLine("")
[void]$sb.AppendLine("inline constexpr unsigned char kJetBrainsMonoRegular[$($bytes.Length)] = {")

for ($i = 0; $i -lt $bytes.Length; $i += 16) {
    $lineBytes = @()

    for ($j = $i; $j -lt [Math]::Min($i + 16, $bytes.Length); $j++) {
        $lineBytes += ("0x{0:X2}" -f $bytes[$j])
    }

    [void]$sb.AppendLine("    " + ($lineBytes -join ", ") + ",")
}

[void]$sb.AppendLine("};")
[void]$sb.AppendLine("")
[void]$sb.AppendLine("inline constexpr std::size_t kJetBrainsMonoRegularSize = sizeof(kJetBrainsMonoRegular);")

[System.IO.File]::WriteAllText($Output, $sb.ToString(), [System.Text.Encoding]::UTF8)

Write-Host "Embedded font:"
Write-Host "  Input:  $resolvedInput"
Write-Host "  Output: $Output"
Write-Host "  Bytes:  $($bytes.Length)"