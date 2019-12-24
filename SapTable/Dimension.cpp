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
}

ProWstring* Dimension::getDimensionText()
{
	ProWstring* completeDimension;
	ProArrayAlloc(1, sizeof(wchar_t), 1, (ProArray*)&completeDimension);
	completeDimension[0] = (wchar_t*)calloc(PRO_COMMENT_SIZE, sizeof(wchar_t));
	ProLine* dim_text;
	ProDimensionTextGet(&dimension, &dim_text);
	ProDimensionTypeGet(&dimension, &dimensionType);
	char dim_char_arr[200];
	ProWstringToString(dim_char_arr, dim_text[0]);
	char* beg_end[2];
	beg_end[0] = strtok(dim_char_arr, "@D");
	beg_end[1] = strtok(NULL, "@D");
	ProWstring* beggining;
	ProWstring* end;
	ProArrayAlloc(1, sizeof(wchar_t), 1, (ProArray*)&beggining);
	beggining[0] = (wchar_t*)calloc(PRO_COMMENT_SIZE, sizeof(wchar_t));
	ProArrayAlloc(1, sizeof(wchar_t), 1, (ProArray*)&end);
	end[0] = (wchar_t*)calloc(PRO_COMMENT_SIZE, sizeof(wchar_t));
	ProStringToWstring(beggining[0], beg_end[0]);
	ProStringToWstring(end[0], beg_end[1]);

	ProWstringConcatenate(beggining[0], completeDimension[0], PRO_VALUE_UNUSED);
	ProWstring value = getValue();
	ProWstringConcatenate(value, completeDimension[0], PRO_VALUE_UNUSED);
	ProWstring tolerance = getTolerance();
	ProWstringConcatenate(tolerance, completeDimension[0], PRO_VALUE_UNUSED);
	ProWstringConcatenate(end[0], completeDimension[0], PRO_VALUE_UNUSED);

	return completeDimension;
}

ProWstring Dimension::getValue()
{
	double value;
	int decimals;
	ProDimensionDecimalsGet(&dimension, &decimals);
	ProDimensionValueGet(&dimension, &value);
	ProWstring* v;
	static char vv[10];
	ProArrayAlloc(1, sizeof(wchar_t), 1, (ProArray*)&v);
	v[0] = (wchar_t*)calloc(PRO_COMMENT_SIZE, sizeof(wchar_t));
	sprintf(vv, "%.*f", decimals, value);
	if (decimals > 0)
		trimTrailingZeros(vv, 10);
	ProStringToWstring(v[0], vv);
	if (dimensionType == PRODIMTYPE_ANGLE)
	{
		ProWstring* degree_s;
		ProArrayAlloc(1, sizeof(wchar_t), 1, (ProArray*)&degree_s);
		degree_s[0] = (wchar_t*)calloc(PRO_COMMENT_SIZE, sizeof(wchar_t));
		ProStringToWstring(degree_s[0], degree_sym);
		ProWstringConcatenate(degree_s[0], v[0], PRO_VALUE_UNUSED);
		ProArrayFree((ProArray*)&degree_s);
	}
	return v[0];
}

ProWstring Dimension::getTolerance()
{
	ProWstring* tol;
	double upper_limit, lower_limit;
	ProDimToleranceType tol_type;
	ProDimensionToltypeGet(&dimension, &tol_type);
	ProDimensionDisplayedToleranceGet(&dimension, &upper_limit, &lower_limit);
	ProArrayAlloc(1, sizeof(wchar_t), 1, (ProArray*)&tol);
	tol[0] = (wchar_t*)calloc(PRO_COMMENT_SIZE, sizeof(wchar_t));

	switch (tol_type)
	{
	case PRO_TOL_DEFAULT:
		break;
	case PRO_TOL_PLUS_MINUS:
		char tol_text[100];
		static char upper_text[20];
		static char lower_text[20];
		sprintf(upper_text, "%+f", upper_limit);
		sprintf(lower_text, "%+f", (lower_limit * -1.0));
		trimTrailingZerosOfUpperAndLowerLimits(upper_text, lower_text, 20);
		if (dimensionType == PRODIMTYPE_ANGLE)
		{
			sprintf(tol_text, "@+%s%s@#@-%s%s@#", upper_text, degree_sym, lower_text, degree_sym);
		}
		else
		{
			sprintf(tol_text, "@+%s@#@-%s@#", upper_text, lower_text);
		}
		ProStringToWstring(tol[0], tol_text);
		break;
	case PRO_TOL_LIMITS:
		break;
	case PRO_TOL_PLUS_MINUS_SYM:
		ProWstring* tol_s;
		ProWstring* tol_v;
		static char tol_value[20];
		ProArrayAlloc(1, sizeof(wchar_t), 1, (ProArray*)&tol_s);
		ProArrayAlloc(1, sizeof(wchar_t), 1, (ProArray*)&tol_v);
		tol_s[0] = (wchar_t*)calloc(PRO_COMMENT_SIZE, sizeof(wchar_t));
		tol_v[0] = (wchar_t*)calloc(PRO_COMMENT_SIZE, sizeof(wchar_t));
		sprintf(tol_value, "%f", upper_limit);
		trimTrailingZeros(tol_value, 20);
		ProStringToWstring(tol_s[0], plus_minus_sym);
		ProStringToWstring(tol_v[0], tol_value);
		ProWstringConcatenate(tol_s[0], tol[0], PRO_VALUE_UNUSED);
		ProWstringConcatenate(tol_v[0], tol[0], PRO_VALUE_UNUSED);
		if (dimensionType == PRODIMTYPE_ANGLE)
		{
			ProWstring* degree_s;
			ProArrayAlloc(1, sizeof(wchar_t), 1, (ProArray*)&degree_s);
			degree_s[0] = (wchar_t*)calloc(PRO_COMMENT_SIZE, sizeof(wchar_t));
			ProStringToWstring(degree_s[0], degree_sym);
			ProWstringConcatenate(degree_s[0], tol[0], PRO_VALUE_UNUSED);
			ProArrayFree((ProArray*)&degree_s);
		}
		ProArrayFree((ProArray*)&tol_s);
		ProArrayFree((ProArray*)&tol_v);
		break;
	}
	return tol[0];
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