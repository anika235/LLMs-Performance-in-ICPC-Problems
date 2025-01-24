#include <bits/stdc++.h>
using namespace std;

const double EPS = 1e-9;

struct Point {
    double x, y;
    Point(double _x=0, double _y=0) : x(_x), y(_y) {}
};

// Cross product of AB and AC
double cross_product(const Point& A, const Point& B, const Point& C){
    double abx = B.x - A.x;
    double aby = B.y - A.y;
    double acx = C.x - A.x;
    double acy = C.y - A.y;
    return abx * acy - aby * acx;
}

// Check if point P is to the left of line AB
bool is_left(const Point& A, const Point& B, const Point& P){
    return cross_product(A, B, P) > EPS;
}

// Compute intersection point of lines AB and CD
Point getIntersection(const Point& A, const Point& B, const Point& C, const Point& D){
    double a1 = B.x - A.x;
    double b1 = B.y - A.y;
    double a2 = D.x - C.x;
    double b2 = D.y - C.y;
    double denominator = a1 * b2 - a2 * b1;
    if(abs(denominator) < EPS){
        // Lines are parallel; return a default point
        return Point(0,0);
    }
    double t = ((C.x - A.x) * b2 - (C.y - A.y) * a2) / denominator;
    return Point(A.x + t * a1, A.y + t * b1);
}

// Sutherland-Hodgman polygon clipping with half-plane AB (left side)
vector<Point> clipPolygonWithHalfPlane(const vector<Point>& poly, const Point& A, const Point& B){
    vector<Point> clipped;
    int n = poly.size();
    for(int i=0;i<n;i++){
        Point S = poly[i];
        Point E = poly[(i+1)%n];
        bool inside_S = is_left(A, B, S);
        bool inside_E = is_left(A, B, E);
        if(inside_S && inside_E){
            clipped.push_back(E);
        }
        else if(inside_S && !inside_E){
            Point I = getIntersection(A, B, S, E);
            clipped.push_back(I);
        }
        else if(!inside_S && inside_E){
            Point I = getIntersection(A, B, S, E);
            clipped.push_back(I);
            clipped.push_back(E);
        }
        // else both outside, do nothing
    }
    return clipped;
}

// Compute area of polygon using shoelace formula
double polygonArea(const vector<Point>& poly){
    double area = 0.0;
    int n = poly.size();
    for(int i=0;i<n;i++){
        int j = (i+1)%n;
        area += (poly[i].x * poly[j].y - poly[j].x * poly[i].y);
    }
    return abs(area) / 2.0;
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    double dx, dy;
    int n;
    cin >> dx >> dy >> n;
    // Read dome positions (1-based)
    vector<Point> domes(n+1, Point());
    for(int i=1;i<=n;i++) cin >> domes[i].x >> domes[i].y;
    // Read permutation
    vector<int> p(n);
    for(int i=0;i<n;i++) cin >> p[i];
    // If n ==1, entire rectangle is valid
    if(n ==1){
        printf("%.6lf\n", dx * dy);
        return 0;
    }
    // Initialize polygon as rectangle in CCW order
    vector<Point> polygon;
    polygon.emplace_back(0.0, 0.0);
    polygon.emplace_back(dx, 0.0);
    polygon.emplace_back(dx, dy);
    polygon.emplace_back(0.0, dy);
    // Apply each half-plane constraint
    bool empty = false;
    for(int i=0;i<n-1;i++){
        int pk = p[i];
        int pk1 = p[i+1];
        Point A = domes[pk];
        Point B = domes[pk1];
        polygon = clipPolygonWithHalfPlane(polygon, A, B);
        if(polygon.empty()){
            empty = true;
            break;
        }
    }
    double area = 0.0;
    if(!empty && !polygon.empty()){
        area = polygonArea(polygon);
    }
    printf("%.6lf\n", area);
}
