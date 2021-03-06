//
//  Polio_boxcar_simulator.cpp
//  
//
//  Created by Celeste on 12/10/17.
//
//

#include <stdio.h>
#include "Polio_boxcar_model_extended_waning.h"
#include "DIFFEQ_SIM.h"
#include <math.h>
#include "Polio_boxcar_parameters.h"
#include <vector>

void usage(){
    cout<<"\n\t Usage: Input model initialization <S> <I1> <I2> <I3> <sum of all R compartments>\n";
    exit(-1);
}

int main(int argc, char** argv){
    if(argc!= 6) usage();
    
    vector<double> initialValues(argc-1);
    for(int i = 1; i<argc; i++){
        initialValues[i-1] = atof(argv[i]);
    }
    
    //Determine peak antibody level for each infected compartment
    setPeakAnt();

    //Determine infectious period for each infected compartment
    infectPeriod();
    
    //set number of compartments
    int nbins = 13; //total compartments
    int sbins = 1;
    int ibins = 3;
    int rbins = nbins - (sbins + ibins);
    
    //vectors for parameters
    vector<double> beta(sbins + rbins-1);//1 recovered compartment has complete immunity
    vector<double> gamma(ibins);
    vector<double> recovery(rbins);
    
    for(unsigned int i = 0; i < beta.size(); i++){
        //beta[i] = numContacts*probInfection(pow(10,i));
        beta[i] = numContacts*((i+1)/2.0);
    }
    for(unsigned int i = 0; i < gamma.size(); i++){
        //gamma[i] = 1.0/infectPeriod()[i];
        gamma[i] = 13.0*(i+1);
    }
    for(unsigned int i = 0; i < recovery.size(); i++){
        recovery[i] = .03;
    }
    
    
    //Constuct model and parameters
    Polio_boxcar_model model(birthRate,beta,gamma,recovery,nbins);
    
    
    //Initialize model using user inputs
    model.initialize(initialValues, rbins);
    
    //Run model
    model.run_simulation();
    
    //Output simulation results
    model.printX();
    
    
    return 0;
}
