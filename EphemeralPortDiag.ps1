#USAGE
# .\EphemeralPortDiag.ps1 chrome

param (
    [Parameter(Mandatory = $true)]
    [string]$procName
)

# Get process ID(s)
$proc = Get-Process -Name $procName -ErrorAction SilentlyContinue
if (-not $proc) {
    Write-Host "Process '$procName' not found."
    exit
}
$pids = $proc.Id

# Get ephemeral port range
$ephemeral = netsh int ipv4 show dynamicport tcp | Select-String "Start Port|Number of Ports"
$startPort = ($ephemeral[0] -split ":")[1].Trim()
$portCount = ($ephemeral[1] -split ":")[1].Trim()
$maxPort = [int]$startPort + [int]$portCount - 1

Write-Host "Ephemeral Port Range: $startPort to $maxPort ($portCount ports)"

# Gather all TCP connections
$conns = Get-NetTCPConnection

# Group by state
$stateSummary = $conns | Group-Object State | Sort-Object Count -Descending
Write-Host "`nTCP Connection States:"
foreach ($group in $stateSummary) {
    $count = $group.Count
    $state = $group.Name
    Write-Host "$state : $count"
}

# Show connections owned by the target process
$owned = $conns | Where-Object { $pids -contains $_.OwningProcess }
$byState = $owned | Group-Object State | Sort-Object Count -Descending

Write-Host "`nConnections owned by '$procName' (PID $($pids -join ', ')):"
foreach ($group in $byState) {
    Write-Host "$($group.Name) : $($group.Count)"
}

# Check ephemeral port exhaustion
$usedEphemeral = $conns | Where-Object {
    ($_."LocalPort" -ge $startPort) -and ($_."LocalPort" -le $maxPort)
}
$usedCount = $usedEphemeral.Count
$usagePercent = [math]::Round(($usedCount / $portCount) * 100, 2)

Write-Host "`nEphemeral Port Usage: $usedCount / $portCount ($usagePercent`%)"
if ($usagePercent -gt 80) {
    Write-Warning "WARNING: Ephemeral ports > 80% used. System may be nearing port exhaustion!"
}
