$script = Join-Path $PSScriptRoot "..\getversion.bat"
if(!$ENV:FORCE_VERSION)
{
    $version = &"$script"
}
else
{
    $version = $ENV:FORCE_VERSION
}

$verFile = Join-Path $PSScriptRoot "..\PoePal\GeneratedFiles\version.h"
$newVerString = "#define __VERSION__ `"$version`"";
if(Test-Path $verFile)
{
    $verFileContents = Get-Content $verFile | Out-String
    if($verFileContents.StartsWith("$newVerString"))
    {
        Write-Host "Version has not changed, ignoring";
        Exit 0
    }
}
Write-Host  "Version different, updating"
Set-Content $verFile "$newVerString"
$verTokens = $version.split('.');
$maj = $verTokens[0];
$min = $verTokens[1];
$pat = $verTokens[2];
Add-Content $verFile "#define __VERARRAY__ $maj,$min,$pat,0"
