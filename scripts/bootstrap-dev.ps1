param(
    [string]$ProjectRoot = (Resolve-Path "$PSScriptRoot\..").Path
)

$ErrorActionPreference = "Stop"

Write-Host "Bootstrapping CovertOps1 development environment..."
Write-Host "Project root: $ProjectRoot"

$backendPath = Join-Path $ProjectRoot "services\backend"
if (Test-Path $backendPath) {
    Push-Location $backendPath
    try {
        if (!(Test-Path ".env")) {
            Copy-Item ".env.example" ".env"
            Write-Host "Created backend .env from template."
        }
        npm install
    }
    finally {
        Pop-Location
    }
}

Write-Host "Bootstrap complete."
