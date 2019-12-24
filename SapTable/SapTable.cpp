/**
Create "critical dimensions table" with values from selected dimensions.
 */

#include "Dimension.h"
#include <ProCore.h>
#include <ProDwgtable.h>
#include <ProMenuBar.h>
#include <ProMessage.h>
#include <ProWindows.h>
#include <Windows.h>
#include <xstring>
#include "Table.h"

using namespace std;

static uiCmdAccessState AccessAvailable(uiCmdAccessMode);
void printMessage(string);
void makeTableFromDimensions();
char* strToCharArr(string);
void initializeMsgFile();

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
	ProCmdActionAdd(sapTab, (uiCmdCmdActFn)makeTableFromDimensions,
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

void makeTableFromDimensions()
{
	int winId;
	ProWindowCurrentGet(&winId);
	ProDrawing drawing;
	ProMdlCurrentGet((ProMdl*)&drawing);

	printMessage("wybierz wymiary");
	ProErr err = PRO_TK_NO_ERROR;
	ProSelection* selection = NULL;
	string filter = "dimension";
	int sel_count = 0;

	err = ProSelect(strToCharArr(filter), 10, NULL, NULL, NULL, NULL, &selection, &sel_count);
	ProDimension dim;
	Table table(drawing);
	for (int i = 0; i < sel_count; i++)
	{
		ProSelectionModelitemGet(selection[i], &dim);
		Dimension dimension(dim);
		table.insertDimension(dimension);
	}
	ProWindowActivate(winId);
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