
wxMemMonitorLib
----------------


wxWidgets Setting
---------


Usage:
---------

add dia library path

config json file example
```sh
{
	"pdbpath" : "../Debug/RenderLight2.pdb",
	"sharedmemoryname" : "DXSAMPLE"
}
```




```sh
#include "../wxMemMonitorLib/wxMemMonitor.h"

MEMORYMONITOR_INNER_PROCESS();

if (!memmonitor::Init(memmonitor::INNER_PROCESS,hInstance,"config_target.json" ))
{
    MessageBoxA(NULL, memmonitor::GetLastError().c_str(), "ERROR", MB_OK);
}

DECLARE_TYPE_NAME(sGlobal)
struct sGlobal : public memmonitor::Monitor<sGlobal, TYPE_NAME(sGlobal)>
{
	int value1;
	int value2;
};

~~
~~
memmonitor::Cleanup();
```

* ambigious funciton 'floor' 
	- modify -> floor to ::floor (in wxWidgets/geometry.h)



