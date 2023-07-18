// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __EDITSLIDERPAIR_H__
#define __EDITSLIDERPAIR_H__

#include "SliderView.h"
#include "FloatEditView.h"

class CEditSliderPair
{
public:
	void SubclassWindows(HWND hwndSlider, HWND hwndEdit);
	void SetValue(float fValue);
	float GetValue() const;
	void SetRange(float fMin, float fMax);

	TinyDocument<float>& GetDocument();

private:
	TinyDocument<float>		m_value;
	SliderView						m_slider;
	FloatEditView					m_edit;
};

#endif //__EDITSLIDERPAIR_H__
