#pragma once
#include <ProDimension.h>
#include <ProWstring.h>
class Dimension
{
public:
	Dimension(ProDimension dim);
	~Dimension();
	ProWstring* getDimensionText();
private:
	ProDimension dimension;
	ProDimensiontype dimensionType;
	ProWstring* dimensionValue;
	ProWstring* toleranceText;
	ProWstring* completeDimension;
	ProWstring getTolerance();
	ProWstring getValue();
};