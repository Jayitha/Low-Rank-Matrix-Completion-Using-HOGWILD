//Parallel with synchronization
#include<bits/stdc++.h>
#include<omp.h>
#include<fstream>
#include <cstdlib> 
#include <ctime>
#include<cmath>

using namespace std;


#define r 10 //Rank of Matrix
#define mu 0.001 //Regularization Factor
#define alpha_factor 1 //Multiplication factor for step size after each epoch
#define nr 10000 //Number of rows
#define nc 10000 //Number of columns
#define alpha 0.001 //Step size
#define omega r*(nr + nc - r) //Number of data points given

long double computef(int index1, int index2); // returns LiRj - Zij
long double computef(); // returns \sum f_ij(LiRj)
void computeMinimum(); // returns minimization value


// A hash function used to hash a pair of any kind 
struct hash_pair { 
    template <class T1, class T2> 
    size_t operator()(const pair<T1, T2>& p) const
    { 
        auto hash1 = hash<T1>{}(p.first); 
        auto hash2 = hash<T2>{}(p.second); 
        return hash1 ^ hash2; 
    } 
}; 

float a = alpha; //Learning rate
unordered_map<pair<int, int>, long double, hash_pair> Z; //Stores matrix values
pair<int, int> dummy;
long double L[nr][r], R[nc][r]; //L and R matrices
long double minimum; //to calculate the minimization function
int omegai_[nr], omega_j[nc]; 
int epochs;

long double computef(int index1, int index2)
{
    long double LR = 0;
    for(int i=0; i < r; i++)
        LR += L[index1][i]*R[index2][i];
    pair<int, int> index_pair = make_pair(index1, index2);
    if(Z.find(index_pair) == Z.end())
        return(LR);

    return(LR - Z[index_pair]);
}

long double computef()
{
    long double f = 0;
    for(int i=0; i < nr; i++)
        for(int j=0; j < nc; j++)
            f += pow(computef(i, j), 2);
    return(f);
}

void computeMinimum()
{
    minimum = 0;
    long double L_norm = 0;
    long double R_norm = 0;
    long double f = 0;

    for(int k = 0; k < r; k++)
    {
        for(int i=0; i < nr; i++)
            L_norm += L[i][k]*L[i][k];

        for(int i=0; i < nc; i++)
            R_norm += R[i][k]*R[i][k];
    }
    L_norm = L_norm * mu * 0.5;
    R_norm = R_norm * mu * 0.5;
    f = computef();
    minimum = L_norm + R_norm + f;
}

int main(int argc, char* argv[])
{

    int index1, index2, i, j;
    long double value;
    Z.reserve(omega);
    omp_lock_t L_lock[nr][r], R_lock[nc][r];
 
    for(int k= 0; k < r; k++)
    {
        for(i = 0; i < nr; i++)
            omp_init_lock(&(L_lock[i][k]));
 
        for(i = 0; i < nc; i++)
            omp_init_lock(&(R_lock[i][k]));
    }

    //Reading data from input file
    ifstream fptr;
    fptr.open(argv[1]);
    for(int i=0; i < omega; i++)
    {
        fptr>>index1>>index2>>value;
        dummy = make_pair(index1, index2);
        Z[dummy]= value;
    }
    fptr.close();

    // cout<<"\nFirst 10 elements of Z are: ";
    // auto itr = Z.begin();
    // i = 0;
    // while(i < 10 && itr != Z.end())
    // {
    //     cout<<"\n"<<itr->first.first<<", "<<itr->first.second<<" -> "<<itr->second;
    //     i++;
    //     itr++;
    // }

    epochs = atoi(argv[2]);

    // cout<<"\n\nParameter Values: ";
    // cout<<"\nr: "<<r;
    // cout<<"\nnr: "<<nr;
    // cout<<"\nnc: "<<nc;
    // cout<<"\nOmega: "<<omega;
    // cout<<"\nAlpha: "<<alpha;
    // cout<<"\nAlpha Factor: "<<alpha_factor;
    // cout<<"\nEpochs: "<<epochs;

    //Random Initialization of L and R
    srand(1); 
    for(int k = 0; k < r; k++)
    {
        for(i = 0; i < nr; i++)
            L[i][k] = (rand()%4)-3;

        for(i = 0; i < nc; i++)
            R[i][k] = (rand()%4)-3;
    }

    // cout<<"\n\nFirst 2 rows of L are (Transpose): ";
    // for(i=0; i < r; i++)
    //     cout<<"\n"<<L[0][i]<<" "<<L[1][i];

    // cout<<"\n\nFirst 2 rows of R are (Transpose): ";
    // for(i=0; i < r; i++)
    //     cout<<"\n"<<R[0][i]<<" "<<R[1][i];
    
    //Computing omegai_ and omega_j
    for(i=0; i < nr; i++)
        omegai_[i] = 0;

    for(i=0; i < nc; i++)
        omega_j[i] = 0;

    for(auto itr = Z.begin(); itr != Z.end(); itr++)
    {
        omegai_[(itr->first).first]++;
        omega_j[(itr->first).second]++;
    }

    // cout<<"\n\nFirst 2 values of omegai_: "<<omegai_[0]<<" "<<omegai_[1];
    // cout<<"\nFirst 2 values of omega_j: "<<omega_j[0]<<" "<<omega_j[1];

    //Hogwild!
    for(int epoch_counter = 0; epoch_counter<epochs; epoch_counter++)
    {
        cout<<"\n\nEpoch Number: "<<epoch_counter + 1;
        auto sample_counter = Z.begin();
        #pragma omp parallel
        {
            while(sample_counter!=Z.end())
            {
            
                long double al, ar, b, f;
                int ind1, ind2;
                
                #pragma omp critical
                {
                    if(sample_counter != Z.end())
                    {
                        ind1 = sample_counter->first.first;
                        ind2 = sample_counter->first.second;
                        // cout<<"\nIndex1: "<<index1<<", Index2: "<<index2;
                        sample_counter++;
                    }
                }
                for(int ir = 0; ir < r; ir++)
                {
                    omp_set_lock(&(L_lock[ind1][ir]));
                    omp_set_lock(&(R_lock[ind2][ir]));
                }
                //L = alL - bR
                //R = arR - bL
                f = computef(ind1, ind2);
                //cout<<"\nf: "<<f;
                al = 1 - (mu * a / omegai_[ind1]);
                ar = 1 - (mu * a / omega_j[ind2]);
                b = 2*a*f;
                //cout<<"\n|omegai_|: "<<omegai_[index1]<<", |omega_j|: "<<omega_j[index2];
                //cout<<"\nal: "<<al<<", ar: "<<ar<<", b: "<<b;
                //Update L and R
                for(int ir = 0; ir < r; ir++)
                {
                    float tempL = L[ind1][ir];
                    float tempR = R[ind2][ir];
                    L[ind1][ir] = al*tempL - b*tempR;
                    R[ind2][ir] = ar*tempR - b*tempL;
                }
                for(int ir = 0; ir < r; ir++)
                {
                    omp_unset_lock(&(R_lock[ind2][ir]));
                    omp_unset_lock(&(L_lock[ind1][ir]));
                }
                //computeMinimum();
                //cout<<"\nMinimum: "<<minimum;
            }
            #pragma omp barrier
        }
        //Compute new minimum
        computeMinimum();
        cout<<"\nMinimum: "<<minimum;
        //Updating step size
        a = a * alpha_factor;
    }

    for(int k= 0; k < r; k++)
    {
        for(i = 0; i < nr; i++)
            omp_destroy_lock(&(L_lock[i][k]));

        for(i = 0; i < nc; i++)
            omp_destroy_lock(&(R_lock[i][k]));
    }

    // cout<<"\nWriting final output to file: output.txt";
    
    // ofstream ofptr;
    // ofptr.open("output.txt");
    // for(i = 0; i < nr; i++)
    // {
    //     for(j=0; j < nc; j++)
    //     {
    //         value = 0;
    //         for(int k=0; k < r; k++)
    //             value += L[i][k]*R[j][k];
    //         ofptr<<value<<" ";
    //     }
    //     ofptr<<"\n";
    // }
    // ofptr.close();

    return(0);
}
