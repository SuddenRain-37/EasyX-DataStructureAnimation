#pragma once

#include <graphics.h>
#include <vector>
#include <string>

#include "Project.h"

class Pj8Scene : public Project
{
public:
	Pj8Scene() = default;
	~Pj8Scene() = default;

public:
	void on_enter() override {
		Project::on_enter();
		initAnimation();
	}

protected:
	void drawContent() override {
		drawTitle();
		drawHeap();
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

	std::wstring getAlgorithmTitle() const override { return L"堆排序"; }
	std::wstring getAlgorithmInfo() const override {
		return L"堆排序是一种基于堆数据结构的比较排序算法。堆是一种近似完全二叉树的结构，满足父节点的值总是大于等于（大根堆）或小于等于（小根堆）子节点的值。"
		       L"算法步骤：1）建堆：将无序数组调整为大根堆；2）排序：将堆顶元素（最大值）与末尾交换，然后对剩余元素重新调整堆，重复此过程。"
		       L"时间复杂度：建堆 O(n)，每次调整 O(log n)，总复杂度 O(n log n)。空间复杂度：O(1)。"
		       L"本动画使用大根堆对数组 [4,1,3,2,16,9,10,14,8,7] 进行升序排序。黄色表示正在比较的父子节点，绿色表示已归位的元素。";
	}

private:
	struct HeapStep {
		std::vector<int> arr;
		std::vector<int> highlights;
		std::vector<int> sorted;
		std::wstring desc;
	};

	std::vector<HeapStep> steps;
	int currentStep = -1;

	// 布局参数
	const int barW = 60;
	const int barGap = 40;
	const int baseY = 520;
	const int maxBarH = 320;
	const int startX = 160;
	const int maxVal = 16;

	void initAnimation()
	{
		steps.clear();
		currentStep = -1;

		std::vector<int> arr = { 4, 1, 3, 2, 16, 9, 10, 14, 8, 7 };
		std::vector<int> sorted;

		steps.push_back({ arr, {}, sorted, L"初始数组，接下来进行堆排序（大根堆）" });

		int n = (int)arr.size();

		// 建堆
		for (int i = n / 2 - 1; i >= 0; --i) {
			siftDown(arr, sorted, i, n, L"建堆");
		}

		// 排序
		for (int i = n - 1; i > 0; --i) {
			std::swap(arr[0], arr[i]);
			sorted.push_back(i);
			steps.push_back({ arr, { 0, i }, sorted, L"交换堆顶与末尾，最大值 " + std::to_wstring(arr[i]) + L" 归位" });
			siftDown(arr, sorted, 0, i, L"调整堆");
		}

		sorted.push_back(0);
		steps.push_back({ arr, {}, sorted, L"排序完成！数组已按升序排列" });
	}

	void siftDown(std::vector<int>& arr, std::vector<int>& sorted, int root, int size, const std::wstring& phase)
	{
		while (true) {
			int largest = root;
			int left = 2 * root + 1;
			int right = 2 * root + 2;

			if (left < size) {
				steps.push_back({ arr, { root, left }, sorted,
					phase + L"：比较父节点 " + std::to_wstring(arr[root]) + L" 与左子节点 " + std::to_wstring(arr[left]) });
				if (arr[left] > arr[largest]) {
					largest = left;
				}
			}

			if (right < size) {
				steps.push_back({ arr, { root, right }, sorted,
					phase + L"：比较父节点 " + std::to_wstring(arr[root]) + L" 与右子节点 " + std::to_wstring(arr[right]) });
				if (arr[right] > arr[largest]) {
					largest = right;
				}
			}

			if (largest != root) {
				steps.push_back({ arr, { root, largest }, sorted,
					phase + L"：" + std::to_wstring(arr[largest]) + L" 更大，交换父节点与子节点" });
				std::swap(arr[root], arr[largest]);
				steps.push_back({ arr, { root, largest }, sorted,
					phase + L"：交换完成，继续下沉调整" });
				root = largest;
			}
			else {
				steps.push_back({ arr, { root }, sorted,
					phase + L"：父节点已是最大，无需交换，调整结束" });
				break;
			}
		}
	}

	void drawHeap() const
	{
		if (currentStep < 0 || currentStep >= (int)steps.size()) return;

		const auto& step = steps[currentStep];
		int n = (int)step.arr.size();

		// 绘制父子连接线（在柱子后面）
		drawTreeLines(step.arr);

		for (int i = 0; i < n; ++i) {
			int x = startX + i * (barW + barGap);
			int val = step.arr[i];
			int barH = (int)((float)val / maxVal * maxBarH);
			int y = baseY - barH;

			// 判断颜色
			bool isHighlight = false;
			bool isSorted = false;
			for (int h : step.highlights) {
				if (h == i) isHighlight = true;
			}
			for (int s : step.sorted) {
				if (s == i) isSorted = true;
			}

			COLORREF fillColor = WHITE;
			COLORREF borderColor = RGB(180, 180, 180);
			if (isSorted) {
				fillColor = RGB(150, 230, 150);
				borderColor = RGB(50, 150, 50);
			}
			else if (isHighlight) {
				fillColor = RGB(255, 230, 100);
				borderColor = RGB(220, 150, 0);
			}

			setfillcolor(fillColor);
			setlinecolor(borderColor);
			setlinestyle(PS_SOLID, 2);
			fillrectangle(x, y, x + barW, baseY);

			// 数值
			std::wstring valStr = std::to_wstring(val);
			settextstyle(18, 0, L"微软雅黑");
			settextcolor(isSorted ? RGB(50, 150, 50) : (isHighlight ? RGB(180, 120, 0) : RGB(60, 60, 60)));
			setbkmode(TRANSPARENT);
			int tw = textwidth(valStr.c_str());
			int th = textheight(valStr.c_str());
			outtextxy(x + (barW - tw) / 2, y - th - 4, valStr.c_str());

			// 下标
			std::wstring idxStr = std::to_wstring(i);
			settextstyle(14, 0, L"微软雅黑");
			settextcolor(RGB(120, 120, 120));
			int iw = textwidth(idxStr.c_str());
			outtextxy(x + (barW - iw) / 2, baseY + 6, idxStr.c_str());
		}
	}

	void drawTreeLines(const std::vector<int>& arr) const
	{
		int n = (int)arr.size();
		setlinecolor(RGB(200, 200, 200));
		setlinestyle(PS_SOLID, 1);

		for (int i = 0; i < n; ++i) {
			int left = 2 * i + 1;
			int right = 2 * i + 2;
			if (left < n) drawLineBetween(i, left, arr);
			if (right < n) drawLineBetween(i, right, arr);
		}
	}

	void drawLineBetween(int parent, int child, const std::vector<int>& arr) const
	{
		int px = startX + parent * (barW + barGap) + barW / 2;
		int pval = arr[parent];
		int pbarH = (int)((float)pval / maxVal * maxBarH);
		int py = baseY - pbarH - 20;

		int cx = startX + child * (barW + barGap) + barW / 2;
		int cval = arr[child];
		int cbarH = (int)((float)cval / maxVal * maxBarH);
		int cy = baseY - cbarH - 20;

		line(px, py, cx, cy);
	}

	void drawTitle() const
	{
		std::wstring title = L"堆排序演示";
		settextstyle(36, 0, L"微软雅黑");
		settextcolor(RGB(40, 40, 40));
		setbkmode(TRANSPARENT);

		int tw = textwidth(title.c_str());
		int tx = (1280 - tw) / 2;
		int ty = 25;
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
			info = L"点击“下一步”开始堆排序演示";
		}
		else if (currentStep >= (int)steps.size() - 1) {
			info = steps.back().desc;
		}
		else {
			info = L"步骤 " + std::to_wstring(currentStep + 1) + L" / " + std::to_wstring((int)steps.size() - 1) +
				L" ：" + steps[currentStep].desc;
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
