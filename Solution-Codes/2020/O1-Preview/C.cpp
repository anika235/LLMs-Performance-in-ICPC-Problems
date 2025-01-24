#include <bits/stdc++.h>
using namespace std;

/*
  ------------------------------------------------------------
  QUICK EXPLANATION (very abbreviated, as requested):
  We have n domes inside the rectangle [0,dx] x [0,dy].
  We want the set of all points P in this rectangle such that:
    1) The domes appear in the *desired left-to-right order*
       when viewed from P.
       --> For each adjacent pair (domes[i], domes[i+1]) in
           the requested ordering, we need cross(
             (domes[i] - P), (domes[i+1] - P)
           ) > 0.  (Strictly > 0 means i-th dome is to the
           left of (i+1)-th dome in the view from P.)
    2) All n domes can fit into a 180-degree camera view.
       --> Equivalently, there exists some line through P
           such that all domes lie in the same closed half-plane
           defined by that line.  Another well-known fact:
           the angle subtended by the n points at P is ≤ 180°
           if and only if P lies *outside or on* the convex hull
           of those domes (otherwise they'd wrap > 180° around P).

  Steps:
    A) Index the domes according to the permutation (d1,...,dn).
    B) Build half-plane constraints for "d_k is left of d_{k+1}".
       For each pair (d_k, d_{k+1}), the vantage point P must
       satisfy cross(d_k - P, d_{k+1} - P) > 0.  Rearrange to get
       cross(P - d_k, P - d_{k+1}) < 0 if you prefer.  Each such
       constraint is a half-plane in the plane.
    C) Also build a half-plane constraint for "P is outside or on
       the convex hull" of all domes to ensure 180° coverage. 
       In practice, one can compute the convex hull of the n
       domes, then the feasible region for P is the *exterior*
       of that hull (including its boundary).
    D) Intersect all these half-planes with the rectangle [0,dx] x [0,dy].
    E) Compute the area of the resulting region (which may be 0).

  Because n ≤ 100, we can afford to:
    - Compute the convex hull in O(n log n).
    - For each consecutive pair in the permutation, form the
      corresponding half-plane.  
    - For the hull, form the *exterior* half-planes for each
      hull edge.
    - Then do a standard half-plane intersection in O((n + n)*log(n))
      = O(n log n) or O(n^2), which is okay for n=100.
    - Finally clip against the bounding rectangle and compute
      polygon area.

  The code below sketches this approach.  It omits some
  lower-level details in favor of clarity.  In a real contest
  solution, one must carefully implement half-plane intersection,
  polygon clipping, and so on.  Here, we show a reasonably
  complete version using standard geometry techniques.
*/

struct Pt {
  double x,y;
};

////////////////////  cross product  ////////////////////
double cross(const Pt &A, const Pt &B, const Pt &C) {
  // cross(AB, AC) = (B - A) x (C - A)
  return (B.x - A.x)*(C.y - A.y) - (B.y - A.y)*(C.x - A.x);
}

////////////////////  convex hull  //////////////////////
vector<Pt> convexHull(vector<Pt> pts) {
  // returns the convex hull in CCW order
  // standard Andrew's monotone chain or Graham scan
  sort(pts.begin(), pts.end(), [](auto &a, auto &b){
    if(a.x==b.x) return a.y<b.y;
    return a.x<b.x;
  });
  // build lower hull
  vector<Pt> hull;
  for(int i=0; i<(int)pts.size(); i++){
    while(hull.size()>1 && cross(hull[hull.size()-2], hull[hull.size()-1], pts[i]) <= 0){
      hull.pop_back();
    }
    hull.push_back(pts[i]);
  }
  // build upper hull
  for(int i=(int)pts.size()-2, t=hull.size()+1; i>=0; i--){
    while((int)hull.size()>=t && cross(hull[hull.size()-2], hull[hull.size()-1], pts[i]) <= 0){
      hull.pop_back();
    }
    hull.push_back(pts[i]);
  }
  hull.pop_back();
  return hull;
}

//////////////////////////////////////////////////////////
// We define a half-plane by the line AB (points A,B) and
// a direction "left side" or "right side". We'll store
// them as (A,B, keepLeftSide).
//////////////////////////////////////////////////////////
struct HalfPlane {
  Pt A,B;
  bool keepLeft;
};

////////////////////  clip polygon by one half-plane  //////////////////////
// standard polygon clipping against line AB, keeping the left side if keepLeft==true
// or the right side otherwise.
vector<Pt> clipPoly(const vector<Pt> &poly, const HalfPlane &hp) {
  vector<Pt> res;
  for(int i=0; i<(int)poly.size(); i++){
    Pt cur = poly[i];
    Pt nxt = poly[(i+1)%poly.size()];
    double c1 = cross(hp.A, hp.B, cur);
    double c2 = cross(hp.A, hp.B, nxt);
    bool in1 = (hp.keepLeft ? (c1 >= 0) : (c1 <= 0));
    bool in2 = (hp.keepLeft ? (c2 >= 0) : (c2 <= 0));
    if(in1) res.push_back(cur);
    if(in1 != in2) {
      // There's an intersection with the boundary line
      // parametric intersection with line segment cur->nxt
      double denom = ( (nxt.x - cur.x)*(hp.A.y - hp.B.y) -
                       (nxt.y - cur.y)*(hp.A.x - hp.B.x) );
      // if denom==0 => segment is parallel to line AB (rare in this problem, ignore or handle carefully)
      if(fabs(denom)<1e-12) continue; // skip degenerate
      double t = ( (cur.x - hp.A.x)*(hp.A.y - hp.B.y) -
                   (cur.y - hp.A.y)*(hp.A.x - hp.B.x) ) / denom;
      Pt inter;
      inter.x = cur.x + t*(nxt.x - cur.x);
      inter.y = cur.y + t*(nxt.y - cur.y);
      res.push_back(inter);
    }
  }
  return res;
}

//////////////////// polygon area  //////////////////////
double polyArea(const vector<Pt> &poly) {
  double area=0.0;
  for(int i=0;i<(int)poly.size(); i++){
    Pt a=poly[i], b=poly[(i+1)%poly.size()];
    area += (a.x*b.y - a.y*b.x);
  }
  return fabs(area)/2.0;
}

int main(){
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  int dx, dy, n;
  cin >> dx >> dy >> n;
  vector<Pt> domes(n);
  for(int i=0;i<n;i++){
    cin >> domes[i].x >> domes[i].y;
  }
  vector<int> perm(n);
  for(int i=0;i<n;i++){
    cin >> perm[i];
    perm[i]--; // make 0-based
  }

  // 0) Build initial polygon = the entire Red Square
  // in CCW order
  vector<Pt> region;
  region.push_back({0.0, 0.0});
  region.push_back({(double)dx, 0.0});
  region.push_back({(double)dx, (double)dy});
  region.push_back({0.0, (double)dy});

  // 1) Add half-plane constraints for consecutive pairs in the desired order:
  //    We want cross(d_k - P, d_{k+1} - P) > 0
  //    <=> cross(P, d_k, d_{k+1}) < 0 if you rearrange.  We'll keep a consistent form:
  //    We define line (d_k, d_{k+1}), keepLeft==true means we keep points P
  //    for which cross(d_k, d_{k+1}, P) >= 0.  But we want the opposite sign.
  //    Let's do a direct approach: cross(P, d_k, d_{k+1}) > 0 means from P's perspective,
  //    d_k is to the left of d_{k+1}. We want that to be true for all pairs (k,k+1).
  //    So we define half-plane line = (d_{k+1}, d_k) so that "keepLeft==true" means cross(...)>=0
  //    matches cross(P, d_k, d_{k+1})>0.  We'll do cross(d_{k+1}, d_k, P)>=0.

  for(int i=0;i<n-1;i++){
    Pt A = domes[perm[i+1]];
    Pt B = domes[perm[i]];
    // keepLeft means cross(A,B,P) >= 0
    HalfPlane hp = {A, B, true};
    region = clipPoly(region, hp);
    if(region.empty()) break;
  }

  // 2) For 180-degree coverage: vantage point must be outside
  //    or on the convex hull of all domes, so that the angle
  //    they subtend at P is <= 180.  The "exterior" of the hull
  //    is the intersection of half-planes that keep the *outside* side
  //    of each hull edge. We'll build that.
  vector<Pt> hull = convexHull(domes);
  // hull is CCW.  For each edge hull[i] -> hull[i+1], the interior is on the left.
  // We want vantage points on the *right side or on the line*, i.e. cross(...) <= 0
  // for being "outside or on" if we treat edges in CCW order. We'll do keepLeft=false
  // with line(A,B) = (hull[i], hull[i+1]) to keep the right side.
  for(int i=0;i<(int)hull.size(); i++){
    Pt A=hull[i];
    Pt B=hull[(i+1)%hull.size()];
    HalfPlane hp = {A, B, false}; // keepRight side
    region = clipPoly(region, hp);
    if(region.empty()) break;
  }

  // 3) The result is 'region'.  Compute area:
  double ans = 0.0;
  if(!region.empty()){
    ans = polyArea(region);
  }

  cout << fixed << setprecision(6) << ans << "\n";
  return 0;
}
