/**
Create "critical dimensions table" with values from selected dimensions.
 */

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <ProToolkit.h>
#include <ProMenu.h>
#include <ProDwgtable.h>
#include <ProMenuBar.h>
#include <ProWindows.h>
#include <ProMessage.h>
#include <ProNote.h>
#include <ProAnnotation.h>
#include <ProDimension.h>
#include <ProDrawing.h>
#include <ProUtil.h>
#include <ProCore.h>
#include <ProMdl.h>
#include <ProUICmd.h>
#include <ProMath.h>
#include <ProUtil.h>
#include <string>
#include <cmath>
#include <sstream>


class Dimension
{
public:
	Dimension(ProDimension dim)
	{
		dimension = dim;
	}
	Dimension() {};
	void setDimension(ProDimension dim)
	{
		dimension = dim;
	}
	ProWstring* getDimensionText();
private:
	ProDimension dimension;
	ProWstring getPrefix();
	ProWstring getSuffix();
	ProWstring getTolerance();
	ProWstring getValue();
};

ProWstring* Dimension::getDimensionText()
{
	ProWstring* completeDimension;
	ProArrayAlloc(1, sizeof(wchar_t), 1, (ProArray*)&completeDimension);
	completeDimension[0] = (wchar_t*)calloc(PRO_COMMENT_SIZE, sizeof(wchar_t));
	ProLine* dim_text;
	ProDimensionTextGet(&dimension, &dim_text);

	char dim_char_arr[200];
	ProWstringToString(dim_char_arr, dim_text[0]);
	char* beg_end[2];
	beg_end[0] = strtok(dim_char_arr, "@D");
	beg_end[1] = strtok(NULL, "@D");
	ProWstring* beggining;
	ProWstring* end;
	ProArrayAlloc(1, sizeof(wchar_t), 1, (ProArray*)&beggining);
	beggining[0] = (wchar_t*)calloc(PRO_COMMENT_SIZE, sizeof(wchar_t));
	ProArrayAlloc(1, sizeof(wchar_t), 1, (ProArray*)&end);
	end[0] = (wchar_t*)calloc(PRO_COMMENT_SIZE, sizeof(wchar_t));
	ProStringToWstring(beggining[0], beg_end[0]);
	ProStringToWstring(end[0], beg_end[1]);

	ProWstringConcatenate(beggining[0], completeDimension[0], PRO_VALUE_UNUSED);
	ProWstring value = getValue();
	ProWstringConcatenate(value, completeDimension[0], PRO_VALUE_UNUSED);
	ProWstring tolerance = getTolerance();
	ProWstringConcatenate(tolerance, completeDimension[0], PRO_VALUE_UNUSED);
	ProWstringConcatenate(end[0], completeDimension[0], PRO_VALUE_UNUSED);

	return completeDimension;
}

ProWstring Dimension::getValue()
{
	double value;
	int decimals;
	ProDimensionDecimalsGet(&dimension, &decimals);
	ProDimensionValueGet(&dimension, &value);
	ProWstring* v;
	char vv[10];
	ProArrayAlloc(1, sizeof(wchar_t), 1, (ProArray*)&v);
	v[0] = (wchar_t*)calloc(PRO_COMMENT_SIZE, sizeof(wchar_t));
	sprintf(vv, "%.*f", decimals, value);
	ProStringToWstring(v[0], vv);
	return v[0];
}

ProWstring Dimension::getPrefix()
{
	ProLine pfx;
	ProDimensionPrefixGet(&dimension, pfx);
	return pfx;
}

ProWstring Dimension::getSuffix()
{
	ProLine sfx;
	ProDimensionSuffixGet(&dimension, sfx);
	return sfx;
}

ProWstring Dimension::getTolerance()
{
	ProWstring *tol;
	double upper_limit, lower_limit;
	int decimals;
	ProDimToleranceType tol_type;
	ProDimensionToltypeGet(&dimension, &tol_type);
	ProDimensionDisplayedToleranceGet(&dimension, &upper_limit, &lower_limit);
	ProDimensionTolerancedecimalsGet(&dimension, &decimals);
	if (decimals > 4)
	{
		ProDimensionDecimalsGet(&dimension, &decimals);
	}
	ProArrayAlloc(1, sizeof(wchar_t), 1, (ProArray*)&tol);
	tol[0] = (wchar_t*)calloc(PRO_COMMENT_SIZE, sizeof(wchar_t));

	switch (tol_type)
	{
	case PRO_TOL_DEFAULT:
		break;
	case PRO_TOL_PLUS_MINUS:
		char tol_text[100];
		static char upper_text[20];
		static char lower_text[20];
		sprintf(upper_text, "%+.*f", decimals, upper_limit);
		sprintf(lower_text, "%+.*f", decimals, (lower_limit * -1.0));
		for (int i = 19; i > 0; i--)
		{
			if (upper_text[i] == 0 && lower_text == 0)
			{
				continue;
			}
			if ((upper_text[i] == '0' && upper_text[i - 1] == '0') &&
				(lower_text[i] == '0' && lower_text[i - 1] == '0'))
			{
				upper_text[i] = 0;
				lower_text[i] = 0;
			}
			else if ((upper_text[i] == '0' && upper_text[i - 1] == '.') &&
				(lower_text[i] == '0' && lower_text[i - 1] == '.'))
			{
				upper_text[i] = 0;
				upper_text[i - 1] = 0;
				lower_text[i] = 0;
				lower_text[i - 1] = 0;
				break;
			}
			else
			{
				upper_text[i] = 0;
				lower_text[i] = 0;
				break;
			}
		}
		sprintf(tol_text, "@+%s@#@-%s@#", upper_text, lower_text);
		ProStringToWstring(tol[0], tol_text);
		break;
	case PRO_TOL_LIMITS:
		break;
	case PRO_TOL_PLUS_MINUS_SYM:
		ProWstring* tol_s;
		ProWstring* tol_v;
		char plus_minus_sym[] = { '\001', '#', '\002', '\000'};
		static char tol_value[20];
		ProArrayAlloc(1, sizeof(wchar_t), 1, (ProArray*)&tol_s);
		ProArrayAlloc(1, sizeof(wchar_t), 1, (ProArray*)&tol_v);
		tol_s[0] = (wchar_t*)calloc(PRO_COMMENT_SIZE, sizeof(wchar_t));
		tol_v[0] = (wchar_t*)calloc(PRO_COMMENT_SIZE, sizeof(wchar_t));
		sprintf(tol_value, "%.*f", decimals, upper_limit);
		for (int i = 19; i > 0; i--)
		{
			if (tol_value[i] == 0)
			{
				continue;
			}
			if (tol_value[i] == '0' && tol_value[i-1] == '0')
			{
				tol_value[i] = 0;
			} else if (tol_value[i] == '0' && tol_value[i - 1] == '.')
			{
				tol_value[i] = 0;
				tol_value[i - 1] = 0;
				break;
			} else
			{
				tol_value[i] = 0;
				break;
			}
		}
		ProStringToWstring(tol_s[0], plus_minus_sym);
		ProStringToWstring(tol_v[0], tol_value);
		ProWstringConcatenate(tol_s[0], tol[0], PRO_VALUE_UNUSED);
		ProWstringConcatenate(tol_v[0], tol[0], PRO_VALUE_UNUSED);
		ProArrayFree((ProArray*)&tol_s);
		ProArrayFree((ProArray*)&tol_v);
		break;
	}
	return tol[0];
}


using namespace std;

static uiCmdAccessState AccessAvailable(uiCmdAccessMode);
void printMessage(string);
void createSapTable();
void selectDimensions();
char* strToCharArr(string);
void initializeMsgFile();
void createTable(ProDrawing, Dimension [], int);
void fillTableFirstColumnWithNumbers(ProDwgtable);
void fillTableWithDimensions(ProDwgtable, Dimension [], int);
void formatTextInCells(ProDwgtable, int);

ProFileName msgFile;

extern "C" int main(int argc, char** argv)
{
	ProToolkitMain(argc, argv);
	return 0;
}

extern "C" int user_initialize()
{
	ProErr err = PRO_TK_NO_ERROR;
	
	initializeMsgFile();

	uiCmdCmdId SapTable_cmd_id;

	
	char sapTab[] = "SapTable";
	ProCmdActionAdd(sapTab, (uiCmdCmdActFn)selectDimensions,
		uiCmdPrioDefault, AccessAvailable, PRO_B_FALSE,
		PRO_B_FALSE, &SapTable_cmd_id);
	char util_c[] = "Utilities";
	char item_n[] = "SapTable";
	char item_h[] = "Tabelka SAP";
	ProMenubarmenuPushbuttonAdd(util_c, item_n, item_n, item_h, NULL,
		PRO_B_TRUE, SapTable_cmd_id, msgFile);


	return 0;
}

extern "C" void user_terminate()
{
	return;
}

void createSapTable()
{

}


void selectDimensions()
{
	ProDrawing drawing;
	ProMdlCurrentGet((ProMdl*)&drawing);

	printMessage("wybierz wymiary");
	ProErr err = PRO_TK_NO_ERROR;
	ProSelection *selection = NULL;
	string filter = "dimension";
	int sel_count = 0;
	
	err = ProSelect(strToCharArr(filter), 10, NULL, NULL, NULL, NULL, &selection, &sel_count);
	ProDimension dim;
	Dimension dimensions[10];
	int numDim = 0;
	for (int i = 0; i < sel_count; i++)
	{
		ProSelectionModelitemGet(selection[i], &dim);
		dimensions[i].setDimension(dim);
		numDim = i+1;
		if (i == 9)
			break;
	}

	createTable(drawing, dimensions, numDim);
}



void printMessage(string message)
{
	ProMessageDisplay(msgFile, strToCharArr(message));
}

static uiCmdAccessState AccessAvailable(uiCmdAccessMode access_mode)
{
	return (ACCESS_AVAILABLE);
}

char* strToCharArr(string str)
{
	char* cstr = new char[str.length() + 1];
	strcpy(cstr, str.c_str());
	return cstr;
}

void initializeMsgFile()
{
	char MSGFIL[] = "saptab.txt";
	ProStringToWstring(msgFile, MSGFIL);
}


void createTable(ProDrawing drw, Dimension dimensions[], int numberOfDimensions)
{
	ProError err;
	ProDwgtabledata table_data;
	ProDwgtable table;
	ProPoint3d origin;
	ProMouseButton but;
	double width = 2;
	double height = 1;
	const int n_cols = 10;
	ProHorzJust justifications[n_cols];
	for (int i = 0; i < n_cols; i++) justifications[i] = PROHORZJUST_LEFT;

	err = ProMousePickGet(PRO_LEFT_BUTTON, &but, origin);

	err = ProDwgtabledataAlloc(&table_data);
	err = ProDwgtabledataOriginSet(table_data, origin);
	err = ProDwgtabledataColumnsSet(table_data, 1, &width, justifications);
	err = ProDwgtabledataRowsSet(table_data, 1, &height);
	err = ProDwgtabledataSizetypeSet(table_data, PRODWGTABLESIZE_CHARACTERS);
	err = ProDrawingTableCreate((ProDrawing)(drw), table_data, 1, &table);
	ProDwgtableColumnAdd(&table, 1, 1, n_cols);
	ProDwgtableRowAdd(&table, 1, 1, 1);
	ProDwgtableRowAdd(&table, 1, 1, 1);
	ProDwgtableRowAdd(&table, 1, 1, 1);
	ProDwgtableRowAdd(&table, 1, 1, 1);
	ProDwgtableRowAdd(&table, 1, 1, 1);
	ProDwgtableRowAdd(&table, 1, 1, 1);
	ProDwgtableRowAdd(&table, 1, 1, 1);
	ProDwgtableRowAdd(&table, 1, 1, 1);
	ProDwgtableRowAdd(&table, 1, 1, 1);

	fillTableFirstColumnWithNumbers(table);
	fillTableWithDimensions(table, dimensions, numberOfDimensions);
	
}

void fillTableWithDimensions(ProDwgtable table, Dimension dimensions[], int numberOfDimensions)
{
	for (int i = 1; i <= numberOfDimensions; i++)
	{	
		ProDwgtableTextEnter(&table, 2, i, dimensions[i-1].getDimensionText());
	}

	formatTextInCells(table, numberOfDimensions);
}

boolean findSupSubScript(char text[], int length)
{
	for (int i = 0; i < length - 1; i++)
	{
		if ((text[i] == '@') && ((text[i+1] == '+') || (text[i+1] == '-')))
		{
			return true;
		}
	}
	return false;
}

void formatTextInCells(ProDwgtable table, int numberOfCells)
{
	ProDtlnote note;
	ProDtlnotedata notedata;
	ProDtlnoteline* lines;
	ProDtlnotetext* texts;
	ProLine line;
	char str[80];
	int n_lines = 0, n_texts = 0;
	double t_height = 0;
	boolean modified = false;
	for (int i = 1; i <= numberOfCells; i++)
	{
		ProDwgtableCellNoteGet(&table, 2, i, &note);
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
				if (findSupSubScript(str, 80))
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
}


void fillTableFirstColumnWithNumbers(ProDwgtable table)
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