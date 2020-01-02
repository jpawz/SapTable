#include "Dimension.h"
#include <ProDimension.h>
#include <ProArray.h>
#include <ProSizeConst.h>
#include <ProToolkit.h>
#include <ProUtil.h>
#include <ProWstring.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char plus_minus_sym[] = { '\001', '#', '\002', '\000' };
char degree_sym[] = { '\001', '$', '\002', '\000' };

void trimTrailingZeros(char[], int);
void trimTrailingZerosOfUpperAndLowerLimits(char[], char[], int);

Dimension::Dimension(ProDimension dim)
{
	dimension = dim;
	ProDimensionTypeGet(&dimension, &dimensionType);

	ProArrayAlloc(1, sizeof(wchar_t), 1, (ProArray*)&completeDimension);
	completeDimension[0] = (wchar_t*)calloc(PRO_COMMENT_SIZE, sizeof(wchar_t));

	ProArrayAlloc(1, sizeof(wchar_t), 1, (ProArray*)&dimensionValue);
	dimensionValue[0] = (wchar_t*)calloc(PRO_COMMENT_SIZE, sizeof(wchar_t));

	ProArrayAlloc(1, sizeof(wchar_t), 1, (ProArray*)&toleranceText);
	toleranceText[0] = (wchar_t*)calloc(PRO_COMMENT_SIZE, sizeof(wchar_t));
}

Dimension::~Dimension()
{
	ProArrayFree((ProArray*)&toleranceText);
	ProArrayFree((ProArray*)&dimensionValue);
	ProArrayFree((ProArray*)&completeDimension);
}

ProWstring* Dimension::getDimensionText()
{
	splitDimension();
	ProWstringConcatenate(beforeAtSymbol[0], completeDimension[0], PRO_VALUE_UNUSED);
	if (!dimensionIsFake)
	{
		ProWstring value = getValue();
		ProWstringConcatenate(value, completeDimension[0], PRO_VALUE_UNUSED);

		ProWstring tolerance = getTolerance();
		ProWstringConcatenate(tolerance, completeDimension[0], PRO_VALUE_UNUSED);
	}

	ProWstringConcatenate(afterAtSymbol[0], completeDimension[0], PRO_VALUE_UNUSED);

	ProArrayFree((ProArray*)&beforeAtSymbol);
	ProArrayFree((ProArray*)&afterAtSymbol);

	return completeDimension;
}

void Dimension::splitDimension()
{
	ProLine* dim_text;
	ProDimensionTextGet(&dimension, &dim_text);
	char dim_char_arr[PRO_COMMENT_SIZE];
	ProWstringToString(dim_char_arr, dim_text[0]);
	char* beforeDimSymbol = dim_char_arr;
	char* afterDimSymbol;

	for (int i = 0; i < PRO_COMMENT_SIZE - 1; i++)
	{
		if ((dim_char_arr[i] == '@') && ((dim_char_arr[i + 1] == 'D')))
		{
			dim_char_arr[i] = 0;
			afterDimSymbol = &dim_char_arr[i + 2];
			dimensionIsFake = false;
			break;
		}
		else if ((dim_char_arr[i] == '@') && (dim_char_arr[i + 1] == 'O'))
		{
			dim_char_arr[i] = 0;
			afterDimSymbol = &dim_char_arr[i + 2];
			dimensionIsFake = true;
			break;
		}
	}

	ProArrayAlloc(1, sizeof(wchar_t), 1, (ProArray*)&beforeAtSymbol);
	beforeAtSymbol[0] = (wchar_t*)calloc(PRO_COMMENT_SIZE, sizeof(wchar_t));
	ProArrayAlloc(1, sizeof(wchar_t), 1, (ProArray*)&afterAtSymbol);
	afterAtSymbol[0] = (wchar_t*)calloc(PRO_COMMENT_SIZE, sizeof(wchar_t));
	ProStringToWstring(beforeAtSymbol[0], beforeDimSymbol);
	ProStringToWstring(afterAtSymbol[0], afterDimSymbol);
}

ProWstring Dimension::getValue()
{
	double value;
	int decimals;
	ProDimensionDecimalsGet(&dimension, &decimals);
	ProDimensionValueGet(&dimension, &value);
	static char formattedValue[20];
	sprintf_s(formattedValue, 20, "%.*f", decimals, value);
	if (decimals > 0)
		trimTrailingZeros(formattedValue, 10);
	ProStringToWstring(dimensionValue[0], formattedValue);
	if (dimensionType == PRODIMTYPE_ANGLE)
	{
		ProWstring* degree_s;
		ProArrayAlloc(1, sizeof(wchar_t), 1, (ProArray*)&degree_s);
		degree_s[0] = (wchar_t*)calloc(PRO_COMMENT_SIZE, sizeof(wchar_t));
		ProStringToWstring(degree_s[0], degree_sym);
		ProWstringConcatenate(degree_s[0], dimensionValue[0], PRO_VALUE_UNUSED);
		ProArrayFree((ProArray*)&degree_s);
	}
	return dimensionValue[0];
}

ProWstring Dimension::getTolerance()
{
	double upper_limit, lower_limit;
	ProDimToleranceType tol_type;
	ProDimensionToltypeGet(&dimension, &tol_type);
	ProDimensionDisplayedToleranceGet(&dimension, &upper_limit, &lower_limit);

	switch (tol_type)
	{
	case PRO_TOL_DEFAULT:
		break;
	case PRO_TOL_PLUS_MINUS:
		char tol_text[100];
		static char upper_text[20];
		static char lower_text[20];
		sprintf_s(upper_text, 20, "%+f", upper_limit);
		sprintf_s(lower_text, 20, "%+f", (lower_limit * -1.0));
		trimTrailingZerosOfUpperAndLowerLimits(upper_text, lower_text, 20);
		if (dimensionType == PRODIMTYPE_ANGLE)
		{
			sprintf_s(tol_text, 100, "@+%s%s@#@-%s%s@#", upper_text, degree_sym, lower_text, degree_sym);
		}
		else
		{
			sprintf_s(tol_text, 100, "@+%s@#@-%s@#", upper_text, lower_text);
		}
		ProStringToWstring(toleranceText[0], tol_text);
		break;
	case PRO_TOL_LIMITS:
		break;
	case PRO_TOL_PLUS_MINUS_SYM:
		static char tol_value[20];
		char tol_value_with_symbol[20];
		sprintf_s(tol_value, 20, "%f", upper_limit);
		trimTrailingZeros(tol_value, 20);
		if (dimensionType == PRODIMTYPE_ANGLE)
		{
			sprintf_s(tol_value_with_symbol, 20, "%s%s%s", plus_minus_sym, tol_value, degree_sym);
		}
		else
		{
			sprintf_s(tol_value_with_symbol, 20, "%s%s", plus_minus_sym, tol_value);
		}
		ProStringToWstring(toleranceText[0], tol_value_with_symbol);
		break;
	}
	return toleranceText[0];
}

void trimTrailingZeros(char value[], int textLength)
{
	for (int i = textLength - 1; i > 0; i--)
	{
		if (value[i] == 0)
		{
			continue;
		}
		else if ((value[i] == '0') && (value[i - 1] == '.'))
		{
			value[i] = 0;
			value[i - 1] = 0;
			break;
		}
		else if (value[i] == '0')
		{
			value[i] = 0;
		}
		else
		{
			break;
		}
	}
}

void trimTrailingZerosOfUpperAndLowerLimits(char upper_text[], char lower_text[], int textLength)
{
	for (int i = textLength - 1; i > 0; i--)
	{
		if ((upper_text[i] == 0) && (lower_text[i] == 0))
		{
			continue;
		}
		else if (((upper_text[i] == '0') && (upper_text[i - 1] == '.')) &&
			((lower_text[i] == '0') && (lower_text[i - 1] == '.')))
		{
			upper_text[i] = 0;
			upper_text[i - 1] = 0;
			lower_text[i] = 0;
			lower_text[i - 1] = 0;
			break;
		}
		else if ((upper_text[i] == '0') && (lower_text[i] == '0'))
		{
			upper_text[i] = 0;
			lower_text[i] = 0;
		}
		else
		{
			break;
		}
	}
}