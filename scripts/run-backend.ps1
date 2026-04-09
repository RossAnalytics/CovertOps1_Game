param(
    [string]$ProjectRoot = (Resolve-Path "$PSScriptRoot\..").Path
)

$ErrorActionPreference = "Stop"

Push-Location (Join-Path $ProjectRoot "services\backend")
try {
    if (!(Test-Path "node_modules")) {
        npm install
    }
    npm run dev
}
finally {
    Pop-Location
}
