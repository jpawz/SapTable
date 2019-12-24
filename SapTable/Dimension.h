#pragma once
#include <ProDimension.h>
#include <ProWstring.h>
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