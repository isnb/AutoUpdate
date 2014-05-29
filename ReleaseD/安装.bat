@echo off

"%SystemRoot%\Microsoft.NET\Framework\v4.0.30319\InstallUtil.exe"  Sync.exe

net start "YSSYNC"

pause
