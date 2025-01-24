#include <bits/stdc++.h>
using namespace std;

// We'll use 64-bit for cross products to avoid overflow, but be mindful
// that cross could go up to ~4e18 which still fits into 64-bit (±9e18 max).
// Distances will be stored as double.

static const double EPS = 1e-12;

// A quick struct to hold points
struct Point {
    long long x, y;
};

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    long long T;  // 't' can be up to 1e9, store as long long
    cin >> n >> T;

    vector<Point> pts(n);
    for(int i=0; i<n; i++){
        cin >> pts[i].x >> pts[i].y;
    }
    
    // Edge cases:
    // 1) n <= 2 trivially, but the problem states n >= 3, so no need for special handling.
    // 2) If T=0 and no three are collinear => the maximum on a line is 2.
    //    However, let's not break early; the main method will handle T=0 gracefully.

    if(n == 3 && T == 0){
        // If T=0 and no three collinear => max is 2.
        cout << 2 << "\n";
        return 0;
    }

    // We'll store the maximum number of points on a thick line
    int answer = 1; 
    // (At least 1 point is trivially covered by any line, but we expect at least 2 or more.)

    // Precompute sqrt(...) once could help, but we must do it for each pair anyway
    // since each pair has its own direction.

    // We'll iterate over pairs (i,j) with i < j
    // That gives a direction. Then we compute all distances and do a 2-pointer in sorted order.
    // Keep track of global max.

    for(int i=0; i<n; i++){
        // To reduce overhead, we can do j from i+1..n-1
        // Also, if n is large, you might want some small early break optimizations if answer ~ n
        // but let's keep it straightforward here.

        // We can also do a small optimization: if n-i + 1 <= answer, no need to continue
        // because we can't possibly beat "answer" with fewer points left. 
        // But let's leave it out for clarity.

        for(int j=i+1; j<n; j++){
            // Direction vector (dx, dy)
            long long dx = pts[j].x - pts[i].x;
            long long dy = pts[j].y - pts[i].y;

            // Length of the direction vector
            // We do it as double to avoid overflow; sqrt of up to ~ (2e9)^2 + (2e9)^2 is ~2.8e9
            long double len = sqrt((long double)dx*dx + (long double)dy*dy);
            if(len < EPS) {
                // Should not happen because all points are distinct,
                // but just in case, skip
                continue;
            }

            // Compute signed distances for all points
            // We'll use the cross product formula:
            // dist = ((p - i) x (j - i)) / length(j - i)
            // But for the "maximum coverage with shift s" problem,
            // we want D[k] = ( (p_k - p_i) x (p_j - p_i) ) / len
            // We'll store them in an array of doubles
            vector<long double> distVals;
            distVals.reserve(n);

            // Reference: cross((p - pi), (pj - pi)) = (px - pix)*(dy) - (py - piy)*(dx)
            // We'll do a cast to long double for safety.
            long double DX = (long double)dx, DY = (long double)dy;

            for(int k=0; k<n; k++){
                long double rx = (long double)pts[k].x - pts[i].x; 
                long double ry = (long double)pts[k].y - pts[i].y;
                long double crossVal = rx*DY - ry*DX;  // cross product in 2D
                // signed distance
                long double d = crossVal / len;
                distVals.push_back(d);
            }

            // Now sort distVals
            sort(distVals.begin(), distVals.end());

            // We want the largest subarray distVals[L..R] such that distVals[R] - distVals[L] <= 2*T
            // We'll do a classic two-pointer.
            long double twoT = (long double)T * 2.0L;
            int left = 0;
            for(int right=0; right<n; right++){
                // Expand right pointer
                // Move left pointer if needed
                while(distVals[right] - distVals[left] > twoT){
                    left++;
                }
                int count = right - left + 1;
                if(count > answer) {
                    answer = count;
                    // Optional early break if answer == n => can't do better
                    if(answer == n){
                        // We can break out entirely if we want
                        break;
                    }
                }
            }

            // If we found all n, break the outer loop too
            if(answer == n) break;
        }

        if(answer == n) break;
    }

    cout << answer << "\n";
    return 0;
}
