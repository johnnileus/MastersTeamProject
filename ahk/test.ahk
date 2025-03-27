


SetTitleMatchMode 2

w:= SysGet(61)
h:= SysGet(62)

count := 0

loop 4{
	Run "..\x64\Debug\CSC8503.exe"
}
WinWaitActive("Go Marble Ball",,4)
Sleep(500)

IDs := WinGetList("Go Marble Ball")



loop 4{
	
	WinWait("Go Marble Ball")

	X := Mod(count, 2)
	Y := count // 2
	WinMove X * w / 2, Y * h / 2
	WinMinimize
	count++
}

for id in ids {
	WinRestore id
}