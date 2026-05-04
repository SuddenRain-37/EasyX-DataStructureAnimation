#pragma once

#include <graphics.h>
#include <vector>
#include <string>

#include "Project.h"
#include "Circle.h"
#include "Rectangle.h"
#include "Arrow.h"

class Pj5Scene : public Project
{
public:
	Pj5Scene() = default;
	~Pj5Scene() = default;

public:
	void on_enter() override {
		Project::on_enter();
		initAnimation();
	}

protected:
	void drawContent() override {
		drawTitle();

		// 绘制树边
		for (size_t i = 0; i < treeEdges.size(); i++) {
			if ((int)i <= currentStep * 2 + 1) {
				treeEdges[i].draw();
			}
		}

		// 绘制树节点
		for (size_t i = 0; i < treeNodes.size(); i++) {
			if ((int)i <= currentStep + 5) {
				treeNodes[i].circle.drawWithValue(treeNodes[i].value, BLACK, 16);
			}
		}

		// 绘制当前森林
		drawForestInfo();

		// 绘制步骤信息
		drawStepInfo();
	}

	void updateAnimation(int dalta) override {
	}

	void onPrevStep() override {
		if (currentStep > -1) {
			currentStep--;
			updateHighlight();
		}
	}

	void onNextStep() override {
		if (currentStep < maxSteps - 1) {
			currentStep++;
			updateHighlight();
		}
	}

	bool hasNextStep() const override {
		return currentStep < maxSteps - 1;
	}

	std::wstring getAlgorithmTitle() const override { return L"哈夫曼树"; }
	std::wstring getAlgorithmInfo() const override {
		return L"哈夫曼树（最优二叉树）是一种带权路径长度最短的二叉树，由哈夫曼于1952年提出。"
		       L"构造过程：每次从森林中选出两个权值最小的树合并为一棵新树，新树的权值为两子树根节点权值之和，重复此过程直到只剩一棵树。"
		       L"哈夫曼树广泛应用于数据压缩领域（如哈夫曼编码），频率高的字符使用短编码，频率低的字符使用长编码。"
		       L"本动画演示了权值 [5,9,12,13,16,45] 构造哈夫曼树的完整过程。黄色表示正在合并的节点，绿色表示已合并的节点。";
	}

private:
	struct TreeNode {
		Circle circle;
		int value = 0;
		int leftChild = -1;
		int rightChild = -1;
	};

	std::vector<TreeNode> treeNodes;
	std::vector<Arrow> treeEdges;

	int currentStep = -1;
	int maxSteps = 5;

	void initAnimation()
	{
		treeNodes.clear();
		treeEdges.clear();
		currentStep = -1;

		int radius = 26;

		// ========== 哈夫曼树节点 ==========
		// 初始权值：5, 9, 12, 13, 16, 45
		// 构造过程：
		//   Step0: 5+9=14
		//   Step1: 12+13=25
		//   Step2: 14+16=30
		//   Step3: 25+30=55
		//   Step4: 45+55=100
		//
		//        100
		//       /   \
		//     45     55
		//           /  \
		//         25    30
		//        / \    / \
		//      12  13  14  16
		//             / \
		//            5   9

		// 叶子节点（索引 0~5）
		struct NodeInit { int idx; int val; int x, y; };
		NodeInit leaves[] = {
			{ 0,  5,  840, 490 },
			{ 1,  9,  960, 490 },
			{ 2, 12,  640, 400 },
			{ 3, 13,  800, 400 },
			{ 4, 16, 1120, 400 },
			{ 5, 45,  400, 220 },
		};

		// 内部节点（索引 6~10）
		NodeInit internals[] = {
			{ 6, 14,  880, 400 },  // 5+9
			{ 7, 25,  720, 310 },  // 12+13
			{ 8, 30,  960, 310 },  // 14+16
			{ 9, 55,  800, 220 },  // 25+30
			{ 10, 100, 600, 130 }, // 45+55 (根)
		};

		treeNodes.resize(11);

		for (auto& n : leaves) {
			treeNodes[n.idx].circle.setPos(n.x, n.y);
			treeNodes[n.idx].circle.setRadius(radius);
			treeNodes[n.idx].value = n.val;
			treeNodes[n.idx].circle.setFillColor(WHITE);
			treeNodes[n.idx].circle.setBorderColor(BLACK);
		}

		for (auto& n : internals) {
			treeNodes[n.idx].circle.setPos(n.x, n.y);
			treeNodes[n.idx].circle.setRadius(radius);
			treeNodes[n.idx].value = n.val;
			treeNodes[n.idx].circle.setFillColor(WHITE);
			treeNodes[n.idx].circle.setBorderColor(BLACK);
		}

		// 设置父子关系
		treeNodes[6].leftChild = 0;   // 14 -> 5
		treeNodes[6].rightChild = 1;  // 14 -> 9
		treeNodes[7].leftChild = 2;   // 25 -> 12
		treeNodes[7].rightChild = 3;  // 25 -> 13
		treeNodes[8].leftChild = 6;   // 30 -> 14
		treeNodes[8].rightChild = 4;  // 30 -> 16
		treeNodes[9].leftChild = 7;   // 55 -> 25
		treeNodes[9].rightChild = 8;  // 55 -> 30
		treeNodes[10].leftChild = 5;  // 100 -> 45
		treeNodes[10].rightChild = 9; // 100 -> 55

		// 生成边：按照出现顺序
		// Step0: 边 6->0, 6->1
		addEdge(6, 0, radius);
		addEdge(6, 1, radius);
		// Step1: 边 7->2, 7->3
		addEdge(7, 2, radius);
		addEdge(7, 3, radius);
		// Step2: 边 8->6, 8->4
		addEdge(8, 6, radius);
		addEdge(8, 4, radius);
		// Step3: 边 9->7, 9->8
		addEdge(9, 7, radius);
		addEdge(9, 8, radius);
		// Step4: 边 10->5, 10->9
		addEdge(10, 5, radius);
		addEdge(10, 9, radius);
	}

	void addEdge(int parentIdx, int childIdx, int radius)
	{
		int x1 = treeNodes[parentIdx].circle.getX();
		int y1 = treeNodes[parentIdx].circle.getY() + radius;
		int x2 = treeNodes[childIdx].circle.getX();
		int y2 = treeNodes[childIdx].circle.getY() - radius;
		treeEdges.emplace_back(x1, y1, x2, y2, RGB(120, 120, 120));
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

		if (currentStep < 0) return;

		// 根据步骤高亮
		if (currentStep == 0) {
			// 高亮 5, 9, 新生成的 14
			setNodeColor(0, RGB(255, 220, 100), RGB(200, 150, 0));
			setNodeColor(1, RGB(255, 220, 100), RGB(200, 150, 0));
			setNodeColor(6, RGB(150, 230, 150), RGB(50, 150, 50));
			setEdgeColor(0, RED);
			setEdgeColor(1, RED);
		}
		else if (currentStep == 1) {
			// 高亮 12, 13, 新生成的 25
			setNodeColor(2, RGB(255, 220, 100), RGB(200, 150, 0));
			setNodeColor(3, RGB(255, 220, 100), RGB(200, 150, 0));
			setNodeColor(7, RGB(150, 230, 150), RGB(50, 150, 50));
			setEdgeColor(2, RED);
			setEdgeColor(3, RED);
			// 之前的保持浅绿
			setNodeColor(0, RGB(200, 245, 200), RGB(100, 180, 100));
			setNodeColor(1, RGB(200, 245, 200), RGB(100, 180, 100));
			setNodeColor(6, RGB(200, 245, 200), RGB(100, 180, 100));
			setEdgeColor(0, RGB(100, 180, 100));
			setEdgeColor(1, RGB(100, 180, 100));
		}
		else if (currentStep == 2) {
			// 高亮 14, 16, 新生成的 30
			setNodeColor(4, RGB(255, 220, 100), RGB(200, 150, 0));
			setNodeColor(6, RGB(255, 220, 100), RGB(200, 150, 0));
			setNodeColor(8, RGB(150, 230, 150), RGB(50, 150, 50));
			setEdgeColor(4, RED);
			setEdgeColor(5, RED);
			// 之前的保持浅绿
			setNodeColor(0, RGB(200, 245, 200), RGB(100, 180, 100));
			setNodeColor(1, RGB(200, 245, 200), RGB(100, 180, 100));
			setNodeColor(2, RGB(200, 245, 200), RGB(100, 180, 100));
			setNodeColor(3, RGB(200, 245, 200), RGB(100, 180, 100));
			setNodeColor(7, RGB(200, 245, 200), RGB(100, 180, 100));
			setEdgeColor(0, RGB(100, 180, 100));
			setEdgeColor(1, RGB(100, 180, 100));
			setEdgeColor(2, RGB(100, 180, 100));
			setEdgeColor(3, RGB(100, 180, 100));
		}
		else if (currentStep == 3) {
			// 高亮 25, 30, 新生成的 55
			setNodeColor(7, RGB(255, 220, 100), RGB(200, 150, 0));
			setNodeColor(8, RGB(255, 220, 100), RGB(200, 150, 0));
			setNodeColor(9, RGB(150, 230, 150), RGB(50, 150, 50));
			setEdgeColor(6, RED);
			setEdgeColor(7, RED);
			// 之前的保持浅绿
			for (int i = 0; i <= 6; i++) {
				if (i != 7 && i != 8) setNodeColor(i, RGB(200, 245, 200), RGB(100, 180, 100));
			}
			for (int i = 0; i <= 5; i++) setEdgeColor(i, RGB(100, 180, 100));
		}
		else if (currentStep == 4) {
			// 高亮 45, 55, 新生成的 100（根）
			setNodeColor(5, RGB(255, 220, 100), RGB(200, 150, 0));
			setNodeColor(9, RGB(255, 220, 100), RGB(200, 150, 0));
			setNodeColor(10, RGB(150, 230, 150), RGB(50, 150, 50));
			setEdgeColor(8, RED);
			setEdgeColor(9, RED);
			// 之前的保持浅绿
			for (int i = 0; i <= 8; i++) {
				if (i != 5 && i != 9) setNodeColor(i, RGB(200, 245, 200), RGB(100, 180, 100));
			}
			for (int i = 0; i <= 7; i++) setEdgeColor(i, RGB(100, 180, 100));
		}
	}

	void setNodeColor(int idx, COLORREF fill, COLORREF border)
	{
		if (idx >= 0 && idx < (int)treeNodes.size()) {
			treeNodes[idx].circle.setFillColor(fill);
			treeNodes[idx].circle.setBorderColor(border);
		}
	}

	void setEdgeColor(int idx, COLORREF color)
	{
		if (idx >= 0 && idx < (int)treeEdges.size()) {
			treeEdges[idx].setColor(color);
		}
	}

	void drawTitle() const
	{
		std::wstring title = L"哈夫曼树构造演示";
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

	void drawForestInfo() const
	{
		std::wstring forest = L"当前森林：{ ";
		if (currentStep < 0) {
			forest += L"5, 9, 12, 13, 16, 45";
		}
		else if (currentStep == 0) {
			forest += L"14, 12, 13, 16, 45";
		}
		else if (currentStep == 1) {
			forest += L"14, 25, 16, 45";
		}
		else if (currentStep == 2) {
			forest += L"25, 30, 45";
		}
		else if (currentStep == 3) {
			forest += L"45, 55";
		}
		else if (currentStep == 4) {
			forest += L"100";
		}
		forest += L" }";

		settextstyle(18, 0, L"微软雅黑");
		settextcolor(RGB(100, 100, 100));
		setbkmode(TRANSPARENT);
		outtextxy(100, 560, forest.c_str());
	}

	void drawStepInfo() const
	{
		std::wstring info;
		if (currentStep < 0) {
			info = L"点击“下一步”开始哈夫曼树构造演示。初始权值：5, 9, 12, 13, 16, 45";
		}
		else if (currentStep >= maxSteps) {
			info = L"哈夫曼树构造完成！根节点权值为 100";
		}
		else {
			switch (currentStep)
			{
			case 0: info = L"步骤 1/5：选出最小权值 5 和 9，合并生成新节点 14"; break;
			case 1: info = L"步骤 2/5：选出最小权值 12 和 13，合并生成新节点 25"; break;
			case 2: info = L"步骤 3/5：选出最小权值 14 和 16，合并生成新节点 30"; break;
			case 3: info = L"步骤 4/5：选出最小权值 25 和 30，合并生成新节点 55"; break;
			case 4: info = L"步骤 5/5：选出最小权值 45 和 55，合并生成根节点 100"; break;
			default: info = L""; break;
			}
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
