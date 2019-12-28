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
	ProPoint3d tableOrigin;
	int lastRowIndex;
	void createSapTable();
	void prepareFirstColumn();
	void formatCell(int);
	void formatCell(int, int, double, horizontal_just, vertical_just);
	void formatTemplateCells();
	bool findUpperLowerTolerance(char[], int);
	double getValueInDrawingUnits(double);
};
