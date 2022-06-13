#include <stdio.h>

struct point {
    int x;
    int y;
};

struct point makePoint(int x1, int y1) {
    struct point temp;
    temp.x = x1;
    temp.y = y1;
    return temp;
}

void printPoint(struct point p){
    printf("(%d,%d)\n", p.x, p.y);
}

void modifyPoint(struct point* p){
    p->x = 4;
    p->y = 5;
}

int main() {
    struct point* p[2];
    struct point p0 = {3,4};
    struct point p1 = {9,10};
    p[0] = &p0;
    *(p[0]) = p1;
    p[0]->x++;
    p[0]->y++;
    printPoint(p0);
}

void isr_function() {}