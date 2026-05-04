#pragma once

#include "Scene.h"

extern Scene* menu_scene;
extern Scene* selector_scene;

extern Scene* pj1_scene;
extern Scene* pj2_scene;
extern Scene* pj3_scene;
extern Scene* pj4_scene;
extern Scene* pj5_scene;
extern Scene* pj6_scene;
extern Scene* pj7_scene;
extern Scene* pj8_scene;
extern Scene* pj9_scene;
extern Scene* pj10_scene;


class SceneManager {
public:
	enum class SceneType {
		Menu,
		Selector,
		Pj1,
		Pj2,
		Pj3,
		Pj4,
		Pj5,
		Pj6,
		Pj7,
		Pj8,
		Pj9,
		Pj10
	};
public:
	SceneManager() = default;
	~SceneManager() = default;

	void set_current_scene(Scene* scene) {
		current_scene = scene;
		current_scene->on_enter();
	}

	void switch_to(SceneType type) {
		current_scene->on_exit();
		switch (type)
		{
		case SceneType::Menu:
			current_scene = menu_scene;
			break;
		case SceneType::Selector:
			current_scene = selector_scene;
			break;
		case SceneType::Pj1:
			current_scene = pj1_scene;
			break;
		case SceneType::Pj2:
			current_scene = pj2_scene;
			break;
		case SceneType::Pj3:
			current_scene = pj3_scene;
			break;
		case SceneType::Pj4:
			current_scene = pj4_scene;
			break;
		case SceneType::Pj5:
			current_scene = pj5_scene;
			break;
		case SceneType::Pj6:
			current_scene = pj6_scene;
			break;
		case SceneType::Pj7:
			current_scene = pj7_scene;
			break;
		case SceneType::Pj8:
			current_scene = pj8_scene;
			break;
		case SceneType::Pj9:
			current_scene = pj9_scene;
			break;
		case SceneType::Pj10:
			current_scene = pj10_scene;
			break;
		default:
			break;
		}
		current_scene->on_enter();
	}

	void on_draw() {
		current_scene->on_draw();
	}

	void on_input(const ExMessage msg) {
		current_scene->on_input(msg);
	}

	void on_update(int dalta) {
		current_scene->on_update(dalta);
	}

private:
	Scene* current_scene = nullptr;
};