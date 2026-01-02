

mkdir dotnet
cd dotnet
$dotnet_url="https://download.visualstudio.microsoft.com/download/pr/5af098e1-e433-4fda-84af-3f54fd27c108/6bd1c6e48e64e64871957289023ca590/dotnet-sdk-8.0.302-win-x64.zip"
Invoke-WebRequest -Uri $dotnet_url -Outfile dotnet-sdk-8.0.302-win-x64.zip
Expand-Archive -LiteralPath .\dotnet-sdk-8.0.302-win-x64.zip
$Env:DOTNET_ROOT="$($(Get-Location).Path)\dotnet-sdk-8.0.302-win-x64"
$Env:PATH="$Env:DOTNET_ROOT;$Env:PATH"

$Env:DOTNET_ROOT="$($(Get-Location).Path)\dotnet\dotnet-sdk-8.0.302-win-x64"
$Env:PATH="$Env:DOTNET_ROOT;$Env:PATH"
$Env:DOTNET_SKIP_FIRST_TIME_EXPERIENCE=$true
dotnet tool install --global AzureSignTool
$Env:DOTNET_ROOT="$($(Get-Location).Path)\dotnet\dotnet-sdk-8.0.302-win-x64"
$Env:PATH="$Env:DOTNET_ROOT;$Env:PATH"

AzureSignTool.exe sign -du "$ENV{SIGNING_URL}" -kvu $ENV{KVU_URL} -kvi "$Env{AZSignGUID}" -kvs "$Env{AZSignPWD}" -kvc "$Env{AZSignCertName}" -kvt "$Env{AZSignTID}" -tr http://timestamp.digicert.com -v "GNU Radio-0.1.1-win64.msi"