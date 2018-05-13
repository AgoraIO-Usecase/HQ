@echo off 
@ping 127.0.0.1 -n 2 >nul 
taskkill /f /im AgoraHQ.exe
start AgoraHQ.exe