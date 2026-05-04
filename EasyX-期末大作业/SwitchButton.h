#pragma once

#include <graphics.h>
#include <string>

#include "Button.h"
#include "SceneManager.h"

extern SceneManager scene_manager;

class SwitchButton : public Button
{
public:
    SwitchButton() = default;
    ~SwitchButton() = default;

    // 五个参数：x, y, width, height, 跳转到的场景类型
    void init(int x, int y, int width, int height, SceneManager::SceneType targetScene)
    {
        setX(x);
        setY(y);
        setWidth(width);
        setHeight(height);
        target = targetScene;
    }

    // 设置左侧标签文字（如 "动画一"）
    void setLabel(const std::wstring& labelText)
    {
        label = labelText;
    }

    // 设置右侧场景名称（如 "树的遍历"）
    void setSceneName(const std::wstring& name)
    {
        sceneName = name;
    }

    // 处理鼠标消息，返回是否点击了该按钮
    bool on_input(const ExMessage& msg)
    {
        if (!getEnabled()) return false;

        bool inside = contains(msg.x, msg.y);

        switch (msg.message)
        {
        case WM_MOUSEMOVE:
            if (inside && !getHovered())
            {
                setHovered(true);
            }
            else if (!inside && getHovered())
            {
                setHovered(false);
                setPressed(false);
            }
            break;

        case WM_LBUTTONDOWN:
            if (inside)
            {
                setPressed(true);
                return true;
            }
            break;

        case WM_LBUTTONUP:
            if (getPressed())
            {
                setPressed(false);
                if (inside)
                {
                    scene_manager.switch_to(target);
                    return true;
                }
            }
            break;
        }

        return false;
    }

    // 重写绘制：左侧标签 + 右侧场景名
    void draw() const
    {
        // 根据状态确定颜色
        COLORREF currentFill = Button::getFillColor();
        COLORREF currentBorder = Button::getBorderColor();
        COLORREF currentText = Button::getTextColor();

        if (!getEnabled())
        {
            currentFill = Button::getDisabledColor();
            currentText = Button::getDisabledTextColor();
        }
        else if (getPressed())
        {
            currentFill = Button::getPressedColor();
            currentBorder = Button::getPressedBorderColor();
        }
        else if (getHovered())
        {
            currentFill = Button::getHoverColor();
            currentBorder = Button::getHoverBorderColor();
        }

        // 绘制背景
        setfillcolor(currentFill);
        setlinecolor(currentBorder);
        setlinestyle(PS_SOLID, Button::getBorderWidth());

        int radius = Button::getCornerRadius();
        if (radius > 0)
        {
            fillroundrect(getX(), getY(), getX() + getWidth(), getY() + getHeight(), radius * 2, radius * 2);
        }
        else
        {
            fillrectangle(getX(), getY(), getX() + getWidth(), getY() + getHeight());
        }

        // 绘制文字（透明背景）
        settextcolor(currentText);
        settextstyle(Button::getFontSize(), 0, Button::getFontName().c_str());
        setbkmode(TRANSPARENT);

        int padding = 20;

        // 左侧标签
        int labelY = getY() + (getHeight() - textheight(label.c_str())) / 2;
        outtextxy(getX() + padding, labelY, label.c_str());

        // 右侧场景名称
        int nameW = textwidth(sceneName.c_str());
        int nameX = getX() + getWidth() - nameW - padding;
        int nameY = getY() + (getHeight() - textheight(sceneName.c_str())) / 2;
        outtextxy(nameX, nameY, sceneName.c_str());
    }

    // 获取跳转目标（外部也可手动调用）
    SceneManager::SceneType getTarget() const { return target; }

private:
    SceneManager::SceneType target;
    std::wstring label;
    std::wstring sceneName;
};
