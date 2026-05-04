#pragma once

#include <graphics.h>
#include <string>

class Circle
{
public:
    Circle() = default;
    ~Circle() = default;

    Circle(int x, int y, int radius,
           COLORREF fillColor = WHITE, COLORREF borderColor = BLACK)
        : x(x), y(y), radius(radius),
          fillColor(fillColor), borderColor(borderColor) {}

    // 绘制圆
    void draw() const
    {
        setfillcolor(fillColor);
        setlinecolor(borderColor);
        setlinestyle(PS_SOLID, borderWidth);
        fillcircle(x, y, radius);
    }

    // 绘制带文本的圆（居中显示）
    void drawWithText(const std::wstring& text, COLORREF textColor = BLACK, int fontSize = 20) const
    {
        draw();
        settextcolor(textColor);
        settextstyle(fontSize, 0, L"微软雅黑");
        setbkmode(TRANSPARENT);
        int tw = textwidth(text.c_str());
        int th = textheight(text.c_str());
        int tx = x - tw / 2;
        int ty = y - th / 2;
        outtextxy(tx, ty, text.c_str());
    }

    // 绘制带数值的圆（自动转字符串）
    void drawWithValue(int value, COLORREF textColor = BLACK, int fontSize = 20) const
    {
        drawWithText(std::to_wstring(value), textColor, fontSize);
    }

    // 动画：移动到目标位置（返回是否到达）
    bool moveTo(int targetX, int targetY, float speed)
    {
        bool arrived = true;
        float dx = (float)(targetX - x);
        float dy = (float)(targetY - y);
        float dist = sqrt(dx * dx + dy * dy);
        if (dist > speed)
        {
            x += (int)(dx / dist * speed);
            y += (int)(dy / dist * speed);
            arrived = false;
        }
        else
        {
            x = targetX;
            y = targetY;
        }
        return arrived;
    }

    // 动画：线性插值颜色
    void lerpColor(COLORREF targetColor, float t)
    {
        auto lerp = [](BYTE a, BYTE b, float t) -> BYTE {
            return (BYTE)(a + (b - a) * t);
        };
        fillColor = RGB(
            lerp(GetRValue(fillColor), GetRValue(targetColor), t),
            lerp(GetGValue(fillColor), GetGValue(targetColor), t),
            lerp(GetBValue(fillColor), GetBValue(targetColor), t)
        );
    }

    // 设置高亮状态
    void setHighlight(bool highlight, COLORREF highlightColor = YELLOW)
    {
        isHighlighted = highlight;
        if (highlight)
        {
            originalColor = fillColor;
            fillColor = highlightColor;
        }
        else
        {
            fillColor = originalColor;
        }
    }

    // 设置选中状态
    void setSelected(bool selected, COLORREF selectedColor = GREEN)
    {
        isSelected = selected;
        if (selected)
        {
            originalColor = fillColor;
            fillColor = selectedColor;
        }
        else
        {
            fillColor = originalColor;
        }
    }

    // 检查点是否在圆内
    bool contains(int px, int py) const
    {
        int dx = px - x;
        int dy = py - y;
        return dx * dx + dy * dy <= radius * radius;
    }

    // 检查鼠标是否悬停（需传入鼠标位置）
    bool isHover(int mouseX, int mouseY) const
    {
        return contains(mouseX, mouseY);
    }

    // Getters
    int getX() const { return x; }
    int getY() const { return y; }
    int getRadius() const { return radius; }
    int getDiameter() const { return radius * 2; }
    COLORREF getFillColor() const { return fillColor; }
    COLORREF getBorderColor() const { return borderColor; }
    bool getHighlighted() const { return isHighlighted; }
    bool getSelected() const { return isSelected; }

    // Setters
    void setX(int val) { x = val; }
    void setY(int val) { y = val; }
    void setPos(int px, int py) { x = px; y = py; }
    void setRadius(int val) { radius = val; }
    void setFillColor(COLORREF color) { fillColor = color; }
    void setBorderColor(COLORREF color) { borderColor = color; }
    void setBorderWidth(int val) { borderWidth = val; }

private:
    int x = 0;
    int y = 0;
    int radius = 30;
    int borderWidth = 2;

    COLORREF fillColor = WHITE;
    COLORREF borderColor = BLACK;
    COLORREF originalColor = WHITE;

    bool isHighlighted = false;
    bool isSelected = false;
};
