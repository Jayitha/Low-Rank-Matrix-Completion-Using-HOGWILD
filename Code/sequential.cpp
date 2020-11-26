//Sequential
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

float computef(int index1, int index2); // returns LiRj - Zij
float computef(); // returns \sum f_ij(LiRj)
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
unordered_map<pair<int, int>, float, hash_pair> Z; //Stores matrix values
pair<int, int> dummy;
double L[nr][r], R[nc][r]; //L and R matrices
long double minimum; //to calculate the minimization function
int omegai_[nr], omega_j[nc]; 
int epochs;

float computef(int index1, int index2)
{
    float LR = 0;
    for(int i=0; i < r; i++)
        LR += L[index1][i]*R[index2][i];
    pair<int, int> index_pair = make_pair(index1, index2);
    if(Z.find(index_pair) == Z.end())
        return(LR);

    return(LR - Z[index_pair]);
}

float computef()
{
    float f = 0;
    for(int i=0; i < nr; i++)
        for(int j=0; j < nc; j++)
            f += pow(computef(i, j), 2);
    return(f);
}

void computeMinimum()
{
    minimum = 0;
    float L_norm = 0;
    float R_norm = 0;
    float f = 0;

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
    float value;
    double al, ar, b, f;
    Z.reserve(omega);

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
    //     cout<<"\n"<<L[0][i]<<" "<<L[1][0\i];

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
        while(sample_counter!=Z.end())
        {
            index1 = sample_counter->first.first;
            index2 = sample_counter->first.second;
            //cout<<"\nIndex1: "<<index1<<", Index2: "<<index2;
            sample_counter++;

            //L = alL - bR
            //R = arR - bL
            f = computef(index1, index2);
            //cout<<"\nf: "<<f;
            al = 1 - (mu * a / omegai_[index1]);
            ar = 1 - (mu * a / omega_j[index2]);
            b = 2*a*f;
            //cout<<"\n|omegai_|: "<<omegai_[index1]<<", |omega_j|: "<<omega_j[index2];
            //cout<<"\nal: "<<al<<", ar: "<<ar<<", b: "<<b;
            //Update L and R
            for(i = 0; i < r; i++)
            {
                float tempL = L[index1][i];
                L[index1][i] = al*L[index1][i] - b*R[index2][i];
                R[index2][i] = ar*R[index2][i] - b*tempL;
            }
            //computeMinimum();
            //cout<<"\nMinimum: "<<minimum;
        }
        //Compute new minimum
        computeMinimum();
        cout<<"\nMinimum: "<<minimum;
        //Updating step size
        a = a * alpha_factor;
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
