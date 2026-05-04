#include <graphics.h>

#include "SceneManager.h"
#include "MenuScene.h"
#include "SelectorScene.h"
#include "Project_1.h"
#include "Project_2.h"
#include "project_3.h"
#include "project_4.h"
#include "project_5.h"
#include "project_6.h"
#include "project_7.h"
#include "project_8.h"
#include "project_9.h"
#include "project_10.h"

Scene* menu_scene = nullptr;
Scene* selector_scene = nullptr;
Scene* pj1_scene = nullptr;
Scene* pj2_scene = nullptr;
Scene* pj3_scene = nullptr;
Scene* pj4_scene = nullptr;
Scene* pj5_scene = nullptr;
Scene* pj6_scene = nullptr;
Scene* pj7_scene = nullptr;
Scene* pj8_scene = nullptr;
Scene* pj9_scene = nullptr;
Scene* pj10_scene = nullptr;
SceneManager scene_manager;

int main() {
	ExMessage msg;
	const int FPS = 144;

	initgraph(1280, 720); //EW_SHOWCONSOLE

	BeginBatchDraw();

	menu_scene = new MenuScene();
	selector_scene = new SelectorScene();
	pj1_scene = new Pj1Scene();
	pj2_scene = new Pj2Scene();
	pj3_scene = new Pj3Scene();
	pj4_scene = new Pj4Scene();
	pj5_scene = new Pj5Scene();
	pj6_scene = new Pj6Scene();
	pj7_scene = new Pj7Scene();
	pj8_scene = new Pj8Scene();
	pj9_scene = new Pj9Scene();
	pj10_scene = new Pj10Scene();

	scene_manager.set_current_scene(menu_scene);


	while (1) {

		DWORD frame_start_time = GetTickCount();

		while (peekmessage(&msg)) {
			scene_manager.on_input(msg);
		}

		static DWORD last_tick_time = GetTickCount();
		DWORD current_tick_time = GetTickCount();
		DWORD dalta_tick = current_tick_time - last_tick_time;
		scene_manager.on_update(dalta_tick);
		last_tick_time = current_tick_time;

		cleardevice();
		scene_manager.on_draw();
		FlushBatchDraw();

		DWORD frame_end_time = GetTickCount();
		DWORD frame_dalta_time = frame_end_time - frame_start_time;
		if (frame_dalta_time < 1000 / FPS) {
			Sleep(1000 / FPS - frame_dalta_time);
		}
	}
	EndBatchDraw();
	return 0;
}