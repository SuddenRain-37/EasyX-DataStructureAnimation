#pragma once

#include <graphics.h>
#include <vector>
#include <string>

#include "Project.h"

class Pj9Scene : public Project
{
public:
	Pj9Scene() = default;
	~Pj9Scene() = default;

public:
	void on_enter() override {
		Project::on_enter();
		initAnimation();
	}

protected:
	void drawContent() override {
		drawTitle();
		drawArray();
		drawTargetInfo();
		drawStepInfo();
	}

	void updateAnimation(int dalta) override {
	}

	void onPrevStep() override {
		if (currentStep > 0) {
			currentStep--;
		}
	}

	void onNextStep() override {
		if (currentStep < (int)steps.size() - 1) {
			currentStep++;
		}
	}

	bool hasNextStep() const override {
		return currentStep < (int)steps.size() - 1;
	}

	std::wstring getAlgorithmTitle() const override { return L"二分查找"; }
	std::wstring getAlgorithmInfo() const override {
		return L"二分查找（Binary Search）是一种在有序数组中查找目标元素的高效算法。"
		       L"算法核心思想：每次将查找区间折半，通过比较中间元素与目标值的大小，决定向左半区间还是右半区间继续查找，直到找到目标或区间为空。"
		       L"时间复杂度：O(log n)。空间复杂度：O(1)。要求数组必须是有序的。"
		       L"本动画在有序数组 [3,8,12,18,25,31,42,56,67,89] 中查找目标 42。黄色表示当前 mid 位置，蓝色表示当前搜索范围，灰色表示已排除的范围。";
	}

private:
	struct BSearchStep {
		int low;
		int mid;
		int high;
		int cmpResult;
		std::wstring desc;

		BSearchStep(int l, int m, int h, int c, const std::wstring& d)
			: low(l), mid(m), high(h), cmpResult(c), desc(d) {}
	};

	std::vector<int> arr;
	int target = 0;
	std::vector<BSearchStep> steps;
	int currentStep = -1;

	// 布局
	const int cellW = 80;
	const int cellH = 70;
	const int gap = 20;
	const int baseY = 280;

	void initAnimation()
	{
		arr.clear();
		steps.clear();
		currentStep = -1;

		// 已排序数组
		int values[] = { 3, 8, 12, 18, 25, 31, 42, 56, 67, 89 };
		for (int v : values) arr.push_back(v);
		target = 42;

		// 生成二分查找步骤
		int low = 0;
		int high = (int)arr.size() - 1;
		int mid = -1;
		int cmp = 0;

		std::wstring desc;
		desc = L"初始化：查找目标 ";
		desc.append(std::to_wstring(target));
		desc.append(L"，low=0，high=");
		desc.append(std::to_wstring(high));
		steps.emplace_back(low, mid, high, cmp, desc);

		while (low <= high) {
			mid = low + (high - low) / 2;

			if (arr[mid] == target) {
				cmp = 0;
				desc = L"mid=";
				desc.append(std::to_wstring(mid));
				desc.append(L"，arr[mid]=");
				desc.append(std::to_wstring(arr[mid]));
				desc.append(L" == target，查找成功！");
				steps.emplace_back(low, mid, high, cmp, desc);
				break;
			}
			else if (arr[mid] > target) {
				cmp = -1;
				desc = L"mid=";
				desc.append(std::to_wstring(mid));
				desc.append(L"，arr[mid]=");
				desc.append(std::to_wstring(arr[mid]));
				desc.append(L" > target，向左查找");
				steps.emplace_back(low, mid, high, cmp, desc);
				high = mid - 1;
			}
			else {
				cmp = 1;
				desc = L"mid=";
				desc.append(std::to_wstring(mid));
				desc.append(L"，arr[mid]=");
				desc.append(std::to_wstring(arr[mid]));
				desc.append(L" < target，向右查找");
				steps.emplace_back(low, mid, high, cmp, desc);
				low = mid + 1;
			}
		}

		if (low > high) {
			desc = L"low > high，查找失败，目标 ";
			desc.append(std::to_wstring(target));
			desc.append(L" 不在数组中");
			steps.emplace_back(low, -1, high, cmp, desc);
		}
	}

	void drawArray() const
	{
		int n = (int)arr.size();
		int totalW = n * cellW + (n - 1) * gap;
		int startX = (1280 - totalW) / 2;

		int low = -1, mid = -1, high = -1;
		if (currentStep >= 0 && currentStep < (int)steps.size()) {
			low = steps[currentStep].low;
			mid = steps[currentStep].mid;
			high = steps[currentStep].high;
		}

		for (int i = 0; i < n; i++) {
			int x = startX + i * (cellW + gap);
			int y = baseY;

			// 颜色判断
			COLORREF fillColor = WHITE;
			COLORREF borderColor = RGB(180, 180, 180);

			if (i == mid && mid >= 0) {
				// mid 当前比较位置：黄色
				fillColor = RGB(255, 230, 100);
				borderColor = RGB(220, 150, 0);
			}
			else if (currentStep >= 0 && i >= low && i <= high) {
				// 当前搜索范围内：浅蓝色
				fillColor = RGB(230, 245, 255);
				borderColor = RGB(100, 160, 210);
			}
			else if (currentStep >= 0 && steps[currentStep].mid >= 0) {
				// 已排除的范围：浅灰色
				fillColor = RGB(245, 245, 245);
				borderColor = RGB(200, 200, 200);
			}

			setfillcolor(fillColor);
			setlinecolor(borderColor);
			setlinestyle(PS_SOLID, 2);
			fillrectangle(x, y, x + cellW, y + cellH);

			// 数值
			std::wstring valStr = std::to_wstring(arr[i]);
			settextstyle(22, 0, L"微软雅黑");
			settextcolor(i == mid ? RGB(180, 120, 0) : RGB(60, 60, 60));
			setbkmode(TRANSPARENT);
			int tw = textwidth(valStr.c_str());
			int th = textheight(valStr.c_str());
			outtextxy(x + (cellW - tw) / 2, y + (cellH - th) / 2, valStr.c_str());

			// 下标
			std::wstring idxStr = std::to_wstring(i);
			settextstyle(14, 0, L"微软雅黑");
			settextcolor(RGB(120, 120, 120));
			int iw = textwidth(idxStr.c_str());
			outtextxy(x + (cellW - iw) / 2, y + cellH + 8, idxStr.c_str());

			// 标记 low / mid / high
			if (i == low && i == high && low >= 0) {
				drawLabel(x, y - 28, L"low/mid/high", RGB(70, 130, 180));
			}
			else if (i == low && low >= 0) {
				drawLabel(x, y - 22, L"low", RGB(70, 130, 180));
			}
			else if (i == mid && mid >= 0) {
				drawLabel(x, y - 22, L"mid", RGB(220, 150, 0));
			}
			else if (i == high && high >= 0) {
				drawLabel(x, y - 22, L"high", RGB(70, 130, 180));
			}
		}

		// 如果查找成功，给找到的 mid 画绿色外框
		if (currentStep >= 0 && mid >= 0 && steps[currentStep].cmpResult == 0) {
			int x = startX + mid * (cellW + gap);
			int y = baseY;
			setlinecolor(RGB(50, 180, 50));
			setlinestyle(PS_SOLID, 4);
			rectangle(x - 3, y - 3, x + cellW + 3, y + cellH + 3);
		}
	}

	void drawLabel(int x, int y, const std::wstring& text, COLORREF color) const
	{
		settextstyle(14, 0, L"微软雅黑");
		settextcolor(color);
		setbkmode(TRANSPARENT);
		int tw = textwidth(text.c_str());
		outtextxy(x + (cellW - tw) / 2, y, text.c_str());
	}

	void drawTargetInfo() const
	{
		std::wstring info = L"查找目标：";
		info.append(std::to_wstring(target));
		settextstyle(24, 0, L"微软雅黑");
		settextcolor(RGB(60, 60, 60));
		setbkmode(TRANSPARENT);
		int tw = textwidth(info.c_str());
		int tx = (1280 - tw) / 2;
		outtextxy(tx, baseY - 80, info.c_str());
	}

	void drawTitle() const
	{
		std::wstring title = L"二分查找演示";
		settextstyle(36, 0, L"微软雅黑");
		settextcolor(RGB(40, 40, 40));
		setbkmode(TRANSPARENT);

		int tw = textwidth(title.c_str());
		int tx = (1280 - tw) / 2;
		int ty = 25;
		outtextxy(tx, ty, title.c_str());

		setlinecolor(RGB(70, 130, 180));
		setlinestyle(PS_SOLID, 3);
		int lineW = tw + 30;
		int lineX = (1280 - lineW) / 2;
		line(lineX, ty + 50, lineX + lineW, ty + 50);
	}

	void drawStepInfo() const
	{
		std::wstring info;
		if (currentStep < 0) {
			info = L"点击“下一步”开始二分查找演示";
		}
		else if (currentStep >= (int)steps.size() - 1) {
			info = steps.back().desc;
		}
		else {
			info = L"步骤 ";
			info.append(std::to_wstring(currentStep + 1));
			info.append(L" / ");
			info.append(std::to_wstring((int)steps.size() - 1));
			info.append(L" ：");
			info.append(steps[currentStep].desc);
		}

		settextstyle(20, 0, L"微软雅黑");
		settextcolor(RGB(80, 80, 80));
		setbkmode(TRANSPARENT);

		int tw = textwidth(info.c_str());
		int tx = (1280 - tw) / 2;
		int ty = 600;
		outtextxy(tx, ty, info.c_str());
	}
};
