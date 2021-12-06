
#ifndef STRUCTURES
#define STRUCTURES


typedef enum
{
	LAYOUT_ON = 1,
	LAYOUT_OFF = 2
}Mode;

// Структура для хранения текста и метаифнормации
typedef struct
{
	size_t  MAX_STRING_LINES;  // Максимальное количество абзацев
	size_t  MAX_STRINGS_LAYOUT;// Максимальное количество абзацев, храним для режима верстки
	size_t  size;              // Размер текстового массива
	size_t* paragraphPos;      // Масив, хранящий позицию в байтах первого символа новой строки (индекс массива - строка, значение - начальный байт первого символа новой строки)
	char*   textBuffer;        // Текстовый массив
}MODEL;

// Структура с метаинформацией для представления
typedef struct
{
	int     cxChar;            // Усредненная ширина символа
	int     cyChar;            // Полная высота символа
	int     cxCaps;            // Средняя ширина символа верхнего регистра

	int     cxClient;          // Размеры рабочего окна по x and y
	int     cyClient;

	// For Vertical
	int     iVscrollPos;       // Позиция колесика прокрутки
	int     iVscrollMax;       // Максимальное значение прокрутки
	int     iVscrollInc;       // Параметр инкрементирования
	// For Horizontal
	int     iHscrollPos;       // Позиция колесика прокрутки
	int     iHscrollMax;       // Максимальное значение прокрутки
	int     iHscrollInc;       // Параметр инкрементирования

	size_t  MAX_STRING_LENGTH; // Длина максимальной строки
	Mode    OUTPUT_FORMAT;     // Формат вывода данных (с версткой или без)
}VIEWER;

void initStructures(MODEL* md, VIEWER* vw);

#endif // STRUCTURES_H
