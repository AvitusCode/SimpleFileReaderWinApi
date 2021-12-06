
#ifndef STRUCTURES
#define STRUCTURES


typedef enum
{
	LAYOUT_ON = 1,
	LAYOUT_OFF = 2
}Mode;

// ��������� ��� �������� ������ � ��������������
typedef struct
{
	size_t  MAX_STRING_LINES;  // ������������ ���������� �������
	size_t  MAX_STRINGS_LAYOUT;// ������������ ���������� �������, ������ ��� ������ �������
	size_t  size;              // ������ ���������� �������
	size_t* paragraphPos;      // �����, �������� ������� � ������ ������� ������� ����� ������ (������ ������� - ������, �������� - ��������� ���� ������� ������� ����� ������)
	char*   textBuffer;        // ��������� ������
}MODEL;

// ��������� � ��������������� ��� �������������
typedef struct
{
	int     cxChar;            // ����������� ������ �������
	int     cyChar;            // ������ ������ �������
	int     cxCaps;            // ������� ������ ������� �������� ��������

	int     cxClient;          // ������� �������� ���� �� x and y
	int     cyClient;

	// For Vertical
	int     iVscrollPos;       // ������� �������� ���������
	int     iVscrollMax;       // ������������ �������� ���������
	int     iVscrollInc;       // �������� �����������������
	// For Horizontal
	int     iHscrollPos;       // ������� �������� ���������
	int     iHscrollMax;       // ������������ �������� ���������
	int     iHscrollInc;       // �������� �����������������

	size_t  MAX_STRING_LENGTH; // ����� ������������ ������
	Mode    OUTPUT_FORMAT;     // ������ ������ ������ (� �������� ��� ���)
}VIEWER;

void initStructures(MODEL* md, VIEWER* vw);

#endif // STRUCTURES_H
