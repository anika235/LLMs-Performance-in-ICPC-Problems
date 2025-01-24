#include <bits/stdc++.h>
using namespace std;

// We'll use 64-bit integers (long long) for x,y,z since they can go up to 1e9.
// The sum of positive differences can reach up to 3e9, which still fits in 64-bit.

static const int TOP = 200;  // We'll collect top-200 in each dimension

struct Phone {
    long long x, y, z;
    int idx;  // 1-based index in input
};

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n; 
    cin >> n;

    vector<Phone> phones(n);
    for(int i = 0; i < n; i++){
        cin >> phones[i].x >> phones[i].y >> phones[i].z;
        phones[i].idx = i+1;  // store 1-based index
    }

    // If n == 2, the top-200 logic still works the same, but let's just do it uniformly.

    // We'll build three sorted arrays:
    // Sort by x descending
    vector<Phone> sortByX = phones;
    sort(sortByX.begin(), sortByX.end(), [](auto &a, auto &b){
        return a.x > b.x;  // descending
    });

    // Sort by y descending
    vector<Phone> sortByY = phones;
    sort(sortByY.begin(), sortByY.end(), [](auto &a, auto &b){
        return a.y > b.y;  // descending
    });

    // Sort by z descending
    vector<Phone> sortByZ = phones;
    sort(sortByZ.begin(), sortByZ.end(), [](auto &a, auto &b){
        return a.z > b.z;  // descending
    });

    // Collect top-200 indices from each
    // We'll store the actual phone objects in a set (or vector + unique).
    // We'll do it by pushing the references into a vector<int>.
    unordered_set<int> topSet; 
    topSet.reserve(3 * TOP + 50); // a bit more

    auto pushTop = [&](vector<Phone> &arr){
        for(int i=0; i < min((int)arr.size(), TOP); i++){
            topSet.insert(arr[i].idx);
        }
    };
    pushTop(sortByX);
    pushTop(sortByY);
    pushTop(sortByZ);

    // Build a vector of those "important" phones
    vector<Phone> S;
    S.reserve(topSet.size());
    // We need to quickly find phone data by idx => we'll create a direct map from idx -> phone
    // or we can keep them in an array where array[i] = phone with 1-based index = i+1.
    // But n=200000 => let's do a small array or a vector. We'll do an array of phones in 0-based,
    // but we need to be able to do phones[idx-1].
    // Then we can build S by using that.

    // Let's build a small direct reference array for phone i => phones[i].
    // Actually we already have 'phones'. The phone with .idx = i+1 is at phones[...].
    // We'll do: indexOfPhone[idx] = the position p where phones[p].idx == idx.
    // But building that map would require O(n) with hashing or so. 
    // Alternatively, we can store them in an array 'byIndex', so that byIndex[idx-1] = (x,y,z, idx).
    // Then we can directly pick them. We'll do that.

    // Build an array byIndex so that byIndex[ (phones[p].idx)-1 ] = phones[p].
    vector<Phone> byIndex(n);
    for(int p=0; p<n; p++){
        byIndex[ phones[p].idx - 1 ] = phones[p];
    }

    // Now build S
    for(auto &idx : topSet){
        S.push_back(byIndex[idx - 1]); 
    }

    // Now we have up to ~600 phones in S.
    // For each phone k in 1..n, we compute cost by scanning S.

    long long bestCost = LLONG_MAX;
    int bestIndex = -1;

    for(int k=0; k<n; k++){
        // phone k has (x_k, y_k, z_k) = phones[k]
        long long xk = phones[k].x;
        long long yk = phones[k].y;
        long long zk = phones[k].z;

        long long maxDiff = 0; 
        // We'll check each phone i in S to see how large the sum of positive differences can get.
        for (auto &cand : S){
            // sum of positive diffs:
            long long dx = cand.x - xk;
            if(dx < 0) dx = 0;
            long long dy = cand.y - yk;
            if(dy < 0) dy = 0;
            long long dz = cand.z - zk;
            if(dz < 0) dz = 0;
            long long sumPos = dx + dy + dz;
            if(sumPos > maxDiff){
                maxDiff = sumPos;
            }
        }
        // maxDiff is the opportunity cost for phone k
        if(maxDiff < bestCost){
            bestCost = maxDiff;
            bestIndex = phones[k].idx;  // store the 1-based index
        }
        else if(maxDiff == bestCost){
            // tie => take smaller index
            if(phones[k].idx < bestIndex){
                bestIndex = phones[k].idx;
            }
        }
    }

    cout << bestCost << " " << bestIndex << "\n";
    return 0;
}
