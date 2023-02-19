$source = "."
$target = "..\data"

Get-ChildItem $source | 
Foreach-Object {
    $targetFile = $target + "\" + $_.Name + ".gz"
    $sourceFile = $source + "\" + $_.Name
    $copyTarget = $target + "\" + $_.Name
    if([System.IO.File]::Exists($targetFile)) {
        # Datum vergleichen
        if ((Get-Item $sourceFile).LastWriteTime -gt (Get-Item $targetFile).LastWriteTime) {
            "muss neu geshrinkt werden..." + $copyTarget
            Copy-Item $sourceFile -Destination $target
            & .\_gzip.exe -f $copyTarget
            (Get-Item $targetFile).LastWriteTime = (Get-Date)
        }
    }
}