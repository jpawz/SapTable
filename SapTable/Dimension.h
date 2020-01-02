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
	bool dimensionIsFake;
	ProDimension dimension;
	ProWstring* beforeAtSymbol;
	ProWstring* afterAtSymbol;
	ProDimensiontype dimensionType;
	ProWstring* dimensionValue;
	ProWstring* toleranceText;
	ProWstring* completeDimension;
	ProWstring getTolerance();
	ProWstring getValue();
	void splitDimension();
};