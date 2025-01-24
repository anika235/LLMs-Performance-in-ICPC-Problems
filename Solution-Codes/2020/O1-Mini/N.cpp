#include <bits/stdc++.h>
using namespace std;

const double EPS = 1e-9;

// Function to perform Gaussian Elimination
bool gaussian_elimination(vector<vector<double>> &mat, int d, vector<double> &solution) {
    int n = mat.size();
    int m = d;
    for(int col=0; col<m && col < n; col++) {
        // Find pivot
        int sel = col;
        for(int i=col; i<n; i++) {
            if(abs(mat[i][col]) > abs(mat[sel][col])) sel = i;
        }
        if(abs(mat[sel][col]) < EPS) continue; // No pivot in this column

        // Swap rows
        swap(mat[col], mat[sel]);

        // Normalize pivot row
        double pivot = mat[col][col];
        for(int j=col; j<=m; j++) {
            mat[col][j] /= pivot;
        }

        // Eliminate other rows
        for(int i=0; i<n; i++) {
            if(i != col && abs(mat[i][col]) > EPS) {
                double factor = mat[i][col];
                for(int j=col; j<=m; j++) {
                    mat[i][j] -= factor * mat[col][j];
                }
            }
        }
    }

    // Check for inconsistency
    for(int i=0; i<n; i++) {
        bool all_zero = true;
        for(int j=0; j<m; j++) {
            if(abs(mat[i][j]) > EPS) {
                all_zero = false;
                break;
            }
        }
        if(all_zero && abs(mat[i][m]) > EPS) return false; // No solution
    }

    // Initialize solution with zeros
    solution.assign(m, 0.0);

    // Back substitution
    for(int i=0; i<min(n, m); i++) {
        // Find the first non-zero coefficient
        int first = -1;
        for(int j=0; j<m; j++) {
            if(abs(mat[i][j]) > EPS) {
                first = j;
                break;
            }
        }
        if(first == -1) continue; // Free variable
        solution[first] = mat[i][m];
        for(int j=first+1; j<m; j++) {
            solution[first] -= mat[i][j] * solution[j];
        }
    }

    return true;
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    int d, n;
    cin >> d >> n;
    // Read training results
    struct Training {
        vector<double> x;
        double e;
    };
    vector<Training> trainings(n, Training{vector<double>(d,0.0),0.0});
    for(int i=0; i<n; i++) {
        for(int j=0; j<d; j++) cin >> trainings[i].x[j];
        cin >> trainings[i].e;
    }
    // Choose the first training result as reference
    vector<double> x1 = trainings[0].x;
    double e1 = trainings[0].e;
    // Build the augmented matrix
    // Each row corresponds to a training result from 2 to n
    // Each equation: sum_k (xik -x1k) yk = 0.5*(sum_k (xik^2 -x1k^2) + e_i^2 -e1^2 )
    int equations = n-1;
    vector<vector<double>> mat(equations, vector<double>(d+1, 0.0));
    for(int i=1; i<n; i++) {
        // Compute A[i-1][k] = xik -x1k
        for(int k=0; k<d; k++) {
            mat[i-1][k] = trainings[i].x[k] - x1[k];
        }
        // Compute b[i-1] =0.5*(sum_k (xik^2 -x1k^2) + e_i^2 -e1^2 )
        double sum_diff_sq =0.0;
        for(int k=0; k<d; k++) {
            sum_diff_sq += (trainings[i].x[k]*trainings[i].x[k] - x1[k]*x1[k]);
        }
        sum_diff_sq += (trainings[i].e * trainings[i].e - e1 * e1);
        mat[i-1][d] = 0.5 * sum_diff_sq;
    }
    // Solve the linear system
    vector<double> y;
    bool has_solution = gaussian_elimination(mat, d, y);
    if(!has_solution){
        // According to problem statement, a solution exists
        // But to be safe, output any point on the sphere
        // For example, set y =x1 shifted by e1 in first dimension
        y.assign(d, 0.0);
        if(d >=1) y[0] = x1[0] + e1;
        // Output y
    }
    // Now, verify || y -x1 || =e1 within precision
    // Compute || y -x1 ||
    double norm =0.0;
    for(int k=0; k<d; k++) {
        double diff = y[k] -x1[k];
        norm += diff * diff;
    }
    norm = sqrt(norm);
    double diff_norm = abs(norm - e1);
    if(diff_norm > 1e-5 * max(1.0, e1)){
        // Adjust y to lie on the sphere
        // Project y onto the sphere
        double scale = e1 / norm;
        for(int k=0; k<d; k++) y[k] = x1[k] + (y[k] -x1[k]) * scale;
    }
    // Output y with sufficient precision
    cout << fixed << setprecision(12);
    for(int k=0; k<d; k++) {
        if(k >0) cout << ' ';
        // To handle negative zero, use 0.0
        if(abs(y[k]) < 1e-9) cout << "0";
        else cout << y[k];
    }
    cout << "\n";
}
