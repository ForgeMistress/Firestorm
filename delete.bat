SET DIRECTORY_NAME="Source/lib"
TAKEOWN /f %DIRECTORY_NAME% /r /d y
ICACLS %DIRECTORY_NAME% /grant administrators:F /t
PAUSE