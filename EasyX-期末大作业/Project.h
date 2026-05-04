#pragma once

#include <graphics.h>
#include <string>

#include "Scene.h"
#include "Button.h"
#include "Mini_win.h"
#include "SceneManager.h"

extern SceneManager scene_manager;

class Project : public Scene
{
public:
	Project() = default;
	~Project() = default;

public:
	void on_enter() override {
		initButtons();
		initInfoWindow();
	}

	void on_update(int dalta) override {
		updateAnimation(dalta);

		// 自动播放逻辑
		if (isPlaying) {
			playTimer += dalta;
			if (playTimer >= playInterval) {
				playTimer = 0;
				if (hasNextStep()) {
					onNextStep();
				}
				else {
					isPlaying = false;
				}
			}
		}
	}

	void on_draw() override {
		setbkcolor(WHITE);
		cleardevice();

		// 绘制动画内容（子类重写）
		drawContent();

		// 绘制六个按钮
		btnBack.draw();
		btnPrev.draw();
		btnNext.draw();
		btnPlay.draw();
		btnPause.draw();
		btnInfo.draw();

		// 绘制详情弹窗（最上层）
		infoWin.draw();
	}

	void on_input(const ExMessage& msg) override {
		// 弹窗显示时优先拦截输入
		if (infoWin.isVisible()) {
			infoWin.on_input(msg);
			return;
		}

		btnBack.on_input(msg);
		btnPrev.on_input(msg);
		btnNext.on_input(msg);
		btnPlay.on_input(msg);
		btnPause.on_input(msg);
		btnInfo.on_input(msg);
	}

	void on_exit() override {

	}

protected:
	// 子类重写：绘制动画内容
	virtual void drawContent() {}

	// 子类重写：更新动画状态
	virtual void updateAnimation(int dalta) {}

	// 子类重写：上一步逻辑
	virtual void onPrevStep() {}

	// 子类重写：下一步逻辑
	virtual void onNextStep() {}

	// 子类重写：是否还有下一步（用于自动播放判断）
	virtual bool hasNextStep() const { return true; }

	// 子类重写：返回算法介绍标题
	virtual std::wstring getAlgorithmTitle() const { return L"算法介绍"; }

	// 子类重写：返回算法介绍内容
	virtual std::wstring getAlgorithmInfo() const { return L"暂无介绍"; }

private:
	Button btnBack;
	Button btnPrev;
	Button btnNext;
	Button btnPlay;
	Button btnPause;
	Button btnInfo;
	MiniWin infoWin;

	bool isPlaying = false;
	DWORD playTimer = 0;
	const DWORD playInterval = 1200;

	void initButtons()
	{
		const int screenW = 1280;
		const int screenH = 720;

		// 右上角返回按钮
		btnBack = Button(screenW - 110, 20, 90, 40, L"返回");
		btnBack.setFontSize(18);
		btnBack.setCornerRadius(8);
		btnBack.setColors(RGB(200, 80, 80), RGB(230, 110, 110), RGB(170, 60, 60));
		btnBack.setTextColor(WHITE);
		btnBack.setOnClick([]() {
			scene_manager.switch_to(SceneManager::SceneType::Selector);
		});

		// 左上角：播放、暂停、详情
		btnPlay = Button(20, 20, 70, 40, L"播放");
		btnPlay.setFontSize(18);
		btnPlay.setCornerRadius(8);
		btnPlay.setColors(RGB(80, 180, 80), RGB(110, 210, 110), RGB(60, 150, 60));
		btnPlay.setTextColor(WHITE);
		btnPlay.setOnClick([this]() {
			isPlaying = true;
			playTimer = 0;
		});

		btnPause = Button(100, 20, 70, 40, L"暂停");
		btnPause.setFontSize(18);
		btnPause.setCornerRadius(8);
		btnPause.setColors(RGB(230, 150, 60), RGB(255, 180, 90), RGB(200, 120, 40));
		btnPause.setTextColor(WHITE);
		btnPause.setOnClick([this]() {
			isPlaying = false;
			playTimer = 0;
		});

		btnInfo = Button(180, 20, 70, 40, L"详情");
		btnInfo.setFontSize(18);
		btnInfo.setCornerRadius(8);
		btnInfo.setColors(RGB(100, 149, 237), RGB(130, 179, 255), RGB(70, 119, 207));
		btnInfo.setTextColor(WHITE);
		btnInfo.setOnClick([this]() {
			infoWin.show();
		});

		// 下方居中：上一步、下一步
		const int btnW = 120;
		const int btnH = 45;
		const int gap = 40;
		int centerX = screenW / 2;
		int btnY = screenH - 80;

		btnPrev = Button(centerX - btnW - gap / 2, btnY, btnW, btnH, L"上一步");
		btnPrev.setFontSize(18);
		btnPrev.setCornerRadius(8);
		btnPrev.setColors(RGB(70, 130, 180), RGB(100, 160, 210), RGB(50, 100, 150));
		btnPrev.setTextColor(WHITE);
		btnPrev.setOnClick([this]() {
			onPrevStep();
		});

		btnNext = Button(centerX + gap / 2, btnY, btnW, btnH, L"下一步");
		btnNext.setFontSize(18);
		btnNext.setCornerRadius(8);
		btnNext.setColors(RGB(70, 130, 180), RGB(100, 160, 210), RGB(50, 100, 150));
		btnNext.setTextColor(WHITE);
		btnNext.setOnClick([this]() {
			onNextStep();
		});
	}

	void initInfoWindow()
	{
		infoWin.init(620, 420, getAlgorithmTitle());
		infoWin.setContent(getAlgorithmInfo());
	}
};
