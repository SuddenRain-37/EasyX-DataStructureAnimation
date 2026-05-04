#pragma once

#include <graphics.h>
#include <string>
#include <vector>
#include <functional>

#include "Button.h"

class MiniWin
{
public:
	using Callback = std::function<void()>;

	MiniWin() = default;
	~MiniWin() = default;

	// 初始化弹窗（居中显示）
	void init(int width, int height, const std::wstring& title)
	{
		this->winW = width;
		this->winH = height;
		this->title = title;

		// 居中计算
		const int screenW = 1280;
		const int screenH = 720;
		this->x = (screenW - width) / 2;
		this->y = (screenH - height) / 2;

		// 关闭按钮（右上角）
		int closeBtnSize = 32;
		closeBtn = Button(x + width - closeBtnSize - 10, y + 10, closeBtnSize, closeBtnSize, L"×");
		closeBtn.setFontSize(20);
		closeBtn.setCornerRadius(6);
		closeBtn.setColors(RGB(220, 80, 80), RGB(250, 110, 110), RGB(190, 60, 60));
		closeBtn.setTextColor(WHITE);
		closeBtn.setOnClick([this]() {
			hide();
			if (onClose) onClose();
		});

		// 确定按钮（底部居中）
		okBtn = Button(x + (width - 100) / 2, y + height - 55, 100, 38, L"确定");
		okBtn.setFontSize(16);
		okBtn.setCornerRadius(6);
		okBtn.setColors(RGB(70, 130, 180), RGB(100, 160, 210), RGB(50, 100, 150));
		okBtn.setTextColor(WHITE);
		okBtn.setOnClick([this]() {
			hide();
			if (onClose) onClose();
		});
	}

	// 显示弹窗
	void show()
	{
		visible = true;
	}

	// 隐藏弹窗
	void hide()
	{
		visible = false;
	}

	// 切换显示状态
	void toggle()
	{
		visible = !visible;
	}

	bool isVisible() const { return visible; }

	// 设置关闭回调
	void setOnClose(const Callback& cb) { onClose = cb; }

	// 设置标题
	void setTitle(const std::wstring& t) { title = t; }

	// 设置内容（自动换行）
	void setContent(const std::wstring& text)
	{
		content = text;
		wrapText();
	}

	// 设置内容并自动换行到指定宽度
	void setContent(const std::wstring& text, int maxLineWidth)
	{
		content = text;
		wrapText(maxLineWidth);
	}

	// 处理输入消息，返回是否消耗了消息
	bool on_input(const ExMessage& msg)
	{
		if (!visible) return false;

		if (closeBtn.on_input(msg)) return true;
		if (okBtn.on_input(msg)) return true;

		// 点击弹窗外区域也关闭
		if (msg.message == WM_LBUTTONUP)
		{
			if (!contains(msg.x, msg.y))
			{
				hide();
				if (onClose) onClose();
				return true;
			}
		}

		return true; // 弹窗显示时拦截所有消息
	}

	// 绘制弹窗
	void draw() const
	{
		if (!visible) return;

		const int screenW = 1280;
		const int screenH = 720;

		// 半透明遮罩
		setfillcolor(RGBA(0, 0, 0, 120));
		solidrectangle(0, 0, screenW, screenH);

		// 弹窗阴影
		for (int i = shadowSize; i > 0; i--)
		{
			int alpha = 30 - i * 3;
			if (alpha < 0) alpha = 0;
			setfillcolor(RGBA(0, 0, 0, alpha));
			solidroundrect(x + i, y + i, x + winW + i, y + winH + i, cornerR * 2, cornerR * 2);
		}

		// 弹窗背景
		setfillcolor(bgColor);
		setlinecolor(borderColor);
		setlinestyle(PS_SOLID, 2);
		solidroundrect(x, y, x + winW, y + winH, cornerR * 2, cornerR * 2);
		setlinestyle(PS_SOLID, 2);
		fillroundrect(x, y, x + winW, y + winH, cornerR * 2, cornerR * 2);

		// 标题栏背景
		setfillcolor(titleBgColor);
		solidroundrect(x + 2, y + 2, x + winW - 2, y + titleH, cornerR * 2, cornerR * 2);
		// 补全标题栏下方矩形（避免圆角留下空白）
		solidrectangle(x + 2, y + titleH - cornerR, x + winW - 2, y + titleH);

		// 标题文字
		settextstyle(titleFontSize, 0, L"微软雅黑");
		settextcolor(titleTextColor);
		setbkmode(TRANSPARENT);
		int tw = textwidth(title.c_str());
		int th = textheight(title.c_str());
		outtextxy(x + (winW - tw) / 2, y + (titleH - th) / 2, title.c_str());

		// 标题底部分隔线
		setlinecolor(borderColor);
		setlinestyle(PS_SOLID, 1);
		line(x + 15, y + titleH, x + winW - 15, y + titleH);

		// 内容文字
		drawContent();

		// 关闭按钮
		closeBtn.draw();

		// 确定按钮
		okBtn.draw();
	}

private:
	int x = 0, y = 0;
	int winW = 500, winH = 300;
	int titleH = 44;
	int cornerR = 10;
	int shadowSize = 8;
	int paddingX = 25;
	int paddingY = 20;

	std::wstring title = L"提示";
	std::wstring content;
	std::vector<std::wstring> lines;

	COLORREF bgColor = WHITE;
	COLORREF borderColor = RGB(200, 200, 200);
	COLORREF titleBgColor = RGB(240, 245, 250);
	COLORREF titleTextColor = RGB(50, 50, 50);
	COLORREF contentTextColor = RGB(80, 80, 80);

	int titleFontSize = 26;
	int contentFontSize = 24;

	Button closeBtn;
	Button okBtn;
	bool visible = false;
	Callback onClose = nullptr;

	// 检查点是否在弹窗范围内
	bool contains(int px, int py) const
	{
		return px >= x && px <= x + winW && py >= y && py <= y + winH;
	}

	// 绘制内容文字（自动换行后的多行文本）
	void drawContent() const
	{
		if (lines.empty()) return;

		settextstyle(contentFontSize, 0, L"微软雅黑");
		settextcolor(contentTextColor);
		setbkmode(TRANSPARENT);

		int lineH = textheight(L"A");
		int startY = y + titleH + paddingY;
		int maxContentH = winH - titleH - paddingY - 65; // 预留按钮空间

		for (size_t i = 0; i < lines.size(); i++)
		{
			int ly = startY + (int)i * (lineH + 6);
			if (ly + lineH > y + winH - 65) break; // 超出区域不绘制
			outtextxy(x + paddingX, ly, lines[i].c_str());
		}
	}

	// 自动换行（默认按弹窗宽度计算）
	void wrapText()
	{
		wrapText(winW - paddingX * 2);
	}

	// 自动换行到指定像素宽度
	void wrapText(int maxWidth)
	{
		lines.clear();
		if (content.empty()) return;

		settextstyle(contentFontSize, 0, L"微软雅黑");

		std::wstring currentLine;
		for (size_t i = 0; i < content.size(); i++)
		{
			wchar_t ch = content[i];

			if (ch == L'\n')
			{
				lines.push_back(currentLine);
				currentLine.clear();
				continue;
			}

			currentLine.push_back(ch);
			int tw = textwidth(currentLine.c_str());
			if (tw > maxWidth && currentLine.size() > 1)
			{
				// 回退到最后一个可以断行的地方
				size_t breakPos = currentLine.size() - 1;
				while (breakPos > 0 && currentLine[breakPos] != L' ' && currentLine[breakPos] != L'，'
					&& currentLine[breakPos] != L'。' && currentLine[breakPos] != L'、'
					&& currentLine[breakPos] != L'；' && currentLine[breakPos] != L'：'
					&& currentLine[breakPos] != L'！' && currentLine[breakPos] != L'？')
				{
					breakPos--;
				}

				if (breakPos == 0)
				{
					// 找不到断行点，强制在最后一个字符前断开
					breakPos = currentLine.size() - 1;
				}

				std::wstring lineToPush = currentLine.substr(0, breakPos + 1);
				lines.push_back(lineToPush);
				currentLine = currentLine.substr(breakPos + 1);
			}
		}

		if (!currentLine.empty())
		{
			lines.push_back(currentLine);
		}
	}

	// 辅助函数：带透明度的颜色
	static COLORREF RGBA(int r, int g, int b, int a)
	{
		return RGB(r, g, b);
	}
};
