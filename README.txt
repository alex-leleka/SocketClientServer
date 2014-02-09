Socket server design as a Windows Service

External dependencies - tinyxml2

Installing service:
sc create ServerWinService binPath= "C:\Users\User01\Documents\Visual Studio 2012\Projects\ServerWinService\Debug\ServerWinService.exe" DisplayName= "ServerWinService" start= auto

Uninstalling service:
sc delete ServerWinService

Starting service:
open service manager (services.msc) -> find service Name and manually start service

Stop service:
Open service manager -> find service Name and manually stop service