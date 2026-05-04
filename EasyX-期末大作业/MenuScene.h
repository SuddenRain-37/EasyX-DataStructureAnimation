#pragma once

#include <graphics.h>
#include <string>

#include "Scene.h"
#include "Button.h"
#include "SceneManager.h"
#include "util.h"

extern SceneManager scene_manager;

class MenuScene : public Scene
{
public:
	MenuScene() = default;
	~MenuScene() = default;
public:
	void on_enter() {
		initButtons();
	}
	void on_update(int dalta) {

	}
	void on_draw() {
		setbkcolor(WHITE);
		cleardevice();
		drawTitle();
		btnStart.draw();
		btnExit.draw();
	}
	void on_input(const ExMessage& msg) {
		btnStart.on_input(msg);
		btnExit.on_input(msg);
	}
	void on_exit() {

	}
private:
	Button btnStart;
	Button btnExit;

	void initButtons()
	{
		const int screenW = 1280;
		const int screenH = 720;
		const int btnW = 180;
		const int btnH = 55;
		const int gap = 60;

		int startX = screenW / 2 - btnW - gap / 2;
		int exitX = screenW / 2 + gap / 2;
		int btnY = screenH / 2 + 80;

		LPCTSTR startText = _T("进入动画");
		LPCTSTR exitText = _T("退出程序");

		btnStart = Button(startX, btnY, btnW, btnH, startText);
		btnStart.setFontSize(22);
		btnStart.setCornerRadius(10);
		btnStart.setColors(RGB(70, 130, 180), RGB(100, 160, 210), RGB(50, 100, 150));
		btnStart.setTextColor(WHITE);
		btnStart.setOnClick([]() {
			scene_manager.switch_to(SceneManager::SceneType::Selector);
		});

		btnExit = Button(exitX, btnY, btnW, btnH, exitText);
		btnExit.setFontSize(22);
		btnExit.setCornerRadius(10);
		btnExit.setColors(RGB(200, 80, 80), RGB(230, 110, 110), RGB(170, 60, 60));
		btnExit.setTextColor(WHITE);
		btnExit.setOnClick([]() {
			closegraph();
			exit(0);
		});
	}

	void drawTitle() const
	{
		const int screenW = 1280;
		const int screenH = 720;

		LPCTSTR titleText = _T("数据结构动画一览");
		std::wstring title = titleText;

		// 大字号艺术字效果：先绘制阴影再绘制主文字
		settextstyle(64, 0, L"微软雅黑");
		int tw = textwidth(title.c_str());
		int th = textheight(title.c_str());
		int tx = (screenW - tw) / 2;
		int ty = screenH / 2 - 100;

		// 阴影
		settextcolor(RGB(100, 100, 100));
		setbkmode(TRANSPARENT);
		outtextxy(tx + 2, ty + 2, title.c_str());

		// 主文字
		settextcolor(RGB(30, 30, 30));
		setbkmode(TRANSPARENT);
		outtextxy(tx, ty, title.c_str());

		// 装饰下划线
		setlinecolor(RGB(70, 130, 180));
		setlinestyle(PS_SOLID, 4);
		int lineW = tw + 40;
		int lineX = (screenW - lineW) / 2;
		int lineY = ty + th + 20;
		line(lineX, lineY, lineX + lineW, lineY);
	}
};
