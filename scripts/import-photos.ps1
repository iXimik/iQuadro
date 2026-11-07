# Import originals from a folder into the local checkout. Run AFTER downloading photos.
# Usage: .\scripts\import-photos.ps1 -Source "C:\Users\YOU\Downloads\iQuadro-photos"
param([Parameter(Mandatory=$true)][string]$Source)
$ErrorActionPreference = 'Stop'
$Repo = Split-Path -Parent $PSScriptRoot
$Target = Join-Path $Repo 'photos'
New-Item -ItemType Directory -Path $Target -Force | Out-Null
$ExtensionList = @('*.jpg','*.jpeg','*.png')
$count = 0
foreach ($mask in $ExtensionList) {
  Get-ChildItem -LiteralPath $Source -Filter $mask -File |
    ForEach-Object {
      Copy-Item -LiteralPath $_.FullName -Destination (Join-Path $Target $_.Name) -Force
      $count++
    }
}
Write-Host "Imported $count original photos into $Target."
Write-Host "Check files, then use: git add photos; git commit -m 'Add original iQuadro repair photos'; git push"
