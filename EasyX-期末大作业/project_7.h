#pragma once

#include <graphics.h>
#include <vector>
#include <string>
#include <climits>

#include "Project.h"
#include "Circle.h"
#include "Arrow.h"

class Pj7Scene : public Project
{
public:
	Pj7Scene() = default;
	~Pj7Scene() = default;

public:
	void on_enter() override {
		Project::on_enter();
		initAnimation();
	}

protected:
	void drawContent() override {
		drawTitle();

		// 绘制边
		for (size_t i = 0; i < graphEdges.size(); i += 2) { // 无向图，只画一次
			drawEdge(graphEdges[i]);
		}

		// 绘制节点
		for (auto& node : graphNodes) {
			node.circle.drawWithText(node.name, BLACK, 20);
		}

		// 绘制距离矩阵
		drawMatrix();

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
		if (currentStep < (int)steps.size() - 1) {
			currentStep++;
			updateHighlight();
		}
	}

	bool hasNextStep() const override {
		return currentStep < (int)steps.size() - 1;
	}

	std::wstring getAlgorithmTitle() const override { return L"Floyd 最短路径"; }
	std::wstring getAlgorithmInfo() const override {
		return L"Floyd 算法（Floyd-Warshall）是一种动态规划算法，用于求解图中所有节点对之间的最短路径。"
		       L"算法核心思想：逐步尝试将每个节点作为中间点，更新所有节点对之间的最短距离。状态转移方程：dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j])。"
		       L"时间复杂度：O(V³)，空间复杂度：O(V²)。优点是可以一次求出所有节点对的最短路径，且能处理负权边（但不能有负权环）。"
		       L"本动画演示了 Floyd 算法在4节点图中的执行过程，右侧距离矩阵实时更新，绿色高亮表示本次缩短的路径。";
	}

private:
	struct GraphNode {
		Circle circle;
		std::wstring name;
	};

	struct GraphEdge {
		int from;
		int to;
		int weight;
	};

	struct FloydStep {
		int k;  // 中间节点，-1表示初始化
		std::vector<std::tuple<int, int, int>> updates; // (i,j,newDist)
		std::wstring desc;
	};

	std::vector<GraphNode> graphNodes;
	std::vector<GraphEdge> graphEdges;
	std::vector<FloydStep> steps;
	std::vector<std::vector<std::vector<int>>> matrixHistory; // [step][i][j]

	int currentStep = -1;

	void initAnimation()
	{
		graphNodes.clear();
		graphEdges.clear();
		steps.clear();
		matrixHistory.clear();
		currentStep = -1;

		// 初始化4个节点（十字形布局）
		struct NodeInit { std::wstring name; int x, y; };
		NodeInit nodes[] = {
			{ L"A", 220, 280 },
			{ L"B", 420, 140 },
			{ L"C", 620, 280 },
			{ L"D", 420, 420 },
		};

		int radius = 32;
		for (auto& n : nodes) {
			GraphNode gn;
			gn.circle.setPos(n.x, n.y);
			gn.circle.setRadius(radius);
			gn.name = n.name;
			gn.circle.setFillColor(WHITE);
			gn.circle.setBorderColor(BLACK);
			graphNodes.push_back(gn);
		}

		// 初始化边（无向图，双向存储）
		GraphEdge edges[] = {
			{ 0, 1, 5 },  // A-B
			{ 1, 2, 3 },  // B-C
			{ 2, 3, 4 },  // C-D
			{ 0, 3, 10 }, // A-D
		};

		for (auto& e : edges) {
			graphEdges.push_back(e);
			graphEdges.push_back({ e.to, e.from, e.weight });
		}

		// 生成 Floyd 步骤
		generateFloydSteps();
	}

	void generateFloydSteps()
	{
		int n = 4;
		const int INF = 1000000;

		// 初始化距离矩阵
		std::vector<std::vector<int>> dist(n, std::vector<int>(n, INF));
		for (int i = 0; i < n; i++) dist[i][i] = 0;
		for (auto& e : graphEdges) {
			if (e.weight < dist[e.from][e.to]) {
				dist[e.from][e.to] = e.weight;
			}
		}

		// Step 0: 初始化
		steps.push_back({ -1, {}, L"初始化距离矩阵，对角线为0，直连边填入权重，不连通的为∞" });
		matrixHistory.push_back(dist);

		// Floyd 主循环
		for (int k = 0; k < n; k++) {
			std::vector<std::tuple<int, int, int>> updates;
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < n; j++) {
					if (i == j || i == k || j == k) continue;
					if (dist[i][k] + dist[k][j] < dist[i][j]) {
						dist[i][j] = dist[i][k] + dist[k][j];
						updates.push_back({ i, j, dist[i][j] });
					}
				}
			}

			std::wstring desc = L"以 " + graphNodes[k].name + L" 为中间点";
			if (updates.empty()) {
				desc += L"：无路径可缩短";
			}
			else {
				desc += L"：";
				for (size_t u = 0; u < updates.size(); u++) {
					int ii = std::get<0>(updates[u]);
					int jj = std::get<1>(updates[u]);
					int dd = std::get<2>(updates[u]);
					if (u > 0) desc += L"，";
					desc += graphNodes[ii].name + L"→" + graphNodes[jj].name + L" 缩短为 " + std::to_wstring(dd);
				}
			}
			steps.push_back({ k, updates, desc });
			matrixHistory.push_back(dist);
		}
	}

	void updateHighlight()
	{
		// 重置
		for (auto& node : graphNodes) {
			node.circle.setFillColor(WHITE);
			node.circle.setBorderColor(BLACK);
		}

		if (currentStep < 0) return;

		const auto& step = steps[currentStep];

		// 高亮中间节点 k（蓝色）
		if (step.k >= 0) {
			graphNodes[step.k].circle.setFillColor(RGB(200, 230, 255));
			graphNodes[step.k].circle.setBorderColor(RGB(70, 130, 180));
		}

		// 高亮更新的源节点和目标节点（黄色）
		for (auto& up : step.updates) {
			int i = std::get<0>(up);
			int j = std::get<1>(up);
			graphNodes[i].circle.setFillColor(RGB(255, 220, 100));
			graphNodes[i].circle.setBorderColor(RGB(200, 150, 0));
			graphNodes[j].circle.setFillColor(RGB(255, 220, 100));
			graphNodes[j].circle.setBorderColor(RGB(200, 150, 0));
		}
	}

	void drawEdge(const GraphEdge& edge) const
	{
		int x1 = graphNodes[edge.from].circle.getX();
		int y1 = graphNodes[edge.from].circle.getY();
		int x2 = graphNodes[edge.to].circle.getX();
		int y2 = graphNodes[edge.to].circle.getY();
		int radius = graphNodes[edge.from].circle.getRadius();

		float dx = (float)(x2 - x1);
		float dy = (float)(y2 - y1);
		float dist = sqrt(dx * dx + dy * dy);
		if (dist < 1) return;

		float nx = dx / dist;
		float ny = dy / dist;

		int sx = (int)(x1 + nx * radius);
		int sy = (int)(y1 + ny * radius);
		int ex = (int)(x2 - nx * radius);
		int ey = (int)(y2 - ny * radius);

		setlinecolor(RGB(150, 150, 150));
		setlinestyle(PS_SOLID, 2);
		line(sx, sy, ex, ey);

		// 权重
		int mx = (sx + ex) / 2;
		int my = (sy + ey) / 2;
		std::wstring wstr = std::to_wstring(edge.weight);
		settextstyle(16, 0, L"微软雅黑");
		settextcolor(RGB(100, 100, 100));
		setbkmode(TRANSPARENT);
		int tw = textwidth(wstr.c_str());
		int th = textheight(wstr.c_str());
		outtextxy(mx - tw / 2, my - th / 2 - 8, wstr.c_str());
	}

	void drawMatrix() const
	{
		if (currentStep < 0 || currentStep >= (int)matrixHistory.size()) return;

		const auto& mat = matrixHistory[currentStep];
		int n = (int)mat.size();
		const int INF = 1000000;

		int cellW = 70;
		int cellH = 32;
		int startX = 800;
		int startY = 230;

		// 标题
		settextstyle(18, 0, L"微软雅黑");
		settextcolor(RGB(60, 60, 60));
		setbkmode(TRANSPARENT);
		outtextxy(startX, startY - 28, L"距离矩阵：");

		// 列标题
		for (int j = 0; j < n; j++) {
			int cx = startX + (j + 1) * cellW;
			int cy = startY;
			setfillcolor(RGB(230, 240, 250));
			setlinecolor(RGB(70, 130, 180));
			fillrectangle(cx, cy, cx + cellW, cy + cellH);
			settextcolor(RGB(40, 40, 40));
			settextstyle(16, 0, L"微软雅黑");
			int tw = textwidth(graphNodes[j].name.c_str());
			outtextxy(cx + (cellW - tw) / 2, cy + (cellH - textheight(graphNodes[j].name.c_str())) / 2, graphNodes[j].name.c_str());
		}

		// 行
		for (int i = 0; i < n; i++) {
			// 行标题
			int cx = startX;
			int cy = startY + (i + 1) * cellH;
			setfillcolor(RGB(230, 240, 250));
			setlinecolor(RGB(70, 130, 180));
			fillrectangle(cx, cy, cx + cellW, cy + cellH);
			settextcolor(RGB(40, 40, 40));
			settextstyle(16, 0, L"微软雅黑");
			int tw = textwidth(graphNodes[i].name.c_str());
			outtextxy(cx + (cellW - tw) / 2, cy + (cellH - textheight(graphNodes[i].name.c_str())) / 2, graphNodes[i].name.c_str());

			for (int j = 0; j < n; j++) {
				int cx = startX + (j + 1) * cellW;
				int cy = startY + (i + 1) * cellH;

				// 判断是否是本次更新的单元格
				bool isUpdated = false;
				bool isKRowOrCol = false;
				if (currentStep > 0) {
					const auto& step = steps[currentStep];
					if (step.k == i || step.k == j) isKRowOrCol = true;
					for (auto& up : step.updates) {
						if (std::get<0>(up) == i && std::get<1>(up) == j) {
							isUpdated = true;
							break;
						}
					}
				}

				if (isUpdated) {
					setfillcolor(RGB(150, 230, 150));
					setlinecolor(RGB(50, 150, 50));
				}
				else if (isKRowOrCol && currentStep > 0) {
					setfillcolor(RGB(255, 250, 220));
					setlinecolor(RGB(200, 180, 150));
				}
				else {
					setfillcolor(WHITE);
					setlinecolor(RGB(200, 200, 200));
				}

				fillrectangle(cx, cy, cx + cellW, cy + cellH);

				std::wstring valStr;
				if (mat[i][j] >= INF / 2) {
					valStr = L"∞";
					settextcolor(RGB(180, 180, 180));
				}
				else {
					valStr = std::to_wstring(mat[i][j]);
					settextcolor(isUpdated ? RGB(50, 150, 50) : RGB(40, 40, 40));
				}

				settextstyle(16, 0, L"微软雅黑");
				setbkmode(TRANSPARENT);
				int tw = textwidth(valStr.c_str());
				int th = textheight(valStr.c_str());
				outtextxy(cx + (cellW - tw) / 2, cy + (cellH - th) / 2, valStr.c_str());
			}
		}
	}

	void drawTitle() const
	{
		std::wstring title = L"Floyd 最短路径演示";
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
			info = L"点击“下一步”开始 Floyd 最短路径演示";
		}
		else if (currentStep >= (int)steps.size() - 1) {
			info = L"算法完成！已求得所有节点对之间的最短距离";
		}
		else {
			info = L"步骤 " + std::to_wstring(currentStep + 1) + L" / " + std::to_wstring(steps.size() - 1) +
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
