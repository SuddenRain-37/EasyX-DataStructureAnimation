#pragma once

#include <graphics.h>
#include <string>

class RectShape
{
public:
    RectShape() = default;
    ~RectShape() = default;

    RectShape(int x, int y, int width, int height,
              COLORREF fillColor = WHITE, COLORREF borderColor = BLACK)
        : x(x), y(y), width(width), height(height),
          fillColor(fillColor), borderColor(borderColor) {}

    // 绘制矩形
    void draw() const
    {
        setfillcolor(fillColor);
        setlinecolor(borderColor);
        setlinestyle(PS_SOLID, borderWidth);
        fillrectangle(x, y, x + width, y + height);
    }

    // 绘制圆角矩形
    void drawRound(int ellipseWidth, int ellipseHeight) const
    {
        setfillcolor(fillColor);
        setlinecolor(borderColor);
        setlinestyle(PS_SOLID, borderWidth);
        fillroundrect(x, y, x + width, y + height, ellipseWidth, ellipseHeight);
    }

    // 绘制带文本的矩形（居中显示）
    void drawWithText(const std::wstring& text, COLORREF textColor = BLACK, int fontSize = 20) const
    {
        draw();
        settextcolor(textColor);
        settextstyle(fontSize, 0, L"微软雅黑");
        int tw = textwidth(text.c_str());
        int th = textheight(text.c_str());
        int tx = x + (width - tw) / 2;
        int ty = y + (height - th) / 2;
        outtextxy(tx, ty, text.c_str());
    }

    // 绘制带文本的圆角矩形
    void drawRoundWithText(const std::wstring& text, int ew, int eh,
                           COLORREF textColor = BLACK, int fontSize = 20) const
    {
        drawRound(ew, eh);
        settextcolor(textColor);
        settextstyle(fontSize, 0, L"微软雅黑");
        int tw = textwidth(text.c_str());
        int th = textheight(text.c_str());
        int tx = x + (width - tw) / 2;
        int ty = y + (height - th) / 2;
        outtextxy(tx, ty, text.c_str());
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

    // 动画：线性插值颜色（用于高亮渐变）
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

    // 检查点是否在矩形内
    bool contains(int px, int py) const
    {
        return px >= x && px <= x + width && py >= y && py <= y + height;
    }

    // 检查鼠标是否悬停（需要在消息循环中传入鼠标位置，或外部调用 contains 判断）
    bool isHover(int mouseX, int mouseY) const
    {
        return contains(mouseX, mouseY);
    }

    // Getters
    int getX() const { return x; }
    int getY() const { return y; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getCenterX() const { return x + width / 2; }
    int getCenterY() const { return y + height / 2; }
    COLORREF getFillColor() const { return fillColor; }
    COLORREF getBorderColor() const { return borderColor; }
    bool getHighlighted() const { return isHighlighted; }
    bool getSelected() const { return isSelected; }

    // Setters
    void setX(int val) { x = val; }
    void setY(int val) { y = val; }
    void setPos(int px, int py) { x = px; y = py; }
    void setWidth(int val) { width = val; }
    void setHeight(int val) { height = val; }
    void setSize(int w, int h) { width = w; height = h; }
    void setFillColor(COLORREF color) { fillColor = color; }
    void setBorderColor(COLORREF color) { borderColor = color; }
    void setBorderWidth(int val) { borderWidth = val; }

private:
    int x = 0;
    int y = 0;
    int width = 100;
    int height = 50;
    int borderWidth = 2;

    COLORREF fillColor = WHITE;
    COLORREF borderColor = BLACK;
    COLORREF originalColor = WHITE;

    bool isHighlighted = false;
    bool isSelected = false;
};
