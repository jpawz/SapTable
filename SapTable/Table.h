#pragma once
#include <ProDwgtable.h>
#include "Dimension.h"
class Table
{
public:
	Table(ProDrawing);
	void setTableOrigin();
	void prepareTable();
	void insertDimension(Dimension);
private:
	ProDwgtable table;
	ProDrawing drawing;
	static const int NUMBER_OF_COLUMNS = 10;
	ProPoint3d tableOrigin;
	int lastRowIndex;
	void createSapTable();
	void prepareFirstColumn();
	void formatCell(int);
	bool findUpperLowerTolerance(char[], int);
};
