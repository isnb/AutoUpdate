taskkill /f /t /im YS.Sync.WinUI.exe
wmic process where name='YS.Sync.WinUI.exe' call terminate

ping -n 1 127.0.0.1 

net stop YSSYNC

ping -n 1 127.0.0.1 

copy /v /y  %1update\*.* %1



:start
net start YSSYNC
