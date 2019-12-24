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

Dimension::Dimension(ProDimension dim)
{
	dimension = dim;
}

void Dimension::setDimension(ProDimension dim)
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
	char vv[10];
	ProArrayAlloc(1, sizeof(wchar_t), 1, (ProArray*)&v);
	v[0] = (wchar_t*)calloc(PRO_COMMENT_SIZE, sizeof(wchar_t));
	sprintf(vv, "%.*f", decimals, value);
	ProStringToWstring(v[0], vv);
	return v[0];
}

ProWstring Dimension::getTolerance()
{
	ProWstring* tol;
	double upper_limit, lower_limit;
	int decimals;
	ProDimToleranceType tol_type;
	ProDimensionToltypeGet(&dimension, &tol_type);
	ProDimensionDisplayedToleranceGet(&dimension, &upper_limit, &lower_limit);
	ProDimensionTolerancedecimalsGet(&dimension, &decimals);
	if (decimals > 4)
	{
		ProDimensionDecimalsGet(&dimension, &decimals);
	}
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
		sprintf(upper_text, "%+.*f", decimals, upper_limit);
		sprintf(lower_text, "%+.*f", decimals, (lower_limit * -1.0));
		for (int i = 19; i > 0; i--)
		{
			if (upper_text[i] == 0 && lower_text == 0)
			{
				continue;
			}
			if ((upper_text[i] == '0' && upper_text[i - 1] == '0') &&
				(lower_text[i] == '0' && lower_text[i - 1] == '0'))
			{
				upper_text[i] = 0;
				lower_text[i] = 0;
			}
			else if ((upper_text[i] == '0' && upper_text[i - 1] == '.') &&
				(lower_text[i] == '0' && lower_text[i - 1] == '.'))
			{
				upper_text[i] = 0;
				upper_text[i - 1] = 0;
				lower_text[i] = 0;
				lower_text[i - 1] = 0;
				break;
			}
			else
			{
				upper_text[i] = 0;
				lower_text[i] = 0;
				break;
			}
		}
		sprintf(tol_text, "@+%s@#@-%s@#", upper_text, lower_text);
		ProStringToWstring(tol[0], tol_text);
		break;
	case PRO_TOL_LIMITS:
		break;
	case PRO_TOL_PLUS_MINUS_SYM:
		ProWstring* tol_s;
		ProWstring* tol_v;
		char plus_minus_sym[] = { '\001', '#', '\002', '\000' };
		static char tol_value[20];
		ProArrayAlloc(1, sizeof(wchar_t), 1, (ProArray*)&tol_s);
		ProArrayAlloc(1, sizeof(wchar_t), 1, (ProArray*)&tol_v);
		tol_s[0] = (wchar_t*)calloc(PRO_COMMENT_SIZE, sizeof(wchar_t));
		tol_v[0] = (wchar_t*)calloc(PRO_COMMENT_SIZE, sizeof(wchar_t));
		sprintf(tol_value, "%.*f", decimals, upper_limit);
		for (int i = 19; i > 0; i--)
		{
			if (tol_value[i] == 0)
			{
				continue;
			}
			if (tol_value[i] == '0' && tol_value[i - 1] == '0')
			{
				tol_value[i] = 0;
			}
			else if (tol_value[i] == '0' && tol_value[i - 1] == '.')
			{
				tol_value[i] = 0;
				tol_value[i - 1] = 0;
				break;
			}
			else
			{
				tol_value[i] = 0;
				break;
			}
		}
		ProStringToWstring(tol_s[0], plus_minus_sym);
		ProStringToWstring(tol_v[0], tol_value);
		ProWstringConcatenate(tol_s[0], tol[0], PRO_VALUE_UNUSED);
		ProWstringConcatenate(tol_v[0], tol[0], PRO_VALUE_UNUSED);
		ProArrayFree((ProArray*)&tol_s);
		ProArrayFree((ProArray*)&tol_v);
		break;
	}
	return tol[0];
}