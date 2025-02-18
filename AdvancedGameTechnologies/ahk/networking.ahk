
clients:= 3

^g::
{
	ids := WinGetList("Go Marble Ball")
	for id in ids {
		WinClose id
	}
}

^j::{
Global clients
clients++
}
^k::{
Global clients
clients--
}

^H::{


SetTitleMatchMode 2

w:= SysGet(61)
h:= SysGet(62)

count := 1

loop clients{
	Run "..\x64\Debug\CSC8503.exe"
}
WinWaitActive("Go Marble Ball",,4)
Sleep(100)

ids := WinGetList("Go Marble Ball")

loop clients{
	
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
}
