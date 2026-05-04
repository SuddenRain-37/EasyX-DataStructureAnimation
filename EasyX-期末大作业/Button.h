#pragma once

#include <graphics.h>
#include <string>
#include <functional>

class Button
{
public:
    using Callback = std::function<void()>;

    Button() = default;
    ~Button() = default;

    Button(int x, int y, int width, int height, const std::wstring& text)
        : x(x), y(y), width(width), height(height), text(text) {}

    // 处理鼠标消息，返回是否消耗了该消息
    bool on_input(const ExMessage& msg)
    {
        if (!enabled) return false;

        bool inside = contains(msg.x, msg.y);

        switch (msg.message)
        {
        case WM_MOUSEMOVE:
            if (inside && !isHovered)
            {
                isHovered = true;
            }
            else if (!inside && isHovered)
            {
                isHovered = false;
                isPressed = false;
            }
            break;

        case WM_LBUTTONDOWN:
            if (inside)
            {
                isPressed = true;
                return true;
            }
            break;

        case WM_LBUTTONUP:
            if (isPressed)
            {
                isPressed = false;
                if (inside && onClick)
                {
                    onClick();
                    return true;
                }
            }
            break;
        }

        return false;
    }

    // 绘制按钮
    void draw() const
    {
        // 根据状态确定颜色
        COLORREF currentFill = fillColor;
        COLORREF currentBorder = borderColor;
        COLORREF currentText = textColor;

        if (!enabled)
        {
            currentFill = disabledColor;
            currentText = disabledTextColor;
        }
        else if (isPressed)
        {
            currentFill = pressedColor;
            currentBorder = pressedBorderColor;
        }
        else if (isHovered)
        {
            currentFill = hoverColor;
            currentBorder = hoverBorderColor;
        }

        // 绘制背景
        setfillcolor(currentFill);
        setlinecolor(currentBorder);
        setlinestyle(PS_SOLID, borderWidth);

        if (cornerRadius > 0)
        {
            fillroundrect(x, y, x + width, y + height, cornerRadius * 2, cornerRadius * 2);
        }
        else
        {
            fillrectangle(x, y, x + width, y + height);
        }

        // 绘制文字（透明背景模式，去掉文字黑框）
        settextcolor(currentText);
        settextstyle(fontSize, 0, fontName.c_str());
        setbkmode(TRANSPARENT);
        int tw = textwidth(text.c_str());
        int th = textheight(text.c_str());
        int tx = x + (width - tw) / 2;
        int ty = y + (height - th) / 2;
        outtextxy(tx, ty, text.c_str());
    }

    // 检查点是否在按钮范围内
    bool contains(int px, int py) const
    {
        return px >= x && px <= x + width && py >= y && py <= y + height;
    }

    // 设置点击回调
    void setOnClick(const Callback& callback)
    {
        onClick = callback;
    }

    // Getters
    int getX() const { return x; }
    int getY() const { return y; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    bool getHovered() const { return isHovered; }
    bool getPressed() const { return isPressed; }
    bool getEnabled() const { return enabled; }
    std::wstring getText() const { return text; }

public:
    COLORREF getFillColor() const { return fillColor; }
    COLORREF getHoverColor() const { return hoverColor; }
    COLORREF getPressedColor() const { return pressedColor; }
    COLORREF getDisabledColor() const { return disabledColor; }
    COLORREF getBorderColor() const { return borderColor; }
    COLORREF getHoverBorderColor() const { return hoverBorderColor; }
    COLORREF getPressedBorderColor() const { return pressedBorderColor; }
    COLORREF getTextColor() const { return textColor; }
    COLORREF getDisabledTextColor() const { return disabledTextColor; }
    int getBorderWidth() const { return borderWidth; }
    int getCornerRadius() const { return cornerRadius; }
    int getFontSize() const { return fontSize; }
    std::wstring getFontName() const { return fontName; }

    // Setters
    void setX(int val) { x = val; }
    void setY(int val) { y = val; }
    void setPos(int px, int py) { x = px; y = py; }
    void setWidth(int val) { width = val; }
    void setHeight(int val) { height = val; }
    void setSize(int w, int h) { width = w; height = h; }
    void setText(const std::wstring& val) { text = val; }
    void setFontSize(int size) { fontSize = size; }
    void setFontName(const std::wstring& name) { fontName = name; }
    void setCornerRadius(int radius) { cornerRadius = radius; }
    void setEnabled(bool val) { enabled = val; }
    void setHovered(bool val) { isHovered = val; }
    void setPressed(bool val) { isPressed = val; }

    void setColors(COLORREF normal, COLORREF hover, COLORREF pressed)
    {
        fillColor = normal;
        hoverColor = hover;
        pressedColor = pressed;
    }

    void setBorderColors(COLORREF normal, COLORREF hover, COLORREF pressed)
    {
        borderColor = normal;
        hoverBorderColor = hover;
        pressedBorderColor = pressed;
    }

    void setTextColor(COLORREF normal, COLORREF disabled = LIGHTGRAY)
    {
        textColor = normal;
        disabledTextColor = disabled;
    }

private:
    int x = 0;
    int y = 0;
    int width = 120;
    int height = 40;
    int borderWidth = 2;
    int cornerRadius = 6;
    int fontSize = 18;

    std::wstring text = L"Button";
    std::wstring fontName = L"微软雅黑";

    COLORREF fillColor = WHITE;
    COLORREF hoverColor = RGB(230, 230, 230);
    COLORREF pressedColor = RGB(200, 200, 200);
    COLORREF disabledColor = RGB(240, 240, 240);

    COLORREF borderColor = BLACK;
    COLORREF hoverBorderColor = BLACK;
    COLORREF pressedBorderColor = BLACK;

    COLORREF textColor = BLACK;
    COLORREF disabledTextColor = LIGHTGRAY;

    bool isHovered = false;
    bool isPressed = false;
    bool enabled = true;

    Callback onClick = nullptr;
};
