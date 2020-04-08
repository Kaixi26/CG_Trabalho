#ifndef POINT__H
#define POINT__H

class Point {
    public:
        float x;
        float y;
        float z;
    public:
        Point(){}
        Point(float x, float y, float z);

        static Point midPoint(Point p1, Point p2);

        static Point* calcBenzier(Point* p);


        void print();
};

#endif