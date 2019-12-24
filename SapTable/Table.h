#pragma once
#include <ProDwgtable.h>
#include "Dimension.h"
class Table
{
public:
	Table(ProDrawing);
	void insertDimension(Dimension);
private:
	ProDwgtable table;
	static const int NUMBER_OF_COLUMNS = 10;
	int lastRowIndex;
	void createSapTable(ProDrawing);
	void prepareFirstColumn();
	void formatCell(int);
	bool findUpperLowerTolerance(char[], int);
};
