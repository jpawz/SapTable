#include "Table.h"
#include <xstring>

Table::Table(ProDrawing drw)
{
	lastRowIndex = 2;
	drawing = drw;
}

void Table::prepareTable()
{
	createSapTable();
	prepareFirstColumn();
	formatTemplateCells();
}

void Table::insertDimension(Dimension dimension)
{
	if (lastRowIndex > 11)
		return;
	ProDwgtableTextEnter(&table, 2, lastRowIndex, dimension.getDimensionText());
	formatCell(lastRowIndex);
	lastRowIndex++;
}

void Table::setTableOrigin()
{
	ProMouseButton but;
	ProMousePickGet(PRO_LEFT_BUTTON, &but, tableOrigin);
}

void Table::createSapTable()
{
	ProDwgtabledata tableData;
	double columnsWidth[] = { 1.7, 8 };
	double height = 1;
	int numberOfRows = 14;

	ProHorzJust justifications[2] = { PROHORZJUST_CENTER, PROHORZJUST_CENTER };

	ProDwgtabledataAlloc(&tableData);
	ProDwgtabledataOriginSet(tableData, tableOrigin);

	ProDwgtabledataColumnsSet(tableData, 2, columnsWidth, justifications);
	ProDwgtabledataRowsSet(tableData, 1, &height);
	ProDwgtabledataSizetypeSet(tableData, PRODWGTABLESIZE_CHARACTERS);
	ProDrawingTableCreate((ProDrawing)(drawing), tableData, 1, &table);

	for (int i = 1; i < numberOfRows - 3; i++)
		ProDwgtableRowAdd(&table, 1, 1, 0.7);

	ProDwgtableRowAdd(&table, 11, 1, 0.3);
	ProDwgtableCellsMerge(&table, 1, 12, 2, 12, 1);
	ProDwgtableRowAdd(&table, 12, 1, 0.3);
	ProDwgtableCellsMerge(&table, 1, 13, 2, 13, 1);
}

void Table::prepareFirstColumn()
{
	char numbers[2] = { '1' };
	ProWstring* text;
	ProArrayAlloc(1, sizeof(wchar_t), 1, (ProArray*)&text);
	text[0] = (wchar_t*)calloc(PRO_COMMENT_SIZE, sizeof(wchar_t));
	char header[] = { '\001', ' ', '\002', '\001', ' ', '\002', '\000' };
	ProStringToWstring(text[0], header);
	ProDwgtableTextEnter(&table, 2, 1, text);
	for (int i = 1; i < 11; i++)
	{
		numbers[0] = i + '0';
		ProStringToWstring(text[0], numbers);
		ProDwgtableTextEnter(&table, 1, i + 1, text);
	}
	numbers[0] = '1';
	numbers[1] = '0';
	ProStringToWstring(text[0], numbers);
	ProDwgtableTextEnter(&table, 1, 11, text);

	char dateSym[] = { "&PTC_WM_MODIFIED_ON" };
	ProStringToWstring(text[0], dateSym);
	ProDwgtableTextEnter(&table, 2, 12, text);

	char nameSym[] = { "&PTC_WM_MODIFIED_BY" };
	ProStringToWstring(text[0], nameSym);
	ProDwgtableTextEnter(&table, 2, 13, text);

	ProArrayFree((ProArray*)&text);
}

void Table::formatCell(int cellNumber)
{
	ProDtlnote note;
	ProDtlnotedata notedata;
	ProDtlnoteline* lines;
	ProDtlnotetext* texts;
	ProLine line;
	char str[80];
	int n_lines = 0, n_texts = 0;
	ProDwgtableCellNoteGet(&table, 2, lastRowIndex, &note);
	ProDtlnoteDataGet(&note, NULL, PRODISPMODE_SYMBOLIC, &notedata);
	ProDtlnotedataLinesCollect(notedata, &lines);
	ProDtlnotedataJustifSet(notedata, PRO_HORZJUST_LEFT, PRO_VERTJUST_MIDDLE);
	ProArraySizeGet((ProArray)lines, &n_lines);
	for (int j = 0; j < n_lines; j++)
	{
		ProDtlnotelineTextsCollect(lines[j], &texts);
		ProArraySizeGet((ProArray)texts, &n_texts);
		for (int k = 0; k < n_texts; k++)
		{
			ProDtlnotetextStringGet(texts[k], line);
			ProWstringToString(str, line);
			if (findUpperLowerTolerance(str, 80))
			{
				ProDtlnotetextHeightSet(texts[k], getValueInDrawingUnits(1.8));
			}
			else
			{
				ProDtlnotetextHeightSet(texts[k], getValueInDrawingUnits(3.0));
			}
		}
		ProDtlnotelineTextsSet(lines[j], texts);
	}
	ProDtlnoteldataLinesSet(notedata, lines);
	ProDtlnoteModify(&note, NULL, notedata);
}

void Table::formatTemplateCells()
{
	formatCell(2, 1, 3.0, PRO_HORZJUST_CENTER, PRO_VERTJUST_MIDDLE);

	for (int i = 2; i <= 11; i++)
	{
		formatCell(1, i, 3.0, PRO_HORZJUST_CENTER, PRO_VERTJUST_MIDDLE);
	}

	for (int i = 12; i <= 13; i++)
	{
		formatCell(2, i, 2.0, PRO_HORZJUST_RIGHT, PRO_VERTJUST_MIDDLE);
	}
}

void Table::formatCell(int column, int row, double textSize, horizontal_just horzJust, vertical_just vertJust)
{
	ProDtlnote note;
	ProDtlnotedata noteData;
	ProDtlnoteline* lines;
	ProDtlnotetext* texts;

	ProDwgtableCellNoteGet(&table, column, row, &note);
	ProDtlnoteDataGet(&note, NULL, PRODISPMODE_SYMBOLIC, &noteData);
	ProDtlnotedataLinesCollect(noteData, &lines);
	ProDtlnotedataJustifSet(noteData, horzJust, vertJust);
	ProDtlnotelineTextsCollect(lines[0], &texts);
	ProDtlnotetextHeightSet(texts[0], getValueInDrawingUnits(textSize));
	ProDtlnotelineTextsSet(lines[0], texts);
	ProDtlnoteldataLinesSet(noteData, lines);
	ProDtlnoteModify(&note, NULL, noteData);
}

double Table::getValueInDrawingUnits(double sizeInScreenUnits)
{
	int currentSheet;
	ProName w_size;
	ProMatrix matrix;
	ProDrawingCurrentSheetGet(drawing, &currentSheet);
	ProDrawingSheetTrfGet(drawing, currentSheet, w_size, matrix);
	return sizeInScreenUnits / matrix[0][0];
}

bool Table::findUpperLowerTolerance(char text[], int length)
{
	for (int i = 0; i < length - 1; i++)
	{
		if ((text[i] == '@') && ((text[i + 1] == '+') || (text[i + 1] == '-')))
		{
			return true;
		}
	}
	return false;
}