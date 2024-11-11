GLOBAL readCaller
GLOBAL writeCaller
GLOBAL printRectangleCaller
GLOBAL getScreenWidthCaller
GLOBAL getScreenHeightCaller
GLOBAL printCharCaller
GLOBAL waitCaller
GLOBAL keyboardStatusCaller
GLOBAL keyboardKeyCaller
GLOBAL beepSysCaller
GLOBAL fontSizeUpCaller
GLOBAL fontSizeDownCaller
GLOBAL getTimeCaller
GLOBAL allocMCaller
GLOBAL freeMCaller
GLOBAL createProcCaller
GLOBAL exitProcCaller
GLOBAL getPIDCaller
GLOBAL killProcCaller
GLOBAL changeProcessPriorityCaller
GLOBAL blockProcessCaller
GLOBAL unblockProcessCaller
GLOBAL yieldProcessCaller
GLOBAL waitPIDCaller
GLOBAL openSemCaller
GLOBAL closeSemCaller
GLOBAL waitSemCaller
GLOBAL postSemCaller
GLOBAL memStatusCaller
GLOBAL listProcessesInfoCaller
GLOBAL createStandardProcCaller
GLOBAL getRunningOutputPipeCaller
GLOBAL getRunningInputPipeCaller
GLOBAL newPipeCaller
GLOBAL clearTerminalCaller
GLOBAL setForegroundProcessCaller

section .text

%macro syscaller 1
    push rbp
    mov rbp, rsp

    mov rdi, %1
    int 80h

    mov rsp, rbp
    pop rbp 
    ret
%endmacro


readCaller:
    syscaller 0
writeCaller:
    syscaller 1
printRectangleCaller:
    syscaller 2
getScreenWidthCaller:
    syscaller 3
getScreenHeightCaller:
    syscaller 4
printCharCaller:
    syscaller 5
waitCaller:
    syscaller 6
keyboardStatusCaller:
    syscaller 7
keyboardKeyCaller:
    syscaller 8

beepSysCaller:
    syscaller 9

fontSizeUpCaller:
    syscaller 10

fontSizeDownCaller:
    syscaller 11
getTimeCaller:
    syscaller 12

allocMCaller:
    syscaller 13
freeMCaller:
    syscaller 14
createProcCaller:
    syscaller 15
exitProcCaller:
    syscaller 16
getPIDCaller:
    syscaller 17
killProcCaller:
    syscaller 18
changeProcessPriorityCaller:
    syscaller 19
blockProcessCaller:
    syscaller 20
unblockProcessCaller:
    syscaller 21
yieldProcessCaller:
    syscaller 22
waitPIDCaller:
    syscaller 23

openSemCaller:
    syscaller 24
closeSemCaller:
    syscaller 25
waitSemCaller:
    syscaller 26
postSemCaller:
    syscaller 27

memStatusCaller:
    syscaller 28
listProcessesInfoCaller:
    syscaller 29
createStandardProcCaller:
    syscaller 30
getRunningOutputPipeCaller:
    syscaller 31
getRunningInputPipeCaller:
    syscaller 32
newPipeCaller:
    syscaller 33
clearTerminalCaller:
    syscaller 34
setForegroundProcessCaller:
    syscaller 35