$source = "."
$target = "..\data"
$files = @("aceedit.htm","animation.js","cmedit.htm","config.htm","index.htm","info.htm","littlefsb.htm","pumpmode.htm","reboot.htm","update.htm","upload.htm","wifiOiler.css")
$count = 0

foreach ($file in $files) {
    $sourceFile = $source + "\" + $file
    $targetFile = $target + "\" + $file + ".gz"
    if ([System.IO.File]::Exists($sourceFile) -and (![System.IO.File]::Exists($targetFile) -or (Get-Item $sourceFile).LastWriteTime -gt (Get-Item $targetFile).LastWriteTime)) {
        "need update: " + $targetFile
        Copy-Item $sourceFile -Destination $target
        & .\_gzip.exe -f ($target + "\" + $file)
        (Get-Item $targetFile).LastWriteTime = (Get-Date)
        $count++
    }
}
if ($count -eq 0) {
    "All files up to date - nothing to do......"
} else {
    "{0} file(s) updated - done!" -f $count
}
if (!$psISE) {
    Write-Host "Press any key to continue..."
    [void][System.Console]::ReadKey($true)
}