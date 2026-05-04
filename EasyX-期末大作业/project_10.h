#pragma once

#include <graphics.h>
#include <vector>
#include <string>

#include "Project.h"

class Pj10Scene : public Project
{
public:
	Pj10Scene() = default;
	~Pj10Scene() = default;
public:
	void on_enter() override {
		Project::on_enter();
		resetAnimation();
	}

	void resetAnimation() {
		currentStep = -1;
		steps.clear();

		// 原始数据
		orig = { 38, 27, 43, 3, 9, 82, 10, 55 };
		arr = orig;

		// 记录初始状态
		steps.push_back(MergeStep(0, (int)arr.size() - 1, -1, -1, L"初始数组", arr));

		// 执行归并排序并记录每一步
		mergeSort(0, (int)arr.size() - 1);

		// 最后一步：排序完成
		steps.push_back(MergeStep(0, (int)arr.size() - 1, -1, -1, L"排序完成！", arr));

		updateHighlight();
	}

	void drawContent() override {
		if (currentStep < 0) return;

		const MergeStep& step = steps[currentStep];

		drawTitle();
		drawArray(step);
		drawStepInfo(step);
	}

	void onNextStep() override {
		if (currentStep < (int)steps.size() - 1) {
			currentStep++;
			updateHighlight();
		}
	}

	void onPrevStep() override {
		if (currentStep > 0) {
			currentStep--;
			updateHighlight();
		}
	}

	bool hasNextStep() const override {
		return currentStep < (int)steps.size() - 1;
	}

	std::wstring getAlgorithmTitle() const override { return L"归并排序（递归与分治）"; }
	std::wstring getAlgorithmInfo() const override {
		return L"归并排序是一种典型的分治算法，由 John von Neumann 于1945年提出。核心思想是【分而治之】：将大问题分解为若干相似的子问题，递归求解后再合并结果。"
		       L"算法步骤：1）分解：将数组从中间分成两半；2）解决：递归地对两半分别排序；3）合并：将两个有序子数组合并为一个有序数组。"
		       L"时间复杂度：O(n log n)，空间复杂度：O(n)。归并排序是稳定排序，适合处理链表和外部排序。"
		       L"本动画演示了数组 [38,27,43,3,9,82,10,55] 的归并排序过程。橙色表示左半区间，粉色表示右半区间，绿色表示已排序合并的区间。";
	}

private:
	struct MergeStep {
		int left, right;    // 当前处理的区间
		int mid;            // 分割点（-1 表示没有）
		int mergePos;       // 当前合并到的位置（-1 表示没有）
		std::wstring desc;
		std::vector<int> values;

		MergeStep(int l, int r, int m, int mp, const std::wstring& d, const std::vector<int>& v)
			: left(l), right(r), mid(m), mergePos(mp), desc(d), values(v) {}
	};

	std::vector<int> orig;
	std::vector<int> arr;
	std::vector<MergeStep> steps;
	int currentStep = -1;

	void mergeSort(int left, int right) {
		if (left >= right) return;

		int mid = left + (right - left) / 2;

		// 记录分解步骤
		std::wstring splitDesc = L"分解: arr[";
		splitDesc.append(std::to_wstring(left));
		splitDesc.append(L"..");
		splitDesc.append(std::to_wstring(right));
		splitDesc.append(L"] 分成 [");
		splitDesc.append(std::to_wstring(left));
		splitDesc.append(L"..");
		splitDesc.append(std::to_wstring(mid));
		splitDesc.append(L"] 和 [");
		splitDesc.append(std::to_wstring(mid + 1));
		splitDesc.append(L"..");
		splitDesc.append(std::to_wstring(right));
		splitDesc.append(L"]");
		steps.push_back(MergeStep(left, right, mid, -1, splitDesc, arr));

		mergeSort(left, mid);
		mergeSort(mid + 1, right);
		merge(left, mid, right);
	}

	void merge(int left, int mid, int right) {
		std::vector<int> temp(right - left + 1);
		int i = left, j = mid + 1, k = 0;

		while (i <= mid && j <= right) {
			if (arr[i] <= arr[j]) {
				temp[k++] = arr[i++];
			}
			else {
				temp[k++] = arr[j++];
			}
		}
		while (i <= mid) temp[k++] = arr[i++];
		while (j <= right) temp[k++] = arr[j++];

		for (int p = 0; p < k; p++) {
			arr[left + p] = temp[p];
		}

		// 记录合并完成步骤
		std::wstring mergeDesc = L"合并: arr[";
		mergeDesc.append(std::to_wstring(left));
		mergeDesc.append(L"..");
		mergeDesc.append(std::to_wstring(right));
		mergeDesc.append(L"] 已排序");
		steps.push_back(MergeStep(left, right, mid, -1, mergeDesc, arr));
	}

	void updateHighlight() {
		// 不需要额外状态更新，步骤数据已经包含所有信息
	}

	void drawTitle() const
	{
		std::wstring title = L"归并排序演示";
		settextstyle(36, 0, L"微软雅黑");
		settextcolor(RGB(40, 40, 40));
		setbkmode(TRANSPARENT);

		int tw = textwidth(title.c_str());
		int tx = (1280 - tw) / 2;
		int ty = 30;
		outtextxy(tx, ty, title.c_str());

		// 装饰线
		setlinecolor(RGB(70, 130, 180));
		setlinestyle(PS_SOLID, 3);
		int lineW = tw + 30;
		int lineX = (1280 - lineW) / 2;
		line(lineX, ty + 50, lineX + lineW, ty + 50);
	}

	void drawArray(const MergeStep& step) {
		int n = (int)step.values.size();
		if (n == 0) return;

		int barW = 70;
		int gap = 20;
		int totalW = n * barW + (n - 1) * gap;
		int startX = (1280 - totalW) / 2;
		int baseY = 500;
		int maxVal = 100;
		int maxBarH = 300;

		for (int i = 0; i < n; i++) {
			int barH = (int)((double)step.values[i] / maxVal * maxBarH);
			int x = startX + i * (barW + gap);
			int y = baseY - barH;

			// 根据当前步骤决定颜色
			COLORREF fillColor = RGB(100, 149, 237); // 默认蓝色
			COLORREF borderColor = RGB(70, 130, 180);

			if (i >= step.left && i <= step.right) {
				if (step.mid >= 0 && i <= step.mid) {
					fillColor = RGB(255, 165, 0); // 左半部分橙色
					borderColor = RGB(200, 120, 0);
				}
				else if (step.mid >= 0) {
					fillColor = RGB(255, 105, 180); // 右半部分粉色
					borderColor = RGB(200, 60, 130);
				}
				else {
					fillColor = RGB(144, 238, 144); // 合并完成浅绿色
					borderColor = RGB(50, 180, 50);
				}
			}
			else {
				fillColor = RGB(200, 200, 200); // 未处理灰色
				borderColor = RGB(160, 160, 160);
			}

			// 绘制柱子
			setfillcolor(fillColor);
			setlinecolor(borderColor);
			solidroundrect(x, y, x + barW, baseY, 6, 6);

			// 绘制数值
			settextstyle(18, 0, L"微软雅黑");
			settextcolor(BLACK);
			setbkmode(TRANSPARENT);
			std::wstring valStr = std::to_wstring(step.values[i]);
			int tw = textwidth(valStr.c_str());
			outtextxy(x + (barW - tw) / 2, y - 22, valStr.c_str());

			// 绘制下标
			settextcolor(RGB(100, 100, 100));
			std::wstring idxStr = std::to_wstring(i);
			tw = textwidth(idxStr.c_str());
			outtextxy(x + (barW - tw) / 2, baseY + 6, idxStr.c_str());
		}

		// 绘制区间范围标记
		if (step.left >= 0 && step.right >= 0 && step.left < n && step.right < n) {
			int leftX = startX + step.left * (barW + gap);
			int rightX = startX + step.right * (barW + gap) + barW;

			setlinecolor(RGB(80, 80, 80));
			setlinestyle(PS_DASH, 2);
			line(leftX - 5, baseY + 35, rightX + 5, baseY + 35);

			// 左括号
			setlinestyle(PS_SOLID, 2);
			line(leftX - 5, baseY + 30, leftX - 5, baseY + 40);
			// 右括号
			line(rightX + 5, baseY + 30, rightX + 5, baseY + 40);
		}
	}

	void drawStepInfo(const MergeStep& step) {
		int ty = 620;
		settextstyle(20, 0, L"微软雅黑");
		settextcolor(RGB(50, 50, 50));
		setbkmode(TRANSPARENT);
		outtextxy(60, ty, step.desc.c_str());

		std::wstring progress = L"步骤: ";
		progress.append(std::to_wstring(currentStep + 1));
		progress.append(L" / ");
		progress.append(std::to_wstring((int)steps.size()));
		int pw = textwidth(progress.c_str());
		outtextxy(1280 - 60 - pw, ty, progress.c_str());
	}
};