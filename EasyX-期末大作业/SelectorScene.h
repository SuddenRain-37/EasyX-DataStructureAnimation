#pragma once

#include <graphics.h>
#include <vector>
#include <string>

#include "Scene.h"
#include "SwitchButton.h"
#include "Button.h"
#include "SceneManager.h"

extern SceneManager scene_manager;

class SelectorScene : public Scene
{
public:
	SelectorScene() = default;
	~SelectorScene() = default;
public:
	void on_enter() {
		buttons.clear();
		buttonBaseY.clear();
		scrollOffsetY = 0;
		initButtons();
	}
	void on_update(int dalta) {

	}
	void on_draw() {
		setbkcolor(WHITE);
		cleardevice();

		// 绘制标题
		drawTitle();

		// 绘制返回按钮
		btnBack.draw();

		// 设置按钮列表的裁剪区域（防止绘制到标题和返回按钮区域）
		HRGN rgn = CreateRectRgn(marginX, contentTop, 1280 - marginX, contentBottom);
		setcliprgn(rgn);
		DeleteObject(rgn);

		// 绘制所有按钮（应用滚动偏移）
		for (size_t i = 0; i < buttons.size(); i++) {
			int drawY = buttonBaseY[i] - (int)scrollOffsetY;
			// 跳过完全不在可视区域内的按钮
			if (drawY + btnH < contentTop || drawY > contentBottom) continue;

			buttons[i].setY(drawY);
			buttons[i].draw();
			buttons[i].setY(buttonBaseY[i]); // 恢复原始坐标
		}

		// 清除裁剪区域
		setcliprgn(NULL);

		// 绘制滚动条
		drawScrollbar();
	}
	void on_input(const ExMessage& msg) {
		// 处理返回按钮（不受滚动影响）
		btnBack.on_input(msg);

		// 处理鼠标滚轮
		if (msg.message == WM_MOUSEWHEEL) {
			scrollOffsetY -= msg.wheel / 6.0f; // 调整滚动速度
			clampScroll();
			return;
		}

		// 处理按钮点击（应用滚动偏移后的坐标）
		for (size_t i = 0; i < buttons.size(); i++) {
			int drawY = buttonBaseY[i] - (int)scrollOffsetY;
			// 跳过完全不在可视区域内的按钮
			if (drawY + btnH < contentTop || drawY > contentBottom) continue;

			buttons[i].setY(drawY);
			buttons[i].on_input(msg);
			buttons[i].setY(buttonBaseY[i]); // 恢复原始坐标
		}
	}

	void on_exit() {
		// 不在此处清空 buttons，避免 on_input 处理过程中被 switch_to 触发清空导致越界
	}

private:
	Button btnBack;
	std::vector<SwitchButton> buttons;
	std::vector<int> buttonBaseY;

	// 布局参数
	const int marginX = 120;
	const int marginTop = 140;
	const int marginBottom = 40;
	const int btnH = 55;
	const int gap = 20;
	const int contentTop = marginTop;
	const int contentBottom = 720 - marginBottom;

	float scrollOffsetY = 0;
	int totalContentHeight = 0;
	int visibleHeight = contentBottom - contentTop;

	void initButtons()
	{
		const int screenW = 1280;

		// 右上角返回按钮
		btnBack = Button(screenW - 110, 20, 90, 40, L"返回");
		btnBack.setFontSize(18);
		btnBack.setCornerRadius(8);
		btnBack.setColors(RGB(200, 80, 80), RGB(230, 110, 110), RGB(170, 60, 60));
		btnBack.setTextColor(WHITE);
		btnBack.setOnClick([]() {
			scene_manager.switch_to(SceneManager::SceneType::Menu);
		});

		int btnW = screenW - marginX * 2;

		struct Item {
			std::wstring label;
			std::wstring name;
			SceneManager::SceneType target;
		};

		Item items[] = {
			{ L"动画一", L"冒泡排序", SceneManager::SceneType::Pj1 },
			{ L"动画二", L"二叉树的中序遍历", SceneManager::SceneType::Pj2 },
			{ L"动画三", L"二叉树的先序遍历", SceneManager::SceneType::Pj3 },
			{ L"动画四", L"二叉树的后序遍历", SceneManager::SceneType::Pj4 },
			{ L"动画五", L"哈夫曼树", SceneManager::SceneType::Pj5 },
			{ L"动画六", L"Dijkstra 最短路径", SceneManager::SceneType::Pj6 },
			{ L"动画七", L"Floyd 最短路径", SceneManager::SceneType::Pj7 },
			{ L"动画八", L"堆排序", SceneManager::SceneType::Pj8 },
			{ L"动画九", L"二分查找", SceneManager::SceneType::Pj9 },
			{ L"动画十", L"归并排序", SceneManager::SceneType::Pj10 },
		};

		int itemCount = sizeof(items) / sizeof(items[0]);
		totalContentHeight = itemCount * btnH + (itemCount - 1) * gap;

		for (int i = 0; i < itemCount; i++)
		{
			int baseY = marginTop + i * (btnH + gap);
			buttonBaseY.push_back(baseY);

			buttons.emplace_back();
			SwitchButton& btn = buttons.back();
			btn.init(marginX, baseY, btnW, btnH, items[i].target);
			btn.setLabel(items[i].label);
			btn.setSceneName(items[i].name);
			btn.setFontSize(20);
			btn.setCornerRadius(8);
			btn.setColors(WHITE, RGB(230, 240, 250), RGB(200, 220, 240));
			btn.setBorderColors(RGB(180, 180, 180), RGB(70, 130, 180), RGB(70, 130, 180));
			btn.setTextColor(RGB(50, 50, 50));
		}
	}

	void clampScroll()
	{
		if (scrollOffsetY < 0) scrollOffsetY = 0;
		int maxScroll = totalContentHeight - visibleHeight;
		if (maxScroll < 0) maxScroll = 0;
		if (scrollOffsetY > maxScroll) scrollOffsetY = (float)maxScroll;
	}

	void drawScrollbar() const
	{
		int maxScroll = totalContentHeight - visibleHeight;
		if (maxScroll <= 0) return; // 内容无需滚动，不显示滚动条

		int trackX = 1280 - marginX / 2;
		int trackY = contentTop;
		int trackH = visibleHeight;
		int trackW = 6;

		// 滚动条轨道
		setfillcolor(RGB(220, 220, 220));
		setlinecolor(RGB(220, 220, 220));
		solidroundrect(trackX - trackW / 2, trackY, trackX + trackW / 2, trackY + trackH, trackW, trackW);

		// 滑块
		float ratio = (float)scrollOffsetY / maxScroll;
		int thumbH = (int)((float)visibleHeight * visibleHeight / totalContentHeight);
		if (thumbH < 30) thumbH = 30; // 最小滑块高度
		int thumbY = trackY + (int)(ratio * (trackH - thumbH));

		setfillcolor(RGB(150, 150, 150));
		setlinecolor(RGB(150, 150, 150));
		solidroundrect(trackX - trackW / 2, thumbY, trackX + trackW / 2, thumbY + thumbH, trackW, trackW);
	}

	void drawTitle() const
	{
		std::wstring title = L"请选择要演示的动画";
		settextstyle(36, 0, L"微软雅黑");
		settextcolor(RGB(40, 40, 40));
		setbkmode(TRANSPARENT);

		int tw = textwidth(title.c_str());
		int tx = (1280 - tw) / 2;
		int ty = 60;
		outtextxy(tx, ty, title.c_str());

		// 装饰线
		setlinecolor(RGB(70, 130, 180));
		setlinestyle(PS_SOLID, 3);
		int lineW = tw + 30;
		int lineX = (1280 - lineW) / 2;
		line(lineX, ty + 50, lineX + lineW, ty + 50);
	}
};
