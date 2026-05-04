#pragma once

#include <graphics.h>
#include <vector>
#include <string>

#include "Project.h"
#include "Circle.h"
#include "Rectangle.h"
#include "Arrow.h"

class Pj3Scene : public Project
{
public:
	Pj3Scene() = default;
	~Pj3Scene() = default;

public:
	void on_enter() override {
		Project::on_enter();
		initAnimation();
	}

protected:
	void drawContent() override {
		drawTitle();

		// 绘制树边
		for (auto& edge : treeEdges) {
			edge.draw();
		}

		// 绘制树节点
		for (auto& node : treeNodes) {
			node.circle.drawWithValue(node.value, BLACK, 18);
		}

		// 绘制遍历顺序（底部结果栏）
		drawTraversalResult();

		// 绘制步骤信息
		drawStepInfo();
	}

	void updateAnimation(int dalta) override {
		// 动画更新逻辑（如需要平滑移动可在此实现）
	}

	void onPrevStep() override {
		if (currentStep > 0) {
			currentStep--;
			updateHighlight();
		}
	}

	void onNextStep() override {
		if (currentStep < (int)traversalSteps.size() - 1) {
			currentStep++;
			updateHighlight();
		}
	}

	bool hasNextStep() const override {
		return currentStep < (int)traversalSteps.size() - 1;
	}

	std::wstring getAlgorithmTitle() const override { return L"二叉树先序遍历"; }
	std::wstring getAlgorithmInfo() const override {
		return L"先序遍历是二叉树遍历的一种方式，遍历顺序为：根节点 → 左子树 → 右子树。"
		       L"先序遍历常用于复制二叉树或输出树的结构表示。本动画演示了二叉树 [50,25,75,10,30,60] 的先序遍历过程。"
		       L"遍历结果应为：50, 25, 10, 30, 75, 60。绿色表示已访问的节点，黄色表示正在回溯准备遍历子树的节点。";
	}

private:
	struct TreeNode {
		Circle circle;
		int value = 0;
		int leftChild = -1;
		int rightChild = -1;
	};

	struct TraversalStep {
		int nodeIdx;           // 当前访问的节点索引
		int orderIdx;          // 这是第几个被遍历的节点（-1表示正在访问但未记录）
		std::wstring desc;     // 步骤描述
	};

	std::vector<TreeNode> treeNodes;
	std::vector<Arrow> treeEdges;
	std::vector<TraversalStep> traversalSteps;
	std::vector<int> traversalOrder; // 记录遍历顺序的节点值

	int currentStep = -1;

	// 结果栏布局
	int resultStartX = 0;
	int resultStartY = 520;
	int resultCellW = 70;
	int resultCellH = 50;
	int resultGap = 20;

	void initAnimation()
	{
		treeNodes.clear();
		treeEdges.clear();
		traversalSteps.clear();
		traversalOrder.clear();
		currentStep = -1;

		// ========== 初始化二叉树 ==========
		// 树结构：
		//        50
		//       /  \
		//     25    75
		//    / \    /
		//   10 30  60
		int treeValues[] = { 50, 25, 75, 10, 30, 60 };
		int treeCount = sizeof(treeValues) / sizeof(treeValues[0]);
		int radius = 32;
		int treeCenterX = 1280 / 2;
		int treeStartY = 140;
		int levelHeight = 110;
		int levelGap[] = { 0, 220, 110 };

		// 节点位置（手动布局）
		int posX[] = {
			treeCenterX,
			treeCenterX - levelGap[1], treeCenterX + levelGap[1],
			treeCenterX - levelGap[1] - levelGap[2],
			treeCenterX - levelGap[1] + levelGap[2],
			treeCenterX + levelGap[1] - levelGap[2]
		};
		int posY[] = {
			treeStartY,
			treeStartY + levelHeight, treeStartY + levelHeight,
			treeStartY + levelHeight * 2, treeStartY + levelHeight * 2,
			treeStartY + levelHeight * 2
		};

		for (int i = 0; i < treeCount; i++) {
			TreeNode node;
			node.circle.setPos(posX[i], posY[i]);
			node.circle.setRadius(radius);
			node.value = treeValues[i];
			node.circle.setFillColor(WHITE);
			node.circle.setBorderColor(BLACK);
			treeNodes.push_back(node);
		}

		// 设置父子关系
		treeNodes[0].leftChild = 1;
		treeNodes[0].rightChild = 2;
		treeNodes[1].leftChild = 3;
		treeNodes[1].rightChild = 4;
		treeNodes[2].leftChild = 5;

		// 绘制树边
		for (int i = 0; i < treeCount; i++) {
			if (treeNodes[i].leftChild >= 0) {
				int child = treeNodes[i].leftChild;
				int x1 = treeNodes[i].circle.getX();
				int y1 = treeNodes[i].circle.getY() + radius;
				int x2 = treeNodes[child].circle.getX();
				int y2 = treeNodes[child].circle.getY() - radius;
				treeEdges.emplace_back(x1, y1, x2, y2, RGB(120, 120, 120));
			}
			if (treeNodes[i].rightChild >= 0) {
				int child = treeNodes[i].rightChild;
				int x1 = treeNodes[i].circle.getX();
				int y1 = treeNodes[i].circle.getY() + radius;
				int x2 = treeNodes[child].circle.getX();
				int y2 = treeNodes[child].circle.getY() - radius;
				treeEdges.emplace_back(x1, y1, x2, y2, RGB(120, 120, 120));
			}
		}

		// ========== 生成先序遍历步骤 ==========
		// 先序遍历：根 -> 左 -> 右
		// 结果应为：50, 25, 10, 30, 75, 60
		generatePreorderSteps(0);

		// 初始化结果栏位置
		int orderCount = (int)traversalOrder.size();
		resultStartX = (1280 - (orderCount * resultCellW + (orderCount - 1) * resultGap)) / 2;
	}

	void generatePreorderSteps(int nodeIdx)
	{
		if (nodeIdx < 0 || nodeIdx >= (int)treeNodes.size()) return;

		// 1. 先访问当前节点（记录到遍历顺序中）—— 先序遍历的核心
		int orderIdx = (int)traversalOrder.size();
		traversalOrder.push_back(treeNodes[nodeIdx].value);
		traversalSteps.push_back({ nodeIdx, orderIdx,
			L"访问节点 " + std::to_wstring(treeNodes[nodeIdx].value) + L"（第 " + std::to_wstring(orderIdx + 1) + L" 个）" });

		// 2. 遍历左子树
		if (treeNodes[nodeIdx].leftChild >= 0) {
			traversalSteps.push_back({ nodeIdx, -1,
				L"节点 " + std::to_wstring(treeNodes[nodeIdx].value) + L" 已访问，准备遍历左子树" });
			generatePreorderSteps(treeNodes[nodeIdx].leftChild);
		}

		// 3. 遍历右子树
		if (treeNodes[nodeIdx].rightChild >= 0) {
			traversalSteps.push_back({ nodeIdx, -1,
				L"节点 " + std::to_wstring(treeNodes[nodeIdx].value) + L" 左子树遍历完毕，准备遍历右子树" });
			generatePreorderSteps(treeNodes[nodeIdx].rightChild);
		}
	}

	void updateHighlight()
	{
		// 重置所有颜色
		for (auto& node : treeNodes) {
			node.circle.setFillColor(WHITE);
			node.circle.setBorderColor(BLACK);
		}
		for (auto& edge : treeEdges) {
			edge.setColor(RGB(120, 120, 120));
		}

		if (currentStep < 0 || currentStep >= (int)traversalSteps.size()) return;

		const auto& step = traversalSteps[currentStep];
		int idx = step.nodeIdx;

		if (idx >= 0 && idx < (int)treeNodes.size()) {
			if (step.orderIdx >= 0) {
				// 已正式访问（记录到遍历顺序中）：绿色高亮
				treeNodes[idx].circle.setFillColor(RGB(150, 230, 150));
				treeNodes[idx].circle.setBorderColor(RGB(50, 150, 50));
			}
			else {
				// 正在回溯/准备遍历子树：黄色高亮
				treeNodes[idx].circle.setFillColor(RGB(255, 220, 100));
				treeNodes[idx].circle.setBorderColor(RGB(200, 150, 0));
			}
		}

		// 已遍历过的节点保持浅绿色
		for (const auto& s : traversalSteps) {
			if (s.orderIdx >= 0 && s.nodeIdx != idx) {
				treeNodes[s.nodeIdx].circle.setFillColor(RGB(200, 245, 200));
			}
		}
	}

	void drawTitle() const
	{
		std::wstring title = L"二叉树先序遍历演示";
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

	void drawTraversalResult() const
	{
		// 绘制结果栏背景说明
		settextstyle(18, 0, L"微软雅黑");
		settextcolor(RGB(100, 100, 100));
		setbkmode(TRANSPARENT);
		std::wstring label = L"遍历顺序（先序：根 → 左 → 右）：";
		outtextxy(resultStartX, resultStartY - 30, label.c_str());

		// 绘制每个结果格子
		for (size_t i = 0; i < traversalOrder.size(); i++) {
			int cx = resultStartX + (int)i * (resultCellW + resultGap);
			int cy = resultStartY;

			// 判断这个格子是否已经被遍历到
			bool visited = false;
			if (currentStep >= 0) {
				for (int s = 0; s <= currentStep; s++) {
					if (traversalSteps[s].orderIdx == (int)i) {
						visited = true;
						break;
					}
				}
			}

			// 绘制格子背景
			if (visited) {
				setfillcolor(RGB(150, 230, 150));
				setlinecolor(RGB(50, 150, 50));
			}
			else {
				setfillcolor(WHITE);
				setlinecolor(RGB(180, 180, 180));
			}
			setlinestyle(PS_SOLID, 2);
			fillrectangle(cx, cy, cx + resultCellW, cy + resultCellH);

			// 绘制格子中的数字
			if (visited) {
				settextcolor(BLACK);
			}
			else {
				settextcolor(RGB(180, 180, 180));
			}
			settextstyle(20, 0, L"微软雅黑");
			setbkmode(TRANSPARENT);
			std::wstring num = std::to_wstring(traversalOrder[i]);
			int tw = textwidth(num.c_str());
			int th = textheight(num.c_str());
			outtextxy(cx + (resultCellW - tw) / 2, cy + (resultCellH - th) / 2, num.c_str());
		}
	}

	void drawStepInfo() const
	{
		std::wstring info;
		if (currentStep < 0) {
			info = L"点击“下一步”开始先序遍历演示";
		}
		else if (currentStep >= (int)traversalSteps.size()) {
			info = L"遍历完成！结果：50, 25, 10, 30, 75, 60";
		}
		else {
			info = L"步骤 " + std::to_wstring(currentStep + 1) + L" / " + std::to_wstring(traversalSteps.size()) +
			       L" ：" + traversalSteps[currentStep].desc;
		}

		settextstyle(20, 0, L"微软雅黑");
		settextcolor(RGB(80, 80, 80));
		setbkmode(TRANSPARENT);

		int tw = textwidth(info.c_str());
		int tx = (1280 - tw) / 2;
		int ty = 610;
		outtextxy(tx, ty, info.c_str());
	}
};
