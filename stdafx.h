// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here
enum FORMULA_TYPE {
	FT_CNF,
	FT_FOF
};

enum OPERATION_TYPE {
	OP_ATOM,
	OP_NOT,
	OP_OR,
	OP_AND,
	OP_IMPL,
	OP_EQ
};
