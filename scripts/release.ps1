[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12;
# Find the Qt tools install directory
$qtToolsDir = $ENV:QT_TOOLS
if(-not $qtToolsDir)
{
    $qtToolsDir = "c:\Qt\Tools"
}
if(-not $qtToolsDir -or -not (Test-Path $qtToolsDir))
{
    Write-Error("Could not find Qt tools in $qtToolsDir. You must install Qt Tools and set the " + 
        "QT_TOOLS environment variable if you did not choose the default install location C:\Qt\Tools.");
    Exit 1
}

# Find the Visual Studio install. Check if the MSBUILD environment variable is set.
$msBuildPath = $ENV:MSBUILD
if(-not $msBuildPath -or -not (Test-Path $msBuildPath))
{
    # It's not set, so find vswhere.exe. Try the VSWHERE environment vairiable first, then try the
    # default install location if that doesn't work.
    $vsWherePath = $ENV:VSWHdERE
    if(-not $vsWherePath)
    {
        $vsWherePath = Join-Path ${ENV:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
    }
    if(Test-Path $vsWherePath)
    {
        $msBuildPath = (&"$vsWherePath" -latest -products * -requires Microsoft.Component.MSBuild -property installationPath)
        $msBuildPath += "\MSBuild\15.0\Bin\MSBuild.exe"
    }
}
if(-not (Test-Path $msBuildPath))
{
    Write-Error("Could not determine the install location of Visual Studio. You must install" +
        "Visual Studio 2017 in the default location or ensure that the path to msbuild.exe can " + 
        "be found in the MSBUILD environment variable.");
    Exit 1
}

# Collect the GitHub username and password from the user.
$username = Read-Host "GitHub Username: "
$password = Read-Host "GitHub Password: "

# Figure out what version we want to bump, (major, minor, version)
$bumpVer = Read-Host "Which version do you want to bump, 0-major, 1-minor, 2-patch? [2]"
if(-not $bumpVer)
{
    $bumpVer = 2
}
$version = &"$PSScriptRoot\..\getversion.bat"
$verTokens = $version.split('.')
$verTokens[$bumpVer] = [int]$verTokens[$bumpVer] + 1;
$newVer = [string]::Join(".", $verTokens)

# Make sure the code is built with the new version.
$ENV:FORCE_VERSION = $newVer
$updateVerScript = Join-Path $PSScriptRoot "..\updateversion.bat"
&"$updateVerScript"

# Build the solution with msbuild.
Write-Host("### Building solution");
&"$msBuildPath"  "$PSScriptRoot\..\PoePal.sln" /property:Configuration=Release -verbosity:minimal -nologo

# Find the Qt directory from the .user file in one of the projects
$xmlFile = "$PSScriptRoot\..\PoePal\PoePal.vcxproj.user"
[xml]$userFileXml = Get-Content $xmlFile
$msbuildNamespace = @{msbuild = "http://schemas.microsoft.com/developer/msbuild/2003"}
$qtDir = ($userFileXml | Select-Xml -XPath "//msbuild:QTDIR" -Namespace $msbuildNamespace)[0]

if(-not (Test-Path $qtDir))
{
    Write-Error("Could not find the QT_DIR from Visual Studio Projects. Make sure you" + 
        "can build the solution and that PoePal\PoePal.vcxproj.user exists and has" + 
        "QTDIR defined in it.");
    Exit 1
}
$installerDir = "$PSScriptRoot\..\installer";
foreach($line in Get-Content "$installerDir\config\qtbinaries.txt")
{
    Copy-Item "$qtDir\bin\$line" "$installerDir\packages\com.PoePal.PoePal\data"
}
foreach($line in Get-Content "$installerDir\config\poepalbinaries.txt")
{
    Copy-Item "$PSScriptRoot\..\x64\Release\$line" "$installerDir\packages\com.PoePal.PoePal\data"
}

# Replace the date and version in the installer configuration files and release notes.
(Get-Content "$installerDir\config\config.template.xml").Replace("0.0.0", $newVer) | Set-Content "$installerDir\config\config.xml"
(Get-Content "$installerDir\packages\com.PoePal.PoePal\meta\package.template.xml").Replace("0.0.0-0", "$newVer-1").Replace("0000-00-00", (Get-Date -UFormat "%Y-%m-%d")) | Set-Content "$installerDir\packages\com.PoePal.PoePal\meta\package.xml"
(Get-Content "$installerDir\ReleaseNotesTemplate.txt").Replace("0.0.0", $newVer) | Set-Content("$installerDir\ReleaseNotes.txt")

# Build the installer binary package.
Write-Host("### Building installer");
&"$qtToolsDir\QtInstallerFramework\3.0\bin\binarycreator.exe" -c installer\config\config.xml -p installer\packages "PoePal-v$newVer.exe"

# Read the git comments since the last release and find all issues mentioned.
$gitCommits = &"git" log "v$version..HEAD" --decorate --pretty=%s%n%b

Write-Host("### Assembling relevant tickets for release notes")
foreach($ticket in (($gitCommits | Select-String -Pattern "#(\d+)" -AllMatches).Matches.Value | Sort-Object))
{
    $ticketNum = $ticket.Substring(1);
    try{
        $ticketJson = Invoke-RestMethod -Method Get -Uri "https://api.github.com/repos/PoePal/PoePal/issues/$ticketNum" -ErrorAction SilentlyContinue
        Add-Content "$installerDir\ReleaseNotes.txt" ("`n * $ticket : " + $ticketJson.title)
    }
    catch
    {
    } 
}

Write-Host("### Editing release notes")
notepad.exe "$installerDir\ReleaseNotes.txt" | Out-Null

$data = Get-Content "$installerDir\ReleaseNotes.txt" | Out-String
Set-Location("$PSScriptRoot/..");
$hash = &"git" rev-parse HEAD

$release = @{
    tag_name="v$newVer"
    name="PoePal v$newVer"
    body= $data
    target_commitish="$hash"
    draft=$true
}
$json = $release | ConvertTo-Json
Write-Host($json)
$response = Read-Host "Does the above release look correct? [y]"
if($response -ne "y" -and $response)
{
    Exit 0
}
$base64AuthInfo = [Convert]::ToBase64String([Text.Encoding]::ASCII.GetBytes(("{0}:{1}" -f $username,$password)))
Write-Host("### Creating release on GitHub");
$result = Invoke-RestMethod -Headers @{Authorization=("Basic {0}" -f $base64AuthInfo)} -Method POST -Uri "https://api.github.com/repos/PoePal/PoePal/releases" -Body $json -ContentType 'application/json'
Write-Host("### Uploading installer");
$uploadUrl = $result.upload_url.split('{')[0] + "?name=PoePal-v$newVer.exe"
$uploadRes = Invoke-RestMethod -Headers @{Authorization=("Basic {0}" -f $base64AuthInfo)} -Method POST -Uri $uploadUrl -ContentType 'application/vnd.microsoft.portable-executable' -Infile "PoePal-v$newVer.exe"
$release.draft = $false
$json = $release | ConvertTo-Json
$result = Invoke-RestMethod -Headers @{Authorization=("Basic {0}" -f $base64AuthInfo)} -Method POST -Uri $result.url -Body $json -ContentType 'application/json'
Start-Process -FilePath $result.html_url
