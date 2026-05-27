param(
    [string]$InputDir = "external/monaco/vs",
    [string]$Output = "generated/embedded_monaco.h"
)

$InputDir = (Resolve-Path $InputDir).Path

$outputDir = Split-Path -Parent $Output
if ($outputDir -and !(Test-Path $outputDir)) {
    New-Item -ItemType Directory -Force -Path $outputDir | Out-Null
}

$files = Get-ChildItem -Path $InputDir -Recurse -File | Sort-Object FullName

$sb = [System.Text.StringBuilder]::new()

[void]$sb.AppendLine("#pragma once")
[void]$sb.AppendLine("#include <cstddef>")
[void]$sb.AppendLine("")
[void]$sb.AppendLine("struct EmbeddedAsset")
[void]$sb.AppendLine("{")
[void]$sb.AppendLine("    const wchar_t* path;")
[void]$sb.AppendLine("    const unsigned char* data;")
[void]$sb.AppendLine("    size_t size;")
[void]$sb.AppendLine("};")
[void]$sb.AppendLine("")

$index = 0

foreach ($file in $files) {
    $relative = $file.FullName.Substring($InputDir.Length).TrimStart('\', '/')
    $relative = $relative.Replace('\', '/')
    $urlPath = "/" + $relative

    $bytes = [System.IO.File]::ReadAllBytes($file.FullName)

    [void]$sb.AppendLine("inline constexpr unsigned char monaco_asset_$index[] = {")

    for ($i = 0; $i -lt $bytes.Length; $i += 16) {
        $lineBytes = @()

        for ($j = $i; $j -lt [Math]::Min($i + 16, $bytes.Length); $j++) {
            $lineBytes += ("0x{0:X2}" -f $bytes[$j])
        }

        [void]$sb.AppendLine("    " + ($lineBytes -join ", ") + ",")
    }

    [void]$sb.AppendLine("};")
    [void]$sb.AppendLine("")

    $index++
}

[void]$sb.AppendLine("inline constexpr EmbeddedAsset kEmbeddedAssets[] = {")

$index = 0

foreach ($file in $files) {
    $relative = $file.FullName.Substring($InputDir.Length).TrimStart('\', '/')
    $relative = $relative.Replace('\', '/')
    $urlPath = "/" + $relative

    [void]$sb.AppendLine("    { L`"$urlPath`", monaco_asset_$index, sizeof(monaco_asset_$index) },")

    $index++
}

[void]$sb.AppendLine("};")
[void]$sb.AppendLine("")
[void]$sb.AppendLine("inline constexpr size_t kEmbeddedAssetCount = sizeof(kEmbeddedAssets) / sizeof(kEmbeddedAssets[0]);")

[System.IO.File]::WriteAllText($Output, $sb.ToString(), [System.Text.Encoding]::UTF8)

Write-Host "Embedded $($files.Count) Monaco files into $Output"