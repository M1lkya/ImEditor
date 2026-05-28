param(
    [string]$InputDir = "assets",
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
[void]$sb.AppendLine("    const wchar_t* mime;")
[void]$sb.AppendLine("    const unsigned char* data;")
[void]$sb.AppendLine("    size_t size;")
[void]$sb.AppendLine("};")
[void]$sb.AppendLine("")

function Get-MimeType($path) {
    $ext = [System.IO.Path]::GetExtension($path).ToLowerInvariant()

    switch ($ext) {
        ".js"    { return "application/javascript" }
        ".mjs"   { return "application/javascript" }
        ".css"   { return "text/css" }
        ".html"  { return "text/html" }
        ".json"  { return "application/json" }
        ".wasm"  { return "application/wasm" }
        ".ttf"   { return "font/ttf" }
        ".woff"  { return "font/woff" }
        ".woff2" { return "font/woff2" }
        ".svg"   { return "image/svg+xml" }
        ".png"   { return "image/png" }
        ".jpg"   { return "image/jpeg" }
        ".jpeg"  { return "image/jpeg" }
        default  { return "application/octet-stream" }
    }
}

$index = 0

foreach ($file in $files) {
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
    $mime = Get-MimeType $file.FullName

    [void]$sb.AppendLine("    { L`"$urlPath`", L`"$mime`", monaco_asset_$index, sizeof(monaco_asset_$index) },")
    $index++
}

[void]$sb.AppendLine("};")
[void]$sb.AppendLine("")
[void]$sb.AppendLine("inline constexpr size_t kEmbeddedAssetCount = sizeof(kEmbeddedAssets) / sizeof(kEmbeddedAssets[0]);")

[System.IO.File]::WriteAllText($Output, $sb.ToString(), [System.Text.Encoding]::UTF8)

Write-Host "Embedded $($files.Count) Monaco files into $Output"