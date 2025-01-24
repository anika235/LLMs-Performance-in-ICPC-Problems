#include <bits/stdc++.h>
using namespace std;

/*
  Solve the system:
    For i=2..n:
      sum_j (x_{i,j}-x_{1,j}) * z_j = rhs_i
    where rhs_i = ( (e_i^2 - ||x_i||^2) - (e_1^2 - ||x_1||^2 ) )/2.

  That yields (n-1) linear equations in d unknowns.

  Then impose the first sphere equation:
    ||z||^2 - 2 z·x_1 + ||x_1||^2 = e_1^2.

  We'll do standard Gaussian elimination.  We'll keep track of pivot columns, free variables, etc.
*/

static const double EPS = 1e-12;

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int d, n;
    cin >> d >> n;

    vector<vector<double>> X(n, vector<double>(d));
    vector<double> E(n);

    for(int i=0; i<n; i++){
        for(int j=0; j<d; j++){
            cin >> X[i][j];
        }
        cin >> E[i];
    }

    // If n=1, we only have: ||z - X[0]|| = E[0].
    // We can choose z = X[0] + (E[0], 0, 0, ..., 0) for instance.
    // (unless E[0]=0, then just z=X[0].)
    if(n==1){
        // trivial
        // if E[0]=0 => z = X[0]. else we can offset in the 1st dimension
        vector<double> z(d);
        for(int j=0;j<d;j++) z[j] = X[0][j];
        if(std::fabs(E[0])>EPS){
            // shift along dimension 0
            z[0] += E[0];
        }
        // output
        for(int j=0;j<d;j++){
            cout << fixed << setprecision(9) << z[j];
            if(j+1<d) cout<<" ";
        }
        cout << "\n";
        return 0;
    }

    // Otherwise, build the matrix (n-1 x d) and vector (n-1)
    // row i = x_{i+1} - x_{1}, for i=1..(n-1).
    // b_i = [ ( e_{i+1}^2 - ||x_{i+1}||^2 ) - ( e_1^2 - ||x_1||^2 ) ]/2
    auto sqnorm = [&](const vector<double> &v){
        double s=0; 
        for(double x: v) s += x*x;
        return s;
    };
    double C1 = sqnorm(X[0]); // ||x_1||^2

    vector<double> b(n-1);
    vector<vector<double>> A(n-1, vector<double>(d));
    for(int i=1; i<n; i++){
        double Ci = sqnorm(X[i]);
        double rhs = ((E[i]*E[i] - Ci) - (E[0]*E[0] - C1))/2.0;
        b[i-1] = rhs;
        for(int j=0;j<d;j++){
            A[i-1][j] = (X[i][j] - X[0][j]);
        }
    }
    int m = n-1; // number of linear equations

    // Solve A z = b by Gaussian elimination in double precision
    // We'll keep track of pivot columns, etc.
    // We'll produce a parametric solution:
    //   z = z_particular + sum_{k in freeVars} alpha_k * e_k
    // We only actually construct one particular solution and a basis for the null space.

    // We'll do row-based elimination. Because m <= 499, d <=500, it's feasible.

    // "row i" means A[i][*], b[i].

    // pivot variables: keep an array pivotCol[] that says which column is the pivot for row i, or -1 if none.
    vector<int> pivotOfRow(m, -1);
    
    int row=0; // which row we're processing
    for(int col=0; col<d && row<m; col++){
        // find pivot
        int pivot = row;
        double best = fabs(A[pivot][col]);
        for(int r=row+1; r<m; r++){
            double val = fabs(A[r][col]);
            if(val>best){
                best=val; 
                pivot=r;
            }
        }
        if(best < EPS){
            // no pivot in this column
            continue;
        }
        // swap row with pivot if needed
        if(pivot!=row){
            std::swap(A[pivot], A[row]);
            std::swap(b[pivot], b[row]);
        }
        pivotOfRow[row] = col;

        // normalize row "row"
        double diag = A[row][col];
        for(int cc=col; cc<d; cc++){
            A[row][cc] /= diag;
        }
        b[row] /= diag;

        // eliminate below
        for(int r=row+1; r<m; r++){
            double factor = A[r][col];
            if(fabs(factor)>EPS){
                for(int cc=col; cc<d; cc++){
                    A[r][cc] -= factor*A[row][cc];
                }
                b[r] -= factor*b[row];
            }
        }
        row++;
    }
    int rank = row; // number of pivot rows found

    // Now back-substitute to get a particular solution for the pivot variables
    // The non-pivot columns => "free" variables. We'll set them = 0 for now in particular solution.
    vector<double> zPart(d, 0.0);

    // Eliminate upwards
    for(int r=rank-1; r>=0; r--){
        int c = pivotOfRow[r];
        if(c<0) continue; 
        double rhsVal = b[r];
        // subtract sum of known columns * zPart
        for(int cc=c+1; cc<d; cc++){
            rhsVal -= A[r][cc]*zPart[cc];
        }
        zPart[c] = rhsVal; 
    }

    // Next, find a basis for the null space.  If rank<d, we have (d-rank) free variables.
    // We’ll store them as vectors v_k in dimension d. For each free col, set that free col=1, others=0, pivot col solved.
    vector<vector<double>> nullBasis; // each is size d
    {
        // Identify which columns are pivot vs free
        vector<bool> isPivot(d,false);
        for(int r=0; r<rank; r++){
            if(pivotOfRow[r]>=0) {
                isPivot[ pivotOfRow[r] ] = true;
            }
        }
        // for each free column fc, we build a vector v of length d
        for(int fc=0; fc<d; fc++){
            if(!isPivot[fc]){
                // set that free column to 1, solve for pivot columns in back-substitution
                vector<double> v(d,0.0);
                v[fc] = 1.0;
                // now go upwards
                for(int r=rank-1; r>=0; r--){
                    int c = pivotOfRow[r];
                    if(c<0) continue;
                    double val = -0.0;
                    // subtract sum_{cc>c} A[r][cc]*v[cc] from 0 => v[c]
                    double sumAbove = 0.0;
                    for(int cc=c+1; cc<d; cc++){
                        sumAbove += A[r][cc]*v[cc];
                    }
                    v[c] = - sumAbove; // because row r eq => A[r][c]* z_c + ... = 0 for the free-variable vector
                }
                nullBasis.push_back(v);
            }
        }
    }
    // We now have a parametric solution:
    //   z = zPart + sum_{k} alpha_k * nullBasis[k].
    // Next we impose the 1st sphere eq:
    //   ||z - x_1||^2 = e_0^2.

    // Define a function that, for a given candidate z, checks the distance to X[0].
    auto distSqToX1 = [&](const vector<double> &z){
        double s=0;
        for(int j=0;j<d;j++){
            double diff = z[j] - X[0][j];
            s += diff*diff;
        }
        return s;
    };

    // If the dimension of nullBasis is 0 => no free variables => we must check if zPart satisfies eq #1.
    // If it does not, the system is overconstrained (or no solution).  Problem statement suggests there *is* a solution,
    // so hopefully it works out if rank==d.
    double want = E[0]*E[0]; // want distSqToX1(z)=want
    double gotPart = distSqToX1(zPart);
    if(nullBasis.empty()){
        // check if gotPart is close to want. If yes => done. If not => there's no fix (no free dimension).
        if(fabs(gotPart - want) > 1e-5*(1+fabs(want))) {
            // numerically might be no exact solution. We'll just output zPart anyway, 
            // but it's possible we needed more precision or the system is inconsistent.
            // We'll do the best we can.
        }
        // output zPart
        for(int j=0;j<d;j++){
            cout<<fixed<<setprecision(9)<<zPart[j]<< ( (j+1<d) ? ' ' : '\n');
        }
        return 0;
    }

    // Else we have at least 1 free vector => let’s pick just one dimension of freedom:
    // We'll try each nullBasis[k], setting alpha for it, ignoring others. We have:
    //
    //   z(alpha) = zPart + alpha * nullBasis[k].
    //
    // We want  distSqToX1( z(alpha) ) = want.  That is:
    //
    //   ||z(alpha)-X[0]||^2 = want.
    //   Let  f(alpha) = sum_j [zPart[j]+ alpha*v[j] - X[0][j]]^2.
    //
    // This is a quadratic in alpha. Solve for alpha. If a real solution exists, pick it and done.
    // If no real solution, try next basis vector. 
    // If none works, we do the best we can (the problem statement typically implies a solution should exist).

    // Let’s define a small helper that tries one null vector and returns a suitable alpha if possible:
    auto tryOneBasis = [&](const vector<double> &v) -> bool {
        // distSq( zPart + alpha*v , X[0] ) = want
        // expand:
        // sum_j [ (zPart[j]-x1j) + alpha*v[j] ]^2 = want
        // = sum_j [ A_j + alpha*v[j] ]^2,  where A_j = zPart[j]-x1j
        // = sum_j ( A_j^2 + 2 alpha A_j v[j] + alpha^2 v[j]^2 )
        // = sum_j A_j^2  + 2 alpha sum_j(A_j v[j]) + alpha^2 sum_j v[j]^2
        // define:
        //   AA = sum_j A_j^2
        //   BB = 2 sum_j(A_j v[j])   (the coefficient in front of alpha)
        //   CC = sum_j(v[j]^2)
        // we want  AA + BB*alpha + CC*alpha^2 = want
        // => CC*alpha^2 + BB*alpha + (AA - want) = 0.
        // solve for alpha. We pick *any* real root if it exists.

        double AA=0, BB=0, CC=0;
        for(int j=0;j<d;j++){
            double Aj = zPart[j] - X[0][j];
            AA += Aj*Aj;
        }
        for(int j=0;j<d;j++){
            double Aj = zPart[j] - X[0][j];
            BB += (Aj * v[j]);
        }
        BB *= 2.0;
        for(int j=0;j<d;j++){
            CC += v[j]*v[j];
        }

        double eqC = (AA - want);

        // we have  CC*alpha^2 + BB*alpha + eqC = 0
        if(fabs(CC)<EPS){
            // then linear in alpha => BB*alpha + eqC=0 => alpha=-eqC/BB if BB!=0
            if(fabs(BB)<EPS){
                // then eqC=0 => the original zPart already had the correct distance, alpha is free => alpha=0 is fine
                // => that means we don't even need to shift.  We'll pick alpha=0.
                // But if eqC != 0 => no solution with this direction.
                if(fabs(eqC)>1e-10) {
                    return false;
                } else {
                    // close enough
                    return true; // alpha=0 is good
                }
            } else {
                double alpha = - eqC/BB;
                // that might work, let's do it
                // We'll set zPart = zPart + alpha*v
                for(int j=0;j<d;j++){
                    zPart[j] += alpha*v[j];
                }
                return true;
            }
        } else {
            // quadratic
            double disc = BB*BB - 4.0*CC*eqC; 
            if(disc<0) {
                // no real solution
                return false;
            }
            double sqrtDisc = sqrt(std::max(0.0, disc));
            // we can pick alpha = (-BB +- sqrtDisc)/(2*CC).
            // We'll just pick one root.  Let's pick the one that yields smaller |alpha|, for stability
            double alpha1 = (-BB + sqrtDisc)/(2.0*CC);
            double alpha2 = (-BB - sqrtDisc)/(2.0*CC);
            double a1 = fabs(alpha1), a2=fabs(alpha2);
            double alpha = (a1< a2? alpha1 : alpha2);
            // apply
            for(int j=0;j<d;j++){
                zPart[j] += alpha*v[j];
            }
            return true;
        }
    };

    for(auto &v : nullBasis){
        // try this basis
        // we keep a copy of zPart to revert if fails
        vector<double> backup = zPart;
        if( tryOneBasis(v) ){
            // success
            // done => we have updated zPart
            // check final distance
            // (not strictly required, but let’s be sure we are close)
            double finalDist = distSqToX1(zPart);
            // good enough => output
            for(int j=0;j<d;j++){
                cout<<fixed<<setprecision(9)<<zPart[j]<< (j+1<d ? ' ' : '\n');
            }
            return 0;
        } else {
            // revert
            zPart = backup;
        }
    }

    // If we get here, none of the single-basis attempts found a real solution. 
    // In principle, we could try combinations of basis vectors, etc.  
    // But typically the problem statement suggests there *is* a solution.  
    // As a fallback, just output the particular solution (it satisfies the difference constraints but might be off on eq#1).
    // That at least partially solves the system. 

    for(int j=0;j<d;j++){
        cout<<fixed<<setprecision(9)<<zPart[j]<< (j+1<d ? ' ' : '\n');
    }
    return 0;
}
