//
//  PDE_Simulator.cpp
//  
//
//  Created by Celeste on 10/1/17.
//
//

#include <iostream>
#include <math.h>
#include <vector>
#include <array>
//#include <gsl/gsl_odeiv2.h>
//#include <gsl/gsl_math.h>
//#include "SIR_H.h"


int main(){
    
//within-host parameters
    double mu0 = 0.1841; //pathogen growth rate
    double b0 = 1; //initial pathogen concentration
    double y0 = 1; //initial antibody concentration
    double y1 = pow(10,5);//peak antibody concentration
    double t1 = 28; //duration of infection
    double mu = (1/t1)*log(y1/y0); //antibody growth rate
    double c = b0*((mu-mu0)/(y0*(exp((mu-mu0)*t1)-1))); //pathogen clearance rate
    double r = 1.69; //immunity shape parameter
    double nu = 1.41; //immunity waning rate
    
//between-host parameters
    double K1 = 100; //I1 contact rate
    double K2 = 50; //Ir contact rate
    double alpha1 = 13; //I1 recovery parameter
    double alpha2 = 31; //Ir recovery parameter
    double omega = 0.2; //waning parameter
    double C = 0.0001; //saturation constant
    double delta = 0.02; //birth/death rate
    double totalPop = 101; //total population size
    
//time parameters
    double dt = 0.1; //time step
    double N = 4; //delta t * N = final time of interest (days)
    double M = 4; //delta tau * M = final time since infection (days)
    

    
//vectors for linking functions
    std::vector<double> beta1;
    std::vector<double> beta2;
    std::vector<double> gamma1;
    std::vector<double> gamma2;
    std::vector<double> rho;
    
//vectors for within-host solution
    std::vector<double> pathogen;
    std::vector<double> antibody;
    
    for(double i = 0; i<M;i = i+ dt){
        double b;
        double y;
        if(i<=t1){
            b = b0*exp(mu0*i) - (c*y0*(exp(mu*i)-exp(mu0*i))/(mu-mu0));
            y = y0*exp(mu*i);
        }
        else{
            b = 0;
            y = y1*pow((1+(r-1)*pow(y1,r-1)*nu*(i-t1)),-(1/(r-1)));
        }
        pathogen.push_back(b);
        antibody.push_back(y);
    }

//Construct linking functions
    for(int j =0; j<pathogen.size();j++){
        double beta1Link = (K1*pathogen[j])/(pathogen[j]+C);
        double beta2Link = (K2*pathogen[j])/(pathogen[j]+C);
        double gamma1Link = (alpha1*antibody[j])/(pathogen[j]+C);
        double gamma2Link = (alpha2*antibody[j])/(pathogen[j]+C);
        double rhoLink = omega/(antibody[j]+C);
        
        beta1.push_back(beta1Link);
        beta2.push_back(beta2Link);
        gamma1.push_back(gamma1Link);
        gamma2.push_back(gamma2Link);
        rho.push_back(rhoLink);
    }
//vectors for between-host solution
    std::vector<double> S(N);
    std::vector<double> I1(N*M);
    std::vector<double> R(N*M);
    std::vector<double> Ir(N*M);
    std::vector<double> symptomaticIncidence (N);
//initialize between-host compartments
    S[0] = 100;
    for(int i = 0; i<N;i++){
        I1[i] = 1;
        R[i] = 5;
        Ir[i] = 10;
    }
    symptomaticIncidence[0] = 1;

//Finite Difference Method
    // use backward Euler difference quotient to approximate time derivatives
    // approximate integrals using right end point rule
    
    for(int k=0; k<(N-1); k++){
        double intSum = 0;
        double intSum1 = 0;
        for(int s=0; s<M; s++){
            intSum += dt*(beta1[s]*I1[sqrt(I1.size())*k+s] + beta2[s]*Ir[sqrt(Ir.size())*k+s]);
            intSum1 += dt*(gamma1[s]*I1[sqrt(I1.size())*k+s]+gamma2[s]*Ir[sqrt(Ir.size())*k+s]);
        }
        
        S[k+1] = (S[k] + dt*delta*totalPop)/(1+dt*intSum*(1/totalPop)+dt*delta);
        
        double intSum2 =0;
        for(int j=0;j<M;j++){
            R[(sqrt(R.size())*(k+1))+j] = intSum1;
            R[(sqrt(R.size())*(k+1))+(j+1)] = R[k+j]/(1 + dt*((rho[j+1]/totalPop)*intSum+delta));
            intSum2+= dt*rho[j]*R[(sqrt(R.size())*(k+1))+j];
        }
        double intSum3=0;
        for(int j=0; j<M; j++){
            I1[(sqrt(I1.size())*(k+1))+j] = intSum*S[k+1]/totalPop;
            Ir[(sqrt(Ir.size())*(k+1))+j] = (1/totalPop)*intSum2*intSum;
            
            I1[(sqrt(I1.size())*(k+1))+(j+1)] = I1[(sqrt(I1.size())*k)+j]/(1+dt*(gamma1[j+1]+delta));
            Ir[(sqrt(Ir.size())*(k+1))+(j+1)] = Ir[(sqrt(Ir.size())*k)+j]/(1+dt*(gamma2[j+1]+delta));
            intSum3 += dt*(beta1[j]*I1[(sqrt(I1.size())*k)+j]+beta2[j]*Ir[(sqrt(Ir.size())*k)+j]);
            
        }
        
        symptomaticIncidence[k+1] = S[k+1]*intSum3/totalPop;
        }
    /*std::cout<<"R vec\n";
    std::cout<<"R vec size "<<R.size()<<"\n";
    for(int i =0; i<R.size();i++){
        std::cout<<R[i]<<" ";
    }
    std::cout<<"I1 vec\n";
    std::cout<<"I1 vec size "<<I1.size()<<"\n";
    for(int i =0; i<I1.size();i++){
        std::cout<<I1[i]<<" ";
    }
    std::cout<<"Ir vec\n";
    std::cout<<"Ir vec size "<<Ir.size()<<"\n";
    for(int i =0; i<Ir.size();i++){
        std::cout<<Ir[i]<<" ";
    }
    std::cout<<"S vec\n";
    std::cout<<"S vec size "<<S.size()<<"\n";
    for(int i =0; i<S.size();i++){
        std::cout<<S[i]<<" ";
    }
    std::cout<<"symptomatic incidence\n";
    std::cout<<"symptomatic incdience size "<<symptomaticIncidence.size()<<"\n";
    for(int i =0; i<symptomaticIncidence.size();i++){
        std::cout<<symptomaticIncidence[i]<<" ";
    }*/
    return 0;
}
