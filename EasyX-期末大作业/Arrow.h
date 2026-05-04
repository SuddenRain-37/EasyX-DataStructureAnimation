#pragma once

#include <graphics.h>
#include <string>
#include <cmath>

class Arrow
{
public:
    Arrow() = default;
    ~Arrow() = default;

    Arrow(int x1, int y1, int x2, int y2, COLORREF color = BLACK)
        : x1(x1), y1(y1), x2(x2), y2(y2), color(color) {}

    // 绘制箭头（带箭头头部）
    void draw() const
    {
        setlinecolor(color);
        setlinestyle(PS_SOLID, lineWidth);

        // 绘制主线
        line(x1, y1, x2, y2);

        // 绘制箭头头部
        drawArrowHead();
    }

    // 绘制虚线箭头
    void drawDashed() const
    {
        setlinecolor(color);
        setlinestyle(PS_DASH, lineWidth);
        line(x1, y1, x2, y2);
        setlinestyle(PS_SOLID, lineWidth);
        drawArrowHead();
    }

    // 绘制双向箭头
    void drawDouble() const
    {
        setlinecolor(color);
        setlinestyle(PS_SOLID, lineWidth);
        line(x1, y1, x2, y2);
        drawArrowHead();
        drawArrowHeadAtStart();
    }

    // 绘制带标签的箭头（在箭头中点显示文字）
    void drawWithLabel(const std::wstring& text, COLORREF textColor = BLACK, int fontSize = 16) const
    {
        draw();

        int mx = (x1 + x2) / 2;
        int my = (y1 + y2) / 2;

        settextcolor(textColor);
        settextstyle(fontSize, 0, L"微软雅黑");
        setbkmode(TRANSPARENT);

        int tw = textwidth(text.c_str());
        int th = textheight(text.c_str());

        // 文字背景（白色填充，避免被线穿过）
        setfillcolor(WHITE);
        solidrectangle(mx - tw / 2 - 4, my - th / 2 - 2, mx + tw / 2 + 4, my + th / 2 + 2);

        outtextxy(mx - tw / 2, my - th / 2, text.c_str());
    }

    // 动画：移动到新的目标终点（返回是否到达）
    bool moveTo(int targetX, int targetY, float speed)
    {
        bool arrived = true;
        float dx = (float)(targetX - x2);
        float dy = (float)(targetY - y2);
        float dist = sqrt(dx * dx + dy * dy);
        if (dist > speed)
        {
            x2 += (int)(dx / dist * speed);
            y2 += (int)(dy / dist * speed);
            arrived = false;
        }
        else
        {
            x2 = targetX;
            y2 = targetY;
        }
        return arrived;
    }

    // 动画：起点和终点同时移动
    bool moveBoth(int targetX1, int targetY1, int targetX2, int targetY2, float speed)
    {
        bool arrived1 = moveStartTo(targetX1, targetY1, speed);
        bool arrived2 = moveEndTo(targetX2, targetY2, speed);
        return arrived1 && arrived2;
    }

    bool moveStartTo(int targetX, int targetY, float speed)
    {
        bool arrived = true;
        float dx = (float)(targetX - x1);
        float dy = (float)(targetY - y1);
        float dist = sqrt(dx * dx + dy * dy);
        if (dist > speed)
        {
            x1 += (int)(dx / dist * speed);
            y1 += (int)(dy / dist * speed);
            arrived = false;
        }
        else
        {
            x1 = targetX;
            y1 = targetY;
        }
        return arrived;
    }

    bool moveEndTo(int targetX, int targetY, float speed)
    {
        bool arrived = true;
        float dx = (float)(targetX - x2);
        float dy = (float)(targetY - y2);
        float dist = sqrt(dx * dx + dy * dy);
        if (dist > speed)
        {
            x2 += (int)(dx / dist * speed);
            y2 += (int)(dy / dist * speed);
            arrived = false;
        }
        else
        {
            x2 = targetX;
            y2 = targetY;
        }
        return arrived;
    }

    // 设置高亮状态
    void setHighlight(bool highlight, COLORREF highlightColor = RED)
    {
        isHighlighted = highlight;
        if (highlight)
        {
            originalColor = color;
            color = highlightColor;
        }
        else
        {
            color = originalColor;
        }
    }

    // 计算箭头长度
    double length() const
    {
        double dx = x2 - x1;
        double dy = y2 - y1;
        return sqrt(dx * dx + dy * dy);
    }

    // 获取中点坐标
    int getMidX() const { return (x1 + x2) / 2; }
    int getMidY() const { return (y1 + y2) / 2; }

    // Getters
    int getX1() const { return x1; }
    int getY1() const { return y1; }
    int getX2() const { return x2; }
    int getY2() const { return y2; }
    COLORREF getColor() const { return color; }
    int getLineWidth() const { return lineWidth; }
    int getHeadSize() const { return headSize; }

    // Setters
    void setX1(int val) { x1 = val; }
    void setY1(int val) { y1 = val; }
    void setX2(int val) { x2 = val; }
    void setY2(int val) { y2 = val; }
    void setStart(int px, int py) { x1 = px; y1 = py; }
    void setEnd(int px, int py) { x2 = px; y2 = py; }
    void setColor(COLORREF c) { color = c; }
    void setLineWidth(int w) { lineWidth = w; }
    void setHeadSize(int s) { headSize = s; }

private:
    int x1 = 0, y1 = 0;
    int x2 = 100, y2 = 100;
    int lineWidth = 2;
    int headSize = 10;

    COLORREF color = BLACK;
    COLORREF originalColor = BLACK;
    bool isHighlighted = false;

    // 绘制箭头头部（在终点）
    void drawArrowHead() const
    {
        double angle = atan2((double)(y2 - y1), (double)(x2 - x1));
        double arrowAngle = 0.5; // 箭头张角（弧度）

        int x3 = x2 - (int)(headSize * cos(angle - arrowAngle));
        int y3 = y2 - (int)(headSize * sin(angle - arrowAngle));
        int x4 = x2 - (int)(headSize * cos(angle + arrowAngle));
        int y4 = y2 - (int)(headSize * sin(angle + arrowAngle));

        line(x2, y2, x3, y3);
        line(x2, y2, x4, y4);
    }

    // 绘制起点箭头头部（双向箭头用）
    void drawArrowHeadAtStart() const
    {
        double angle = atan2((double)(y1 - y2), (double)(x1 - x2));
        double arrowAngle = 0.5;

        int x3 = x1 - (int)(headSize * cos(angle - arrowAngle));
        int y3 = y1 - (int)(headSize * sin(angle - arrowAngle));
        int x4 = x1 - (int)(headSize * cos(angle + arrowAngle));
        int y4 = y1 - (int)(headSize * sin(angle + arrowAngle));

        line(x1, y1, x3, y3);
        line(x1, y1, x4, y4);
    }
};
