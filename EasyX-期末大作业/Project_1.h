#pragma once

#include <graphics.h>
#include <vector>
#include <string>

#include "Project.h"
#include "Rectangle.h"
// Note: Rectangle.h defines RectShape (renamed to avoid Windows GDI conflict)

class Pj1Scene : public Project
{
public:
	Pj1Scene() = default;
	~Pj1Scene() = default;

public:
	void on_enter() override {
		Project::on_enter();
		initAnimation();
	}

protected:
	void drawContent() override {
		// 绘制标题
		drawTitle();

		// 绘制数组矩形
		for (auto& rect : rects) {
			rect.drawWithText(std::to_wstring(rect.getValue()), BLACK, 22);
		}

		// 绘制步骤提示
		drawStepInfo();
	}

	void updateAnimation(int dalta) override {
		// 动画更新逻辑（如需要平滑移动可在此实现）
	}

	void onPrevStep() override {
		if (currentStep >= 0) {
			// 回退当前步骤的交换操作
			if (steps[currentStep].swapped) {
				std::swap(rects[steps[currentStep].leftIdx].value,
				          rects[steps[currentStep].rightIdx].value);
			}
			currentStep--;
			updateHighlight();
		}
	}

	void onNextStep() override {
		if (currentStep < (int)steps.size() - 1) {
			currentStep++;
			// 执行当前步骤的交换操作
			if (steps[currentStep].swapped) {
				std::swap(rects[steps[currentStep].leftIdx].value,
				          rects[steps[currentStep].rightIdx].value);
			}
			updateHighlight();
		}
	}

	bool hasNextStep() const override {
		return currentStep < (int)steps.size() - 1;
	}

	std::wstring getAlgorithmTitle() const override { return L"冒泡排序"; }
	std::wstring getAlgorithmInfo() const override {
		return L"冒泡排序是一种基础的交换排序算法。其核心思想是：重复走访待排序的元素列，依次比较两个相邻的元素，如果顺序错误就把它们交换过来。"
		       L"每一轮遍历会将当前未排序区间中最大的元素【冒泡】到正确的位置。算法名字来源于越小的元素会经由交换慢慢【浮】到数列的顶端。"
		       L"时间复杂度：平均和最坏情况均为 O(n²)，最好情况（已排序）为 O(n)。空间复杂度：O(1)。"
		       L"本动画演示了数组 [5,3,8,1,9,2,7,4] 的完整冒泡排序过程，黄色表示正在比较，绿色表示发生了交换。";
	}

private:
	struct Step {
		int leftIdx;   // 左索引
		int rightIdx;  // 右索引
		bool swapped;  // 是否发生了交换
	};

	struct RectNode : public RectShape {
		int value = 0;
		int getValue() const { return value; }
		void setValue(int v) { value = v; }
	};

	std::vector<RectNode> rects;
	std::vector<Step> steps; // 每一步的记录
	int currentStep = -1;

	void initAnimation()
	{
		rects.clear();
		steps.clear();
		currentStep = -1;

		// 初始化数组数据
		int values[] = { 5, 3, 8, 1, 9, 2, 7, 4 };
		int count = sizeof(values) / sizeof(values[0]);
		int rectW = 70;
		int rectH = 70;
		int gap = 15;
		int startX = (1280 - (count * rectW + (count - 1) * gap)) / 2;
		int startY = 250;

		for (int i = 0; i < count; i++) {
			RectNode node;
			node.setPos(startX + i * (rectW + gap), startY);
			node.setSize(rectW, rectH);
			node.setValue(values[i]);
			node.setFillColor(WHITE);
			node.setBorderColor(BLACK);
			rects.push_back(node);
		}

		// 模拟冒泡排序的每一步（记录比较的两个索引及是否交换）
		int arr[] = { 5, 3, 8, 1, 9, 2, 7, 4 };
		int n = count;
		for (int i = 0; i < n - 1; i++) {
			for (int j = 0; j < n - i - 1; j++) {
				bool needSwap = arr[j] > arr[j + 1];
				steps.push_back({ j, j + 1, needSwap });
				if (needSwap) {
					std::swap(arr[j], arr[j + 1]);
				}
			}
		}
	}

	void updateHighlight()
	{
		// 重置所有颜色
		for (auto& rect : rects) {
			rect.setFillColor(WHITE);
			rect.setBorderColor(BLACK);
		}

		if (currentStep >= 0 && currentStep < (int)steps.size()) {
			int i = steps[currentStep].leftIdx;
			int j = steps[currentStep].rightIdx;
			if (steps[currentStep].swapped) {
				// 发生交换：绿色高亮
				rects[i].setFillColor(RGB(150, 230, 150));
				rects[j].setFillColor(RGB(150, 230, 150));
				rects[i].setBorderColor(RGB(50, 150, 50));
				rects[j].setBorderColor(RGB(50, 150, 50));
			}
			else {
				// 仅比较：黄色高亮
				rects[i].setFillColor(RGB(255, 220, 100));
				rects[j].setFillColor(RGB(255, 220, 100));
				rects[i].setBorderColor(RGB(200, 150, 0));
				rects[j].setBorderColor(RGB(200, 150, 0));
			}
		}
	}

	void drawTitle() const
	{
		std::wstring title = L"冒泡排序演示";
		settextstyle(36, 0, L"微软雅黑");
		settextcolor(RGB(40, 40, 40));
		setbkmode(TRANSPARENT);

		int tw = textwidth(title.c_str());
		int tx = (1280 - tw) / 2;
		int ty = 120;
		outtextxy(tx, ty, title.c_str());

		// 装饰线
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
			info = L"点击下一步开始演示";
		}
		else if (currentStep >= (int)steps.size()) {
			info = L"演示结束";
		}
		else {
			int i = steps[currentStep].leftIdx;
			int j = steps[currentStep].rightIdx;
			info = L"第 " + std::to_wstring(currentStep + 1) + L" / " + std::to_wstring(steps.size()) +
			       L" 步：比较第 " + std::to_wstring(i + 1) + L" 个和第 " + std::to_wstring(j + 1) + L" 个元素";
			if (steps[currentStep].swapped) {
				info += L" （已交换）";
			}
			else {
				info += L" （无需交换）";
			}
		}

		settextstyle(20, 0, L"微软雅黑");
		settextcolor(RGB(80, 80, 80));
		setbkmode(TRANSPARENT);

		int tw = textwidth(info.c_str());
		int tx = (1280 - tw) / 2;
		int ty = 400;
		outtextxy(tx, ty, info.c_str());
	}
};
