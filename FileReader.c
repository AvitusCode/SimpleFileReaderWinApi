#include "FileReader.h"
#include "Structures.h"
#include <memory.h>
#define NUMLINES 100

/*Функция принимает указатель на массив, и новый размер, чтобы перевыделить память*/
BOOL resizeDinamicArray(size_t** array, size_t new_size, size_t old_size)
{
	size_t* re = (size_t*)malloc(new_size * sizeof(size_t));
	
	if (re == NULL)
    {
        printf("ERROR: REALLOC MEMORY ALLOCATION!\n");
        return FALSE;
    }
	
	memcpy(re, *array, old_size * sizeof(size_t));
	free(*array);
	*array = re;

	return TRUE;
}

// a sample exported function
BOOL MyFileReader(HWND hwnd, FILE *file)
{
    MODEL  *model  = (MODEL*)GetWindowLongPtr(hwnd, 0);
    VIEWER *viewer = (VIEWER*)GetWindowLongPtr(hwnd, sizeof(MODEL*));

    if (model->textBuffer)
    {
        free(model->textBuffer);
        free(model->paragraphPos);
        model->MAX_STRING_LINES = 0;
        viewer->MAX_STRING_LENGTH = 0;
    }

    fseek(file, 0, SEEK_END);
    model->size = ftell(file);
    rewind(file);
    model->textBuffer = (char*)malloc(model->size);
    model->paragraphPos = (size_t*)malloc(sizeof(size_t) * NUMLINES);
    model->paragraphPos[0] = 0;

    if (!model->textBuffer || !model->paragraphPos)
    {
        printf("MEMORY allocation error!\n");
        return FALSE;
    }
    
    size_t i = 0, len = 0, max_len = NUMLINES;
    fread(model->textBuffer, sizeof(char), model->size, file);
    
    while (i < model->size)
    {
        len++;

        if (model->textBuffer[i] == '\n')
        {
            if (viewer->MAX_STRING_LENGTH < len)
                viewer->MAX_STRING_LENGTH = len;
            
            len = 0;
            if (model->MAX_STRING_LINES >= (max_len - 1))
            {
                max_len += NUMLINES;
                if (!resizeDinamicArray(&model->paragraphPos, max_len, model->MAX_STRING_LINES + 1))
                    return FALSE;
            }
            
            model->paragraphPos[++model->MAX_STRING_LINES] = ++i;
        }
    }
    
    model->MAX_STRINGS_LAYOUT = model->MAX_STRING_LINES;
    viewer->MAX_STRING_LENGTH *= viewer->cxChar; // For proportional text

    return TRUE;
}
