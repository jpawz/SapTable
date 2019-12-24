#pragma once
#include <ProDimension.h>
#include <ProWstring.h>
class Dimension
{
public:
	Dimension(ProDimension dim);
	ProWstring* getDimensionText();
private:
	ProDimension dimension;
	ProDimensiontype dimensionType;
	ProWstring getTolerance();
	ProWstring getValue();
};