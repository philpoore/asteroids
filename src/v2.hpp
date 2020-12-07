#pragma once

struct v2 {
    float x;
    float y;

    void clamp(int width, int height) {
        if (x > width)
            x -= width;
        else if (x < 0)
            x += width;

        if (y > height)
            y -= height;
        else if (y < 0)
            y += height;
    }
};



bool lines_intersect(v2 p0, v2 p1, v2 p2, v2 p3, v2* i)
{
    v2 s1 = {p1.x - p0.x, p1.y - p0.y};
    v2 s2 = {p3.x - p2.x, p3.y - p2.y};

    float s, t;
    s = (-s1.y * (p0.x - p2.x) + s1.x * (p0.y - p2.y)) / (-s2.x * s1.y + s1.x * s2.y);
    t = ( s2.x * (p0.y - p2.y) - s2.y * (p0.x - p2.x)) / (-s2.x * s1.y + s1.x * s2.y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        // Collision detected
        if (i != nullptr) {
            i->x = p0.x + (t * s1.x);
            i->y = p0.y + (t * s1.y);
        }
        return true;
    }

    return false; // No collision
}