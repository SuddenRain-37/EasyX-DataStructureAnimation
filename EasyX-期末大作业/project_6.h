#pragma once

#include <graphics.h>
#include <vector>
#include <string>
#include <climits>

#include "Project.h"
#include "Circle.h"
#include "Rectangle.h"
#include "Arrow.h"

class Pj6Scene : public Project
{
public:
	Pj6Scene() = default;
	~Pj6Scene() = default;

public:
	void on_enter() override {
		Project::on_enter();
		initAnimation();
	}

protected:
	void drawContent() override {
		drawTitle();

		// 绘制边和权重
		for (auto& edge : graphEdges) {
			drawEdge(edge);
		}

		// 绘制节点
		for (auto& node : graphNodes) {
			node.circle.drawWithText(node.name, BLACK, 20);
		}

		// 绘制距离表格
		drawDistTable();

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

	std::wstring getAlgorithmTitle() const override { return L"Dijkstra 最短路径"; }
	std::wstring getAlgorithmInfo() const override {
		return L"Dijkstra 算法是由荷兰计算机科学家 Edsger Dijkstra 于1956年提出的单源最短路径算法。"
		       L"算法核心思想：维护一个已确定最短距离的节点集合，每次从未确定集合中选出距离起点最近的节点加入确定集合，并用该节点更新其邻居的距离。"
		       L"时间复杂度：使用优先队列时为 O((V+E)logV)。仅适用于边权为非负数的图。"
		       L"本动画以 A 为起点，演示 Dijkstra 算法在6节点图中的执行过程。绿色表示已确定最短距离的节点，黄色表示本次更新的邻居。";
	}

private:
	struct GraphNode {
		Circle circle;
		std::wstring name;
		int dist = INT_MAX;
		bool visited = false;
	};

	struct GraphEdge {
		int from;
		int to;
		int weight;
	};

	struct Step {
		int selectedNode;
		std::vector<int> updatedNodes;
		std::wstring desc;
	};

	std::vector<GraphNode> graphNodes;
	std::vector<GraphEdge> graphEdges;
	std::vector<Step> steps;

	int currentStep = -1;

	void initAnimation()
	{
		graphNodes.clear();
		graphEdges.clear();
		steps.clear();
		currentStep = -1;

		// ========== 初始化图节点 ==========
		// 布局：
		//    B --- D --- F
		//   / \   / \   /
		//  A   C --- E
		struct NodeInit { std::wstring name; int x, y; };
		NodeInit nodes[] = {
			{ L"A", 200, 280 },
			{ L"B", 500, 120 },
			{ L"C", 500, 440 },
			{ L"D", 800, 120 },
			{ L"E", 800, 440 },
			{ L"F", 1100, 280 },
		};

		int radius = 32;
		for (auto& n : nodes) {
			GraphNode gn;
			gn.circle.setPos(n.x, n.y);
			gn.circle.setRadius(radius);
			gn.name = n.name;
			gn.dist = INT_MAX;
			gn.visited = false;
			gn.circle.setFillColor(WHITE);
			gn.circle.setBorderColor(BLACK);
			graphNodes.push_back(gn);
		}

		// ========== 初始化边（无向图，双向存储） ==========
		struct EdgeInit { int from, to, weight; };
		EdgeInit edges[] = {
			{ 0, 1, 4 },  // A-B
			{ 0, 2, 2 },  // A-C
			{ 1, 2, 1 },  // B-C
			{ 1, 3, 5 },  // B-D
			{ 2, 3, 8 },  // C-D
			{ 2, 4, 10 }, // C-E
			{ 3, 4, 2 },  // D-E
			{ 3, 5, 6 },  // D-F
			{ 4, 5, 3 },  // E-F
		};

		for (auto& e : edges) {
			graphEdges.push_back({ e.from, e.to, e.weight });
		}

		// ========== 生成 Dijkstra 步骤 ==========
		// 起点 A(0)
		generateDijkstraSteps();
	}

	void generateDijkstraSteps()
	{
		int n = (int)graphNodes.size();
		std::vector<int> dist(n, INT_MAX);
		std::vector<bool> visited(n, false);
		std::vector<int> prev(n, -1);

		// Step 0: 初始化，起点 A 距离为 0
		dist[0] = 0;
		steps.push_back({ -1, {}, L"初始化：起点 A 的距离设为 0，其余为 ∞" });

		for (int iter = 0; iter < n; iter++) {
			// 找未访问的最小距离节点
			int u = -1;
			int minDist = INT_MAX;
			for (int i = 0; i < n; i++) {
				if (!visited[i] && dist[i] < minDist) {
					minDist = dist[i];
					u = i;
				}
			}

			if (u == -1) break;

			visited[u] = true;

			// 更新邻居
			std::vector<int> updated;
			for (auto& e : graphEdges) {
				int v = -1;
				if (e.from == u && !visited[e.to]) v = e.to;
				else if (e.to == u && !visited[e.from]) v = e.from;

				if (v != -1 && dist[u] != INT_MAX) {
					int newDist = dist[u] + e.weight;
					if (newDist < dist[v]) {
						dist[v] = newDist;
						prev[v] = u;
						updated.push_back(v);
					}
				}
			}

			std::wstring desc = L"选择节点 " + graphNodes[u].name +
				L"（距离=" + (dist[u] == INT_MAX ? L"∞" : std::to_wstring(dist[u])) +
				L"），标记为已确定";
			if (!updated.empty()) {
				desc += L"，更新邻居距离";
			}
			steps.push_back({ u, updated, desc });
		}

		// 记录最终距离到节点
		for (int i = 0; i < n; i++) {
			graphNodes[i].dist = dist[i];
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

		// 已确定的节点：绿色
		for (int s = 0; s <= currentStep; s++) {
			if (steps[s].selectedNode >= 0) {
				int idx = steps[s].selectedNode;
				graphNodes[idx].circle.setFillColor(RGB(150, 230, 150));
				graphNodes[idx].circle.setBorderColor(RGB(50, 150, 50));
			}
		}

		// 当前步骤选中的节点：深绿色
		if (steps[currentStep].selectedNode >= 0) {
			int idx = steps[currentStep].selectedNode;
			graphNodes[idx].circle.setFillColor(RGB(80, 200, 80));
			graphNodes[idx].circle.setBorderColor(RGB(30, 120, 30));
		}

		// 本次更新的节点：黄色
		for (int v : steps[currentStep].updatedNodes) {
			graphNodes[v].circle.setFillColor(RGB(255, 220, 100));
			graphNodes[v].circle.setBorderColor(RGB(200, 150, 0));
		}
	}

	void drawEdge(const GraphEdge& edge) const
	{
		int x1 = graphNodes[edge.from].circle.getX();
		int y1 = graphNodes[edge.from].circle.getY();
		int x2 = graphNodes[edge.to].circle.getX();
		int y2 = graphNodes[edge.to].circle.getY();
		int radius = graphNodes[edge.from].circle.getRadius();

		// 计算从圆心到圆边缘的交点
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

		// 绘制边
		setlinecolor(RGB(150, 150, 150));
		setlinestyle(PS_SOLID, 2);
		line(sx, sy, ex, ey);

		// 绘制权重（中点）
		int mx = (sx + ex) / 2;
		int my = (sy + ey) / 2;
		std::wstring wstr = std::to_wstring(edge.weight);
		settextstyle(16, 0, L"微软雅黑");
		settextcolor(RGB(100, 100, 100));
		setbkmode(TRANSPARENT);
		int tw = textwidth(wstr.c_str());
		int th = textheight(wstr.c_str());
		outtextxy(mx - tw / 2, my - th / 2, wstr.c_str());
	}

	void drawDistTable() const
	{
		int startX = 270;
		int startY = 500;
		int cellW = 130;
		int cellH = 40;

		settextstyle(18, 0, L"微软雅黑");
		settextcolor(RGB(60, 60, 60));
		setbkmode(TRANSPARENT);
		outtextxy(startX, startY - 25, L"当前最短距离：");

		for (size_t i = 0; i < graphNodes.size(); i++) {
			int cx = startX + (int)i * cellW;
			int cy = startY;

			// 节点名背景
			setfillcolor(RGB(230, 240, 250));
			setlinecolor(RGB(70, 130, 180));
			setlinestyle(PS_SOLID, 2);
			fillrectangle(cx, cy, cx + cellW - 5, cy + cellH);

			// 节点名
			settextcolor(RGB(40, 40, 40));
			settextstyle(16, 0, L"微软雅黑");
			int tw = textwidth(graphNodes[i].name.c_str());
			outtextxy(cx + 10, cy + (cellH - textheight(graphNodes[i].name.c_str())) / 2, graphNodes[i].name.c_str());

			// 距离值
			std::wstring distStr;
			bool known = false;
			if (currentStep >= 0) {
				// 根据步骤判断该节点是否已确定
				for (int s = 0; s <= currentStep; s++) {
					if (steps[s].selectedNode == (int)i) {
						known = true;
						break;
					}
				}
			}

			if (known) {
				distStr = std::to_wstring(graphNodes[i].dist);
				settextcolor(RGB(50, 150, 50));
			}
			else if (currentStep < 0) {
				distStr = (i == 0) ? L"0" : L"∞";
				settextcolor(i == 0 ? RGB(50, 150, 50) : RGB(150, 150, 150));
			}
			else {
				// 检查是否被更新过
				bool updated = false;
				int latestDist = INT_MAX;
				for (int s = 0; s <= currentStep; s++) {
					for (int v : steps[s].updatedNodes) {
						if (v == (int)i) {
							updated = true;
						}
					}
				}
				// 实际计算到当前步骤的距离
				latestDist = calcDistAtStep((int)i, currentStep);
				if (latestDist == INT_MAX) {
					distStr = L"∞";
					settextcolor(RGB(150, 150, 150));
				}
				else {
					distStr = std::to_wstring(latestDist);
					settextcolor(updated ? RGB(200, 150, 0) : RGB(80, 80, 80));
				}
			}

			int tw2 = textwidth(distStr.c_str());
			outtextxy(cx + cellW - 15 - tw2, cy + (cellH - textheight(distStr.c_str())) / 2, distStr.c_str());
		}
	}

	int calcDistAtStep(int nodeIdx, int stepIdx) const
	{
		if (stepIdx < 0) return (nodeIdx == 0) ? 0 : INT_MAX;

		std::vector<int> dist(graphNodes.size(), INT_MAX);
		dist[0] = 0;

		for (int s = 1; s <= stepIdx; s++) {
			int u = steps[s].selectedNode;
			if (u < 0) continue;

			for (auto& e : graphEdges) {
				int v = -1;
				if (e.from == u) v = e.to;
				else if (e.to == u) v = e.from;

				if (v != -1 && dist[u] != INT_MAX) {
					int newDist = dist[u] + e.weight;
					if (newDist < dist[v]) {
						dist[v] = newDist;
					}
				}
			}
		}

		return dist[nodeIdx];
	}

	void drawTitle() const
	{
		std::wstring title = L"Dijkstra 最短路径演示";
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

	void drawStepInfo() const
	{
		std::wstring info;
		if (currentStep < 0) {
			info = L"点击“下一步”开始 Dijkstra 最短路径演示。起点：A";
		}
		else if (currentStep >= (int)steps.size() - 1) {
			info = L"算法完成！A 到 F 的最短距离为 13（路径：A→C→B→D→E→F）";
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
		int ty = 580;
		outtextxy(tx, ty, info.c_str());
	}
};
