#include <bits/stdc++.h>
using namespace std;

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    int m, n, t, q;
    cin >> m >> n >> t >> q;
    // Read p_i
    vector<double> p_row(m, 0.0);
    for(int i=0;i<m;i++) cin >> p_row[i];
    // Read q_j
    vector<double> q_col(n, 0.0);
    for(int j=0;j<n;j++) cin >> q_col[j];
    // Precompute p_ij and a_j
    // a_j = p_ij / (1 - p_ij)
    // Store a_j per row
    vector<vector<double>> a(m, vector<double>(n, 0.0));
    for(int i=0;i<m;i++){
        for(int j=0;j<n;j++){
            double p_ij = p_row[i] + q_col[j];
            a[i][j] = p_ij / (1.0 - p_ij);
        }
    }
    // Precompute p_ij for all cells
    // To access quickly, store in a 1D array
    // cell_id = i * n + j
    // Not necessary, use per row access
    while(q--){
        int s;
        cin >> s;
        // Read s cells
        // Use a vector of vectors to store excluded columns per row
        vector<vector<int>> excluded(m, vector<int>());
        vector<pair<int, int>> S_cells(s, {0,0});
        for(int i=0;i<s;i++){
            cin >> S_cells[i].first >> S_cells[i].second;
            S_cells[i].first--; // 0-based
            S_cells[i].second--; // 0-based
            excluded[S_cells[i].first].push_back(S_cells[i].second);
        }
        // Compute C_S(k)
        // Initialize C_S with k=0
        // Using a vector of size s+1
        vector<double> C_S(s+1, 0.0);
        C_S[0] = 1.0;
        for(int idx=0; idx<s; idx++){
            int i = S_cells[idx].first;
            int j = S_cells[idx].second;
            double p_ij = p_row[i] + q_col[j];
            // Update C_S from high to low
            for(int k = min(idx, s-1); k >=0; k--){
                C_S[k+1] += C_S[k] * p_ij;
                C_S[k] *= (1.0 - p_ij);
            }
        }
        // Compute G_T(x)
        // Initialize G_T[k] =1.0 for k=0
        vector<double> G_T(1, 1.0);
        // Iterate over each row
        for(int i=0;i<m;i++){
            // Collect a_j not in S for this row
            vector<double> G_i = {1.0};
            // Sort excluded[j] for faster lookup
            vector<int> &exc = excluded[i];
            sort(exc.begin(), exc.end());
            for(int j=0; j<n; j++){
                if(!binary_search(exc.begin(), exc.end(), j)){
                    double a_j = a[i][j];
                    int G_i_size = G_i.size();
                    // Update G_i by multiplying (1 + a_j x)
                    G_i.emplace_back(0.0); // Increase size by 1
                    for(int k = G_i_size -1; k >=0; k--){
                        G_i[k+1] += G_i[k] * a_j;
                    }
                }
            }
            // Now multiply G_T by G_i, keeping up to degree t
            int G_T_size = G_T.size();
            int G_i_size = G_i.size();
            int new_size = min((int)(G_T_size + G_i_size -1), t +1);
            vector<double> temp(min((int)(t +1), G_T_size + G_i_size -1), 0.0);
            for(int k1=0; k1 < G_T_size && k1 <= t; k1++){
                for(int k2=0; k2 < G_i_size && (k1 + k2) <= t; k2++){
                    temp[k1 + k2] += G_T[k1] * G_i[k2];
                }
            }
            G_T = move(temp);
        }
        // Now, compute [x^t] G(x) = sum_{k=0}^s C_S(k) * G_T(t -k)
        double G_t = 0.0;
        for(int k=0; k<=s; k++){
            if(t - k >=0 && t -k < G_T.size()){
                G_t += C_S[k] * G_T[t -k];
            }
        }
        // Now, compute P(k | t) for k=0 to s
        vector<double> P(s+1, 0.0);
        for(int k=0; k<=s; k++){
            if(t -k >=0 && t -k < G_T.size()){
                P[k] = (C_S[k] * G_T[t -k]) / G_t;
            }
            else{
                P[k] =0.0;
            }
        }
        // Output P(0) to P(s) with 9 decimal places
        string output = "";
        for(int k=0; k<=s; k++){
            if(k >0) output += " ";
            // To ensure the absolute error is within 1e-6, output 9 decimal places
            string num = to_string(P[k]);
            // Trim trailing zeros and possibly the decimal point
            size_t dot = num.find('.');
            if(dot != string::npos){
                size_t last = num.size() -1;
                while(last > dot && num[last] == '0') last--;
                if(last == dot){
                    num.erase(last);
                }
                else{
                    num.erase(last +1);
                }
            }
            // Ensure at least one decimal digit
            if(num.find('.') == string::npos){
                num += ".0";
            }
            output += num;
        }
        cout << output << "\n";
    }
}
