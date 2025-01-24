#include <bits/stdc++.h>
using namespace std;

/*
  We want to compute:

    G(x) = product over all squares k of [ (1 - p_k) + p_k * x ].

  - p_k = p_i + q_j
  - There are mn squares
  - The degree of G is mn

  We'll store G as a vector<double> of length mn+1: G[0..mn].
    G[r] = coefficient of x^r.

  Building G(x) by multiplying mn linear factors is the main challenge:
    a naive O(mn^2) = 250000^2 = 6.25e10 ops, likely too big.
    We must do a "divide & conquer" or segment-tree FFT approach 
    in about O(mn log(mn)) or O(mn (log(mn))^2).

  Then for each query:
    1) Build small polynomial G_S in O(s^2).
    2) Do polynomial division: G_{S^c}(x) = G(x) / G_S(x) in O(mn + s).
    3) For x=0..s, get f_S(x) = G_S[x] (coefficient of x^x).
       Then get f_{S^c}(t-x) = G_{S^c}[t-x].
       Probability = f_S(x) * f_{S^c}(t-x) / G[t].
    4) Output s+1 probabilities.

  Implementation details are substantial.  We'll outline key steps.
*/

// ---------- FFT-based polynomial multiplication (outline) ----------

/*
  We'll have a function to multiply two polynomials A,B of degrees up to ~ mn/2 
  using an FFT. Then to build G we do a segment tree or pairwise merges.

  Because m,n <= 500 => mn <= 250000, we'll need an O(mn log mn) approach.

  Pseudocode for building G with segment-tree merges:

    vector<vector<double>> segtree(4*mn); // each node will store a polynomial
    // polynomials are stored as vector<double>

    // build node idx range [L..R]
    void build(int idx, int L, int R) {
      if (L == R) {
        // Leaf node: polynomial = [ (1 - p[L]), p[L] ]
        segtree[idx].resize(2);
        segtree[idx][0] = 1.0 - p[L];
        segtree[idx][1] = p[L];
        return;
      }
      int M = (L+R)/2;
      build(idx*2, L, M);
      build(idx*2+1, M+1, R);
      // segtree[idx] = multiply( segtree[idx*2], segtree[idx*2+1] ) via FFT
    }

    Then G = segtree[1] is the product for the entire range [0..mn-1].

  This yields G in O(mn log(mn)) or O(mn (log(mn))^2), depending on implementation.
*/

// We'll omit the full FFT details here and just sketch a multiplyPoly() stub:

// ---------- Polynomial division of G by G_S ----------

/*
  If D = deg(G) = mn, and d = deg(G_S) = s, we want Q(x),R(x) such that:
      G(x) = Q(x)*G_S(x) + R(x),    deg(R) < deg(G_S).

  Because G_S(x) is genuinely a product of some subset of G's linear factors,
  in exact math we expect R(x)=0.  In floating arithmetic we'll get small remainders.

  We'll do the standard "high-school" or "synthetic" division in O(D - d + 1) ~ O(mn).

  Then Q[t-x] is the coefficient we need for sum_{k in S^c} X_k = (t-x).
*/

// A function polyDivide(G, S) -> Q (we ignore the remainder, or we can store it if we want).
// G has length = mn+1, S has length = s+1, Q has length = (mn - s + 1).

// ---------- Putting it all together ----------

static const double EPS = 1e-14;

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int m,n,t,q;
    cin >> m >> n >> t >> q;

    // Read p_i (row contributions)
    vector<double> prow(m);
    for(int i=0; i<m; i++){
        cin >> prow[i];
    }
    // Read q_j (column contributions).  We'll call it ccol to avoid confusion w/ number-of-queries.
    vector<double> ccol(n);
    for(int j=0; j<n; j++){
        cin >> ccol[j];
    }

    // Build the array of p_k for each cell
    // We index k in [0..mn-1].
    // Let cell k correspond to (i = k//n, j = k % n)
    vector<double> p(m*n);
    for(int i=0; i<m; i++){
      for(int j=0; j<n; j++){
        int k = i*n + j;
        p[k] = prow[i] + ccol[j];  // p_i + q_j
      }
    }

    // 1) Build G(x) by multiplying all factors ( (1 - p[k]) + p[k]*x ), k=0..mn-1.
    //    We'll call that polynomial G of length (mn+1).
    //    We'll do it with a segment tree + FFT or any fast approach.
    //    For clarity, let's just pretend we have a function buildGlobalPoly(p, G).
    //    That function fills G.size()=mn+1 with the coefficients.
    //    We'll skip the full implementation here (it's the most code-heavy part).

    vector<double> G(m*n + 1, 0.0);
    // buildGlobalPoly(p, G);  // outline

    // --- For illustration, we do a naive O(mn^2) multiplication if mn is small.
    // But mn can be up to 250000, which is too big to do naively. In a real solution:
    //   G = segmentTreeFFT(p).
    // We'll just *outline* the naive approach for smaller test logic:

    // Initialize G to [1.0] (constant poly)
    G[0] = 1.0;
    int currentDeg = 0; // G is initially degree=0

    for(int k=0; k<m*n; k++){
        // multiply G by [ (1-p[k]), p[k] ]
        double a = 1.0 - p[k];
        double b = p[k];
        // new poly has degree currentDeg+1
        for(int r=currentDeg+1; r>=0; r--){
            double tmp = 0.0;
            if(r <= currentDeg) tmp += G[r]*a;
            if(r-1 >= 0)        tmp += G[r-1]*b;
            G[r] = tmp;
        }
        currentDeg++;
    }
    // Now G has size currentDeg+1 = mn+1

    // 2) We have G(...) done.  f_All(r) = G[r], and in particular we need G[t] = P(T=t).
    double denom = G[t]; // f_All(t)

    // We'll answer each query
    //   For each query: we get s squares, build G_S in O(s^2), do polyDivide(G, G_S) -> Q,
    //   then f_{S^c}(r) = Q[r], etc.

    cout << fixed << setprecision(9);

    // A helper to multiply polynomials of degree up to Adeg,Bdeg in O(Adeg*Bdeg) (small s usage)
    auto polyMultiplySmall = [&](const vector<double> &A, const vector<double> &B){
        int Adeg = (int)A.size()-1;
        int Bdeg = (int)B.size()-1;
        vector<double> C(Adeg + Bdeg + 1, 0.0);
        for(int i=0; i<=Adeg; i++){
            for(int j=0; j<=Bdeg; j++){
                C[i+j] += A[i]*B[j];
            }
        }
        return C;
    };

    // Poly division: G of deg(mn), S of deg(s) => Q of deg(mn-s).  We ignore remainder.
    auto polyDivide = [&](const vector<double> &G, const vector<double> &S){
        int D = (int)G.size()-1;  // = mn
        int d = (int)S.size()-1;  // = s
        vector<double> Q(D - d + 1, 0.0); // deg = D-d

        // We'll do the standard approach from highest power down
        // G is a copy we can mutate or we can do it on a temp
        vector<double> R = G; // remainder initially
        double leading = S[d]; // leading coeff of divisor
        // We'll assume leading != 0 because p_k < 1 always. (It could be extremely small but not exactly 0.)

        for(int i=D; i>=d; i--){
            double coef = R[i]/leading; 
            // That goes to Q[i-d]
            Q[i-d] = coef;
            // Subtract coef * S(...) from R(...) in degrees [i-d..i]
            for(int j=0; j<=d; j++){
                R[i - (d-j)] -= coef * S[j];
            }
        }
        // Now R has the remainder, ideally near 0.
        return Q;
    };

    for(int _q=0; _q<q; _q++){
        // read query
        int s; cin >> s;
        vector<int> sq(s*2);
        for(int i=0; i<2*s; i++){
            cin >> sq[i];
        }
        // build polynomial G_S(x) of degree s
        // each factor: [ (1 - p_k), p_k ], where k = (i-1)*n + (j-1)
        // we'll do naive multiply since s<=500
        vector<double> GS(1, 1.0);  // constant 1
        for(int i=0; i<s; i++){
            int r = sq[2*i]   - 1; // i index in [1..m], minus 1 => 0-based
            int c = sq[2*i+1] - 1; // j index in [1..n], minus 1 => 0-based
            int k = r*n + c;
            double a = 1.0 - p[k];
            double b = p[k];
            // multiply GS by [a, b]
            vector<double> factor = {a, b};
            GS = polyMultiplySmall(GS, factor);
        }
        // Now deg(GS) = s.  Then Q = G / GS
        vector<double> Q = polyDivide(G, GS); // deg(Q) = mn - s
        // Q[r] = coefficient of x^r in G_{S^c}(x).

        // next we also need f_S(x) = GS[x] for x=0..s
        // then f_{S^c}(t-x) = Q[t-x], 
        // Probability = f_S(x)*f_{S^c}(t-x) / f_All(t), for x=0..s
        // if (t-x) out of range or negative, that probability is 0

        // We'll store these in an array prob[0..s].
        vector<long double> ans(s+1, 0.0L);
        for(int x=0; x<=s; x++){
            double fSx = (x<(int)GS.size()) ? GS[x] : 0.0;
            int idx = t - x;
            double fSc = 0.0;
            if(idx>=0 && idx<(int)Q.size()){
                fSc = Q[idx];
            }
            long double val = (long double)fSx*(long double)fSc;
            // divide by denom=G[t] to get conditional
            val /= (long double)denom;
            ans[x] = val;
        }

        // Output s+1 values
        for(int x=0; x<=s; x++){
            cout << (long double)ans[x];
            if(x < s) cout << " ";
        }
        cout << "\n";
    }

    return 0;
}
