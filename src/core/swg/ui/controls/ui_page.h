/**
* MIT License
*
*Copyright(c) 2020 Philip Klatt
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files(the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
**/

#pragma once
#include "utinni.h"
#include "ui_widget.h"

namespace utinni
{
class UTINNI_API UIPage : public UIWidget // Size (Excluding base class): 36 (0x24)
{
public:
	swgptr UIPage_unkv01;
	swgptr UIPage_unkv02;
	swgptr UIPage_unkv03;
	swgptr UIPage_unkv04;
	swgptr UIPage_unkv05;
	swgptr UIPage_unkv06;
	swgptr UIPage_unkv07;

	UIPage();
};

}
