#include <bits/stdc++.h>
using namespace std;

// Epsilon for comparing floating-point lat/lon
static const double EPS = 1e-7;

// Compare two doubles for "almost equality"
bool almostEqual(double a, double b) {
    return fabs(a - b) < EPS;
}

// Wrap a longitude into [-180,180)
double wrapLon(double x) {
    // first into [0,360)
    x = fmod(x, 360.0);
    if(x < 0) x += 360.0;
    // then into [-180,180)
    if(x >= 180.0) x -= 360.0;
    return x;
}

// We'll hold points in a struct
struct Pt {
    double lat, lon;
};

// For sorting: sort primarily by lat ascending, if tie, by lon ascending.
bool cmpPt(const Pt &A, const Pt &B) {
    if(!almostEqual(A.lat, B.lat)) {
        return (A.lat < B.lat);
    }
    return (A.lon < B.lon);
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;

    vector<Pt> map1(n), map2(n);
    for(int i=0; i<n; i++){
        cin >> map1[i].lat >> map1[i].lon;
    }
    for(int i=0; i<n; i++){
        cin >> map2[i].lat >> map2[i].lon;
    }

    // Special case: n=1
    if(n==1) {
        // Check lat difference
        if(!almostEqual(map1[0].lat, map2[0].lat)) {
            cout << "Different\n";
        } else {
            // Always can shift the single longitude
            cout << "Same\n";
        }
        return 0;
    }

    // Sort each map by (lat, lon)
    sort(map1.begin(), map1.end(), cmpPt);
    sort(map2.begin(), map2.end(), cmpPt);

    // Group by lat for each map
    // We'll build arrays of (lat, vector<double> of lons sorted)
    vector< pair<double, vector<double>> > groups1, groups2;

    auto buildGroups = [&](const vector<Pt> &arr){
        vector< pair<double, vector<double>> > result;
        int i=0, sz=(int)arr.size();
        while(i<sz){
            double currentLat = arr[i].lat;
            vector<double> block;
            block.push_back(arr[i].lon);
            i++;
            while(i<sz && almostEqual(arr[i].lat, currentLat)) {
                block.push_back(arr[i].lon);
                i++;
            }
            // sort block by lon ascending (already sorted, actually, because of the main sort).
            // but let's confirm it's sorted. It's indeed already sorted, no need to re-sort.
            result.push_back({currentLat, block});
        }
        return result;
    };

    groups1 = buildGroups(map1);
    groups2 = buildGroups(map2);

    // They must have same number of lat-groups
    if(groups1.size() != groups2.size()) {
        cout << "Different\n";
        return 0;
    }

    // Check lat equality and #points in each group
    for(int i=0; i<(int)groups1.size(); i++){
        if(!almostEqual(groups1[i].first, groups2[i].first)) {
            cout << "Different\n";
            return 0;
        }
        if(groups1[i].second.size() != groups2[i].second.size()) {
            cout << "Different\n";
            return 0;
        }
    }

    // We'll pick the largest group as a reference to define the shift.
    int refIdx = 0;
    int maxSize = 0;
    for(int i=0; i<(int)groups1.size(); i++){
        int sz = (int)groups1[i].second.size();
        if(sz > maxSize) {
            maxSize = sz;
            refIdx = i;
        }
    }

    // Attempt to find a shift from that reference group
    // Let S = groups1[refIdx].second, T = groups2[refIdx].second
    // both sorted. We'll define shift = T[0] - S[0] mod 360, then see if SHIFT(S, shift) = T
    // If that fails => "Different".
    // If success => check the same shift for all other groups.

    auto &S = groups1[refIdx].second; 
    auto &T = groups2[refIdx].second;
    // define candidate shift
    double rawShift = T[0] - S[0]; // in degrees
    // wrap to [-180,180)
    rawShift = wrapLon(rawShift);

    // function to shift and compare
    auto shiftAndCompare = [&](const vector<double> &A, const vector<double> &B, double shift){
        // A and B must have same size
        int sz = (int)A.size();
        vector<double> tmp(sz);
        for(int i=0; i<sz; i++){
            double x = A[i] + shift;
            x = wrapLon(x);
            tmp[i] = x;
        }
        sort(tmp.begin(), tmp.end()); // must be re-sorted after shift
        // Compare with B
        for(int i=0; i<sz; i++){
            if(!almostEqual(tmp[i], B[i])) {
                return false;
            }
        }
        return true;
    };

    // check the reference group
    if(!shiftAndCompare(S, T, rawShift)) {
        // Might the sets match if we pick T[0] vs. S[k] for k>0?  
        // In principle, yes.  But if the sets truly match by a single shift, 
        // matching the smallest elements will also work.  
        // So no second try is needed if they are strictly sorted sets.  
        // => "Different".
        cout << "Different\n";
        return 0;
    }

    // Now we verify the same shift for all other groups
    for(int i=0; i<(int)groups1.size(); i++){
        if(i==refIdx) continue; // already done
        auto &g1 = groups1[i].second;
        auto &g2 = groups2[i].second;
        if(!shiftAndCompare(g1, g2, rawShift)) {
            cout << "Different\n";
            return 0;
        }
    }

    // If we never fail, it's consistent
    cout << "Same\n";
    return 0;
}
