#include "Table.h"
#include <xstring>

Table::Table(ProDrawing drw)
{
	lastRowIndex = 1;
	createSapTable(drw);
	prepareFirstColumn();
}

void Table::insertDimension(Dimension dimension)
{
	if (lastRowIndex > 10)
		return;
	ProDwgtableTextEnter(&table, 2, lastRowIndex, dimension.getDimensionText());
	formatCell(lastRowIndex);
	lastRowIndex++;
}

void Table::createSapTable(ProDrawing drw)
{
	ProDwgtabledata table_data;
	ProPoint3d origin;
	ProMouseButton but;
	double width = 2;
	double height = 1;

	ProHorzJust justifications[NUMBER_OF_COLUMNS];
	for (int i = 0; i < NUMBER_OF_COLUMNS; i++) justifications[i] = PROHORZJUST_LEFT;

	ProMousePickGet(PRO_LEFT_BUTTON, &but, origin);

	ProDwgtabledataAlloc(&table_data);
	ProDwgtabledataOriginSet(table_data, origin);
	ProDwgtabledataColumnsSet(table_data, 1, &width, justifications);
	ProDwgtabledataRowsSet(table_data, 1, &height);
	ProDwgtabledataSizetypeSet(table_data, PRODWGTABLESIZE_CHARACTERS);
	ProDrawingTableCreate((ProDrawing)(drw), table_data, 1, &table);
	ProDwgtableColumnAdd(&table, 1, 1, NUMBER_OF_COLUMNS);
	ProDwgtableRowAdd(&table, 1, 1, 1);
	ProDwgtableRowAdd(&table, 1, 1, 1);
	ProDwgtableRowAdd(&table, 1, 1, 1);
	ProDwgtableRowAdd(&table, 1, 1, 1);
	ProDwgtableRowAdd(&table, 1, 1, 1);
	ProDwgtableRowAdd(&table, 1, 1, 1);
	ProDwgtableRowAdd(&table, 1, 1, 1);
	ProDwgtableRowAdd(&table, 1, 1, 1);
	ProDwgtableRowAdd(&table, 1, 1, 1);
}

void Table::prepareFirstColumn()
{
	char numbers[3] = { '1', '.' };
	ProWstring* w_num;
	ProArrayAlloc(1, sizeof(wchar_t), 1, (ProArray*)&w_num);
	w_num[0] = (wchar_t*)calloc(PRO_COMMENT_SIZE, sizeof(wchar_t));
	for (int i = 1; i < 10; i++)
	{
		numbers[0] = i + '0';
		ProStringToWstring(w_num[0], numbers);
		ProDwgtableTextEnter(&table, 1, i, w_num);
	}
	numbers[0] = '1';
	numbers[1] = '0';
	numbers[2] = '.';
	ProStringToWstring(w_num[0], numbers);
	ProDwgtableTextEnter(&table, 1, 10, w_num);
	ProArrayFree((ProArray*)&w_num);
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
	double t_height = 0;
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
				ProDtlnotetextHeightGet(texts[k], &t_height);
				ProDtlnotetextHeightSet(texts[k], t_height * 0.5);
			}
		}
		ProDtlnotelineTextsSet(lines[j], texts);
	}
	ProDtlnoteldataLinesSet(notedata, lines);
	ProDtlnoteModify(&note, NULL, notedata);
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