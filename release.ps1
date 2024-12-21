$ReleaseName = "wishlair1"
$ReleaseTag = Get-Date -Format "yyyy-MM-dd"
$ReleaseId = "$ReleaseName-$ReleaseTag"

$SourcePath = "../NomadEngine/src"
$BuildPath = "./build"
$ReleasePath = "./release/$ReleaseId"

# Remove-Item -Path $BuildPath -Recurse -Force
# Remove-Item -Path $ReleasePath -Recurse -Force

New-Item -Path $BuildPath -ItemType Directory   
New-Item -Path $ReleasePath -ItemType Directory

cmake -DCMAKE_BUILD_TYPE=Release -S $SourcePath -B $BuildPath

cmake --build $BuildPath --config Release

Copy-Item -Path "$BuildPath\Release\*.dll" -Destination $ReleasePath -Force
Copy-Item -Path "$BuildPath\Release\nomad.exe" -Destination "$ReleasePath\$ReleaseName.exe" -Force
Copy-Item -Path "$BuildPath\licenses" -Destination "$ReleasePath" -Force
Copy-Item -Path "$BuildPath\LICENSE.txt" -Destination "$ReleasePath" -Force

Copy-Item -Path "./res/*" -Destination $ReleasePath -Recurse -Force

Compress-Archive -Path $ReleasePath -DestinationPath "$ReleaseId.zip" -Force