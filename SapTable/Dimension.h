#pragma once
#include <ProDimension.h>
#include <ProWstring.h>
class Dimension
{
public:
	Dimension(ProDimension dim);
	void setDimension(ProDimension dim);
	ProWstring* getDimensionText();
private:
	ProDimension dimension;
	ProWstring getTolerance();
	ProWstring getValue();
};