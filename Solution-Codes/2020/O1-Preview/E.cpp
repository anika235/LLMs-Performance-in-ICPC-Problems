#include <bits/stdc++.h>
using namespace std;

/*
  We have n points (1..n), initially all heights = 0.

  We have k modifications of 4 types:
   1) R x1 x2: raise all points in [x1..x2] by 1
   2) D x1 x2: depress all points in [x1..x2] by 1
   3) H x1 x2: add a "hill" from x1..x2 (linear / triangular shape, +1 at edges, +2 one step in, etc.)
   4) V x1 x2: add a "valley" (same as hill but negative)

  We want the final height at each of the n points after all operations.

  Constraints:
   - 1 <= n <= 200000
   - 0 <= k <= 200000
   We need an O(n + k) or O((n + k) log n) solution.  A naive O(n*k) would be too large (up to 4e10).

  -----------------------------------------------------------------------
  Key Idea: "Difference arrays" for piecewise linear updates
  -----------------------------------------------------------------------
  We'll maintain two "difference" arrays (each of size n+2 or so, to avoid boundary issues):

    1) diff[i]:  controls a "constant offset" at index i
    2) slope[i]: controls a "running slope" change starting at index i

  Then, to recover the final height H[j], we do something like:

     let s = 0   (current slope)
     let val = 0 (current height)
     for j in [1..n]:
       s += slope[j]         // adjust slope at j
       val += (diff[j] + s)  // add both the offset jump and the slope contribution
       H[j] = val

  In other words, at each index j:
    - slope[j] modifies the slope from j onward
    - diff[j] modifies the immediate jump at j
    - we keep a running sum "s" of all slope[...] up to j, and a running sum "val" of everything

  We'll define helper functions to add a "flat" update, or a "hill/valley" update, in O(1) each:

  1) R x1 x2 => raise by +1 on [x1..x2]
       We'll do:
         diff[x1]   += +1   // start +1 from x1 onward
         diff[x2+1] -= +1   // stop that +1 after x2

  2) D x1 x2 => depress by -1 on [x1..x2]
       Similar:
         diff[x1]   += -1
         diff[x2+1] -= -1

  3) H x1 x2 => "hill" shape. Let L = (x2 - x1). If L=0, just +1 at x1. If L>0, it forms a triangular bump.
     The shape at integer j in [x1..x2] is: height_change(j) = 1 + min( j - x1, x2 - j )
       (peak is ~ (x1 + x2)/2).
     We can implement this with two linear ramps: from x1 to the midpoint we go up +1 each step, from midpoint to x2 we go down -1 each step, with correct maximum in the middle.

     - We'll add +1 at x1, so diff[x1] += +1
     - We'll also add +1 to slope[x1+1], meaning from x1+1 onward, the slope is +1 (going upward) 
       (if x1+1 <= x2, otherwise no effect if x1==x2).
     - Then at the midpoint, we reduce the slope again (switch from going up to going down).
       The exact midpoint logic:
         mid_left  = (x1 + x2)//2
         mid_right = mid_left + ( (x2 - x1 + 1) % 2 == 0 ? 0 : 1 )
       Actually, if x2 - x1 is even, there's one peak at mid_left. 
       If x2 - x1 is odd, there's a "double" peak at mid_left and mid_left+1.
     - We also must stop the upward slope at mid_left+1 (or mid_right+1), then start a downward slope 
       from that point until x2.  Then we must cancel everything after x2+1.

     Summarily, we can break the hill into two "ramps":
       segment1: from [x1..mid_left], slope +1
       segment2: from [mid_right..x2], slope -1
       and we also set the correct offset adjustments so it forms the correct triangular shape 
       with "1 + min(...)"

     However, we also need the "base offset" to be +1 at x1 (since height_change(x1)=1).
     Then from x1+1.. x1+2.. it accumulates +2, +3,... up to the midpoint. 
     This is exactly how a "running slope" approach works.

     We'll do a function addHill(x1, x2, sign=+1 or -1 for valley) that in O(1) updates the diff/slope arrays:
       let L = (x2 - x1)
       if L==0:
         diff[x1] += sign
         return

       mid = (x1 + x2) // 2   (integer division)
       if L is even => single peak at mid
         => from x1..mid, slope +1
            from mid+1..x2, slope -1
            plus base offset +1 at x1
       if L is odd => double peak at mid, mid+1
         => from x1..mid, slope +1
            from mid+1..mid+1, slope 0  (stays at max)
            from mid+2..x2, slope -1
            plus base offset +1 at x1
       Then we also must "stop" these slopes after x2, 
         so slope[x2+1] cancels ( add +1 or -1 again to revert slope to 0 ).
       We do everything multiplied by sign for valley vs hill.

     The net effect is that, when we do the final pass, each index j in [x1..x2] will get exactly 
     sign*(1 + min(j-x1, x2-j)) added.

  Implementation detail: We can unify the even/odd logic by noticing that the "plateau" in the center 
  (for odd L) is effectively just 1 index wide if L is even, or 2 indices wide if L is odd.  
  We'll handle that carefully in the code.

  -----------------------------------------------------------------------
  After we've processed all k modifications via these O(1) difference-structure updates, 
  we do one pass through [1..n] to compute the final heights in O(n). 
  Then print them.

  The overall complexity is O(k + n). That is good for n,k up to 200k.

  We'll now implement the solution.
*/


static const int MAXN = 200000;  // per the problem statement

// We have arrays (size n+2 to avoid boundary issues):
//   diff[i]  -> immediate height change starting at i
//   slope[i] -> slope increment starting at i
// We'll fill them in, then do a single pass to compute final heights.

static long long diffArr[ MAXN+5 ];
static long long slopeArr[ MAXN+5 ];

inline void rangeAdd_Constant(int L, int R, long long val) {
    // Add +val to [L..R] in the "diffArr" sense
    // i.e. diffArr[L] += val; diffArr[R+1] -= val;
    diffArr[L] += val;
    if(R+1 < MAXN+5) diffArr[R+1] -= val;
}

inline void rangeAdd_Slope(int L, int R, long long val) {
    // Add a slope of +val from L..R
    // i.e. slopeArr[L] += val; slopeArr[R+1] -= val;
    slopeArr[L] += val;
    if(R+1 < MAXN+5) slopeArr[R+1] -= val;
}

// Add a "hill" (sign=+1) or "valley" (sign=-1) from x1..x2
// using piecewise linear logic with slope array
void addHillOrValley(int x1, int x2, int sign){
    if(x1 > x2) return;
    int L = x2 - x1;
    // trivial case
    if(L == 0){
        // single point gets +1 or -1
        rangeAdd_Constant(x1, x1, sign);
        return;
    }
    // We always do an immediate +1 at x1 (the "edge" of the shape)
    rangeAdd_Constant(x1, x1, sign);

    // We'll define the left "up ramp" from x1+1.. midLeft
    // Then possibly a plateau if L is odd, then a right "down ramp".
    int midLeft = (x1 + x2) / 2;  
    // If L is even => single peak at midLeft
    // If L is odd  => two-peak region at midLeft, midLeft+1

    // Up ramp:
    //   from x1+1 to midLeft => slope +sign
    //   (provided x1+1 <= midLeft)
    if(x1+1 <= midLeft){
        rangeAdd_Slope(x1+1, midLeft, sign);  
    }

    // If L is even => we start the downward slope at midLeft+1
    // If L is odd  => we start the downward slope at midLeft+2 
    // because there's a second peak at midLeft+1
    int startDown = midLeft + 1;
    if( (x2 - x1) % 2 == 1 ){
        // odd length => double peak => the second peak is midLeft+1
        // so we do not start going down until midLeft+2
        startDown = midLeft + 2;
    }

    // Down ramp:
    //   from startDown.. x2 => slope -sign
    if(startDown <= x2){
        rangeAdd_Slope(startDown, x2, -sign);
    }

    // We also need to make sure the slope is canceled after x2
    // but that's handled by rangeAdd_Slope(...) which does slopeArr[x2+1] -= ...
    // so that after x2 the slope returns to 0 automatically.

    // done
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n,k; 
    cin >> n >> k;

    // Initialize arrays
    for(int i=0; i<=n+2; i++){
        diffArr[i] = 0;
        slopeArr[i] = 0;
    }

    // Read each modification and perform O(1) difference updates
    for(int _i=0; _i<k; _i++){
        char c; 
        int x1,x2;
        cin >> c >> x1 >> x2;
        // ensure 1 <= x1 <= x2 <= n
        if(c=='R'){
            // Raise [x1..x2] by 1
            rangeAdd_Constant(x1, x2, +1LL);
        } else if(c=='D'){
            // Depress [x1..x2] by 1
            rangeAdd_Constant(x1, x2, -1LL);
        } else if(c=='H'){
            // Hill
            addHillOrValley(x1, x2, +1); 
        } else { 
            // c=='V'
            // Valley
            addHillOrValley(x1, x2, -1);
        }
    }

    // Now compute final heights in one pass
    long long s = 0;    // running slope
    long long val = 0;  // running height
    for(int i=1; i<=n; i++){
        s   += slopeArr[i];   // update slope at i
        val += (diffArr[i] + s); 
        // that is the new height at i
        cout << val << "\n";
    }

    return 0;
}
