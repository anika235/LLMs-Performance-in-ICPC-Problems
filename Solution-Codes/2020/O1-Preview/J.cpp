#include <bits/stdc++.h>
using namespace std;

/*
   We store the tree in an adjacency list:
      adj[u] = vector of (v, w), meaning an edge u->v of length w.

   We'll do two DFS passes (or DFS + "reroot") to find, for each node u:
      bestDown[u] = length of the single longest downward path in subtree(u)
                    ignoring the parent edge.

   Then we'll compute bestUp[u], the longest path from the root or
   from parent's side, effectively going "up" from u's parent and possibly
   down other branches.

   In the end, for each node u, we gather the top 2 directions:
      - bestUp[u]
      - bestDown in each child's direction
   and pick the largest sum of two distinct directions.
   We'll keep track of that global maximum.
*/

static const int MAXN = 100000;

struct Edge {
    int v;
    int w;
};

int n;
vector<Edge> adj[MAXN+1];

// We will store the sum of edge lengths
long long sumDist = 0LL;

// bestDown[u] = length of the longest downward path from u into its subtree
long long bestDown[MAXN+1];

// bestUp[u] = length of the path if we go upward from u's parent (or parent's bestUp),
//             possibly down other branches, i.e. the best path "through the parent" for u.
long long bestUp[MAXN+1];

// We'll keep track of each node's top children for the re-root logic
// but a simpler approach is to do a second DFS passing along the parent's "bestUp".

// 1) DFS downward
void dfsDown(int u, int parent){
    bestDown[u] = 0; // if no children, it's 0
    for(auto &ed : adj[u]){
        int v = ed.v;
        int w = ed.w;
        if(v == parent) continue;
        // recurse
        dfsDown(v, u);
        long long cand = bestDown[v] + w;
        if(cand > bestDown[u]){
            bestDown[u] = cand;
        }
    }
}

// 2) DFS upward
void dfsUp(int u, int parent){
    // for each child, we need to compute bestUp[child]
    // bestUp[child] = max( bestUp[u] + w(u->child),
    //                      bestDown of siblings + w(u->sibling->child) )
    // We do this in two passes or we can do it in O(#children) with partial prefix maxima.

    // First gather all (bestDown[v] + w(u->v)) in a vector
    // to get top prefix/suffix for siblings. This is the classic "reroot" technique.

    // Build array of these child-downwards for all children
    vector<long long> childVals;
    vector<int> childIdx; 
    for(auto &ed : adj[u]){
        int v = ed.v; 
        if(v == parent) continue;
        childVals.push_back(bestDown[v] + ed.w);
        childIdx.push_back(v);
    }

    int ccount = (int)childVals.size();

    // prefixMax[i] = max of childVals[0..i-1]
    vector<long long> prefixMax(ccount+1, -1LL);
    // suffixMax[i] = max of childVals[i..end]
    vector<long long> suffixMax(ccount+1, -1LL);

    prefixMax[0] = -1LL; 
    for(int i=0; i<ccount; i++){
        prefixMax[i+1] = max(prefixMax[i], childVals[i]);
    }
    suffixMax[ccount] = -1LL;
    for(int i=ccount-1; i>=0; i--){
        suffixMax[i] = max(suffixMax[i+1], childVals[i]);
    }

    for(int i=0; i<ccount; i++){
        int v = childIdx[i];
        long long bestSibling = max(prefixMax[i], suffixMax[i+1]);
        // bestUp[v] can come from:
        // 1) bestUp[u] + edge(u->v)
        // 2) bestSibling + edge from u to v doesn't directly help v, we need just bestSibling if it is not for v's subtree
        // Actually bestUp[u] is the best path from parent's side to u.
        // So:
        long long candidate1 = bestUp[u] + (long long)adj[u][0].w; 
        // Actually be careful: we need the w(u->v). Let's store it from the edge.
        long long wuv = childVals[i] - bestDown[v]; // childVals[i] = bestDown[v] + w(u->v)
        // Actually let's keep it simpler:

        // bestUp[v] = max( bestUp[u] + w(u->v),
        //                  bestSibling + w(u->v) ), but we must find w(u->v).
        // We already have w(u->v) in childVals[i] - bestDown[v].
        long long w_edge = wuv;

        long long candidateUpFromParent = bestUp[u] + w_edge;  // go up from u and then down to v
        long long candidateUpFromSibling = (bestSibling == -1LL ? -1LL : bestSibling + w_edge);
        long long newVal = max(candidateUpFromParent, candidateUpFromSibling);

        bestUp[v] = max(bestUp[v], newVal); 

        // Recurse
        dfsUp(v, u);
    }
}

// However, the above approach can be tricky to implement bug-free in one pass. 
// Another, often simpler method is to do a second DFS where we pass the parent's "bestUp" 
// and the parent's bestDown from siblings as a parameter. 
// We'll do a simpler approach below with two DFS passes: 
//   1) dfsDown to compute bestDown[u], 
//   2) a re-root style dfs where we pass "parentContribution" (which is bestUp[u]) to children 
//      so that they can compute their bestUp[v].


// We'll store adjacency in a simpler structure for that approach:
static const long long NEG = -1e15;

vector<long long> bestDownVec;  // Just to avoid global re-declaration?

// We will define a new adjacency that stores only (v, w). We'll proceed with the two-pass method:

// We'll re-define:

long long bestDown2[MAXN+1]; 
// We'll do a single function getDown(u, p) that fills bestDown2[u].
void getDown(int u, int p){
    bestDown2[u] = 0;
    for(auto &ed : adj[u]){
        int v = ed.v, w = ed.w;
        if(v == p) continue;
        getDown(v, u);
        long long cand = bestDown2[v] + w;
        if(cand > bestDown2[u]){
            bestDown2[u] = cand;
        }
    }
}

// We'll also define bestUp2[u], and a function reRoot(u, p, distFromParent) that uses bestUp2[p] and bestDown2 siblings 
// to set bestUp2[u].
long long bestUp2[MAXN+1];

void reRoot(int u, int p, long long wpar){
    // We want to figure out bestUp2[u], which is the best path from "outside" of u's subtree going into u.
    // This can be: bestUp2[p] + wpar, or the bestDown2[sibling] + wpar, 
    // whichever is largest (for some sibling).
    // So we gather the maximum downward path from p that doesn't go through u.
    // We do the "largest from parent" trick: 
    // We find the top two children of p, if the top child is different from u, we use that, else we use second top child.

    // We'll do it in an O(1) or O(log deg(p)) approach if we precompute partial prefix/suffix arrays in the parent.
    // To keep it simpler, let's do a 2-step approach: 
    // a) find the bestDown2 of p if it goes to a child other than u (the best "sibling" path). We'll store in arrays so that we can query quickly.

    // For big n=100000, we do need an efficient approach. 
    // Typically you store for each node p the top two child downward values (and which child leads to them).
    // Then:
    //   bestUp2[u] = max( bestUp2[p] + wpar,
    //                     ( topDownOf_p if that child != u else 2ndDownOf_p ) + wpar )

    // After computing bestUp2[u], we recurse to children.

    // Implementation details:

    // 1) Identify the parent's top two children downward values
    // We'll assume we have arrays: bestChild1[p], bestVal1[p], bestChild2[p], bestVal2[p].
    // bestChild1[p] is the child c of p giving the largest bestDown2[c] + w(p->c).
    // bestVal1[p] is that largest value.
    // bestChild2[p], bestVal2[p] are the second largest.

    // We'll define them globally, fill them in after getDown() by a DFS. Then we do reRoot(…).
}

static int bestChild1[MAXN+1], bestChild2[MAXN+1];
static long long bestVal1[MAXN+1], bestVal2[MAXN+1];

// We'll fill (bestChild1[u], bestVal1[u]) = index and value of child with the largest downward path from u
// similarly for second-largest child
void dfsPrepareChildren(int u, int p){
    // compute among children v: bestDown2[v] + weight(u->v)
    long long top1 = -1, top2 = -1;
    int c1 = -1, c2 = -1;

    for(auto &ed : adj[u]){
        int v = ed.v; 
        long long w = ed.w;
        if(v == p) continue;
        long long val = bestDown2[v] + w;
        if(val > top1){
            top2 = top1;  c2 = c1;
            top1 = val;   c1 = v;
        } else if(val > top2){
            top2 = val;   c2 = v;
        }
    }
    bestVal1[u] = top1 < 0 ? 0 : top1;
    bestVal2[u] = top2 < 0 ? 0 : top2;
    bestChild1[u] = c1;
    bestChild2[u] = c2;

    // Recurse to children
    for(auto &ed : adj[u]){
        int v = ed.v;
        if(v == p) continue;
        dfsPrepareChildren(v, u);
    }
}

void reRoot2(int u, int p, long long wpar){
    // wpar = length(u->p)
    if(p == -1){
        // no parent
        bestUp2[u] = 0;
    } else {
        // parent's bestUp2 + wpar
        long long candidate1 = bestUp2[p] + wpar;
        // or parent's bestVal1 if parent's bestChild1 != u, else bestVal2
        long long siblingVal;
        if(bestChild1[p] != u){
            siblingVal = bestVal1[p];
        } else {
            siblingVal = bestVal2[p];
        }
        long long candidate2 = siblingVal < 0 ? 0 : (siblingVal + wpar);
        bestUp2[u] = max(candidate1, candidate2);
    }

    // Now reRoot for children
    for(auto &ed : adj[u]){
        int v = ed.v; 
        long long w = ed.w;
        if(v == p) continue;
        reRoot2(v, u, w);
    }
}

// Finally, having bestDown2[u] and bestUp2[u], we can compute, for each u, 
// the maximum sum of two disjoint directions from u: 
// directions can be (1) bestUp2[u] plus the largest child's bestDown, or
// or (2) largest child's bestDown plus second largest child's bestDown. 
// We'll take the maximum of these options.

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> n;
    // read edges
    long long totalEdgeLen = 0;
    for(int i=1; i<=n; i++){
        adj[i].clear();
    }
    for(int i=0; i<n-1; i++){
        int a,b; 
        long long d;
        cin >> a >> b >> d;
        adj[a].push_back({b,(int)d});
        adj[b].push_back({a,(int)d});
        totalEdgeLen += d;
    }

    // We'll pick node 1 as an arbitrary root for the DFS (the tree is connected).
    // 1) compute bestDown2[]:
    getDown(1, -1);
    // 2) prepare child info (bestChild1, bestVal1, bestChild2, bestVal2)
    dfsPrepareChildren(1, -1);
    // 3) compute bestUp2[] by re-rooting
    reRoot2(1, -1, 0);

    // Now find the maximum sum of two distinct directions for each u
    long long bestSkip = 0;
    for(int u=1; u<=n; u++){
        // We have bestUp2[u], bestVal1[u], bestVal2[u].
        // The top direction might be bestUp2[u], the second might be bestVal1[u], 
        // or the top two might be bestVal1[u] and bestVal2[u].
        // We'll consider all combinations that yield two distinct directions:

        // combination A: bestUp2[u] + bestVal1[u]
        long long combA = bestUp2[u] + bestVal1[u];

        // combination B: bestVal1[u] + bestVal2[u]
        long long combB = bestVal1[u] + bestVal2[u];

        long long localBest = max(combA, combB);
        bestSkip = max(bestSkip, localBest);
    }

    // The final answer is 2 * sum_of_edges - bestSkip
    long long ans = 2LL * totalEdgeLen - bestSkip;
    cout << ans << "\n";

    return 0;
}
