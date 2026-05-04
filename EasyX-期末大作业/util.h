#pragma once

#include <graphics.h>


inline void outtextxy_shaded(int x, int y, LPCTSTR text) {// 绘制带阴影的文本
	settextcolor(RGB(45, 45, 45));
	outtextxy(x + 2, y + 2, text);
	settextcolor(RGB(255, 255, 255));
	outtextxy(x, y, text);
}