#include "Structures.h"


void initStructures(MODEL* md, VIEWER* vw)
{
	md->MAX_STRING_LINES = 0;
	md->MAX_STRINGS_LAYOUT = 0;
	md->size = 0;
	md->paragraphPos = NULL;
	md->textBuffer = NULL;
	vw->OUTPUT_FORMAT = LAYOUT_OFF;
	vw->cxCaps = vw->cxChar = vw->cxClient = vw->cyChar =
	vw->cyClient = vw->iHscrollInc = vw->iHscrollMax =
	vw->iHscrollPos = vw->iVscrollInc = vw->iVscrollMax = vw->iVscrollPos = vw->MAX_STRING_LENGTH = 0;
}

