#pragma once


#include "../../../../../Common/Common/common.h"
using namespace common;
#include "../../../../../Common/Graphic11/graphic11.h"
#include "../../../../../Common/Framework11/framework11.h"


#define V_RETURN(x) (x)
#define V(x) (x)
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
