#include "CErrors.h"

CErrors::OpMap CErrors::errors = { 
	{ Token((int) LEXERR, (int)LINE_TOO_LONG), "Line has too many chars" },
	{ Token((int) LEXERR, (int)ID_TOO_LONG), "ID has too many chars" },
	{ Token((int)LEXERR, (int) HAS_LEADING_0), "NUM has  a leading 0." },
	{ Token((int) LEXERR, (int)INT_TOO_LONG), "INT is too long" },
	{ Token((int)LEXERR, (int)HAS_ENDING_0), "DEC has ending 0" },	
	{ Token((int) LEXERR, (int)DEC_TOO_LONG), "Dec part too long" },
	{ Token((int)LEXERR, (int)EXP_TOO_LONG), "Exp part too long" },
	{ Token((int)LEXERR, (int)CAN_NOT_MATCH), "Unrecognized Symbol" }
};