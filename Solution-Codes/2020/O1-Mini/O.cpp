#include <bits/stdc++.h>
using namespace std;

typedef long long ll;

// Function to convert latitude to integer by multiplying by 10000 and rounding
int convert_latitude(double a) {
    return round(a * 10000.0);
}

// Function to convert longitude to integer by mapping to [0, 3600000) and multiplying by 10000
int convert_longitude(double b) {
    double adjusted_b = b;
    if (b <= 0.0) {
        adjusted_b += 360.0;
    }
    // Ensure the longitude is within [0, 360)
    if (adjusted_b >= 360.0) adjusted_b -= 360.0;
    return round(adjusted_b * 10000.0);
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    
    int n;
    cin >> n;
    
    // Read map1
    vector<pair<int, int>> map1_points(n);
    for(int i=0; i<n; i++) {
        double a, b;
        cin >> a >> b;
        int lat = convert_latitude(a);
        int lon = convert_longitude(b);
        map1_points[i] = {lat, lon};
    }
    
    // Read map2
    vector<pair<int, int>> map2_points(n);
    for(int i=0; i<n; i++) {
        double a, b;
        cin >> a >> b;
        int lat = convert_latitude(a);
        int lon = convert_longitude(b);
        map2_points[i] = {lat, lon};
    }
    
    // Sort both maps by latitude, then longitude
    sort(map1_points.begin(), map1_points.end(), [&](const pair<int,int> &a, const pair<int,int> &b) -> bool {
        if(a.first != b.first) return a.first < b.first;
        return a.second < b.second;
    });
    
    sort(map2_points.begin(), map2_points.end(), [&](const pair<int,int> &a, const pair<int,int> &b) -> bool {
        if(a.first != b.first) return a.first < b.first;
        return a.second < b.second;
    });
    
    int i =0, j=0;
    int delta = -1;
    bool same = true;
    
    while(i <n && j <n){
        int lat1 = map1_points[i].first;
        int lat2 = map2_points[j].first;
        
        if(lat1 != lat2){
            same = false;
            break;
        }
        
        // Collect all longitudes for this latitude in map1
        int i_start = i;
        while(i <n && map1_points[i].first == lat1) i++;
        int i_end = i;
        int count1 = i_end - i_start;
        
        // Collect all longitudes for this latitude in map2
        int j_start = j;
        while(j <n && map2_points[j].first == lat2) j++;
        int j_end = j;
        int count2 = j_end - j_start;
        
        if(count1 != count2){
            same = false;
            break;
        }
        
        if(count1 ==0){
            continue;
        }
        
        // Compute delta_candidate from first pair
        int x = map1_points[i_start].second;
        int y = map2_points[j_start].second;
        int delta_candidate = (y - x) % 3600000;
        if(delta_candidate <0) delta_candidate += 3600000;
        
        // Verify all pairs have the same delta
        bool valid = true;
        for(int k=0; k<count1; k++){
            int expected = (map1_points[i_start +k].second + delta_candidate) % 3600000;
            if(expected != map2_points[j_start +k].second){
                valid = false;
                break;
            }
        }
        
        if(!valid){
            same = false;
            break;
        }
        
        if(delta == -1){
            delta = delta_candidate;
        }
        else{
            if(delta != delta_candidate){
                same = false;
                break;
            }
        }
    }
    
    // After loop, check if both maps are fully processed
    if(i !=n || j !=n){
        same = false;
    }
    
    if(same){
        cout << "Same\n";
    }
    else{
        cout << "Different\n";
    }
}
