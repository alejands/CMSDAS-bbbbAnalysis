#ifndef BBBB_FUNCTIONS_H
#define BBBB_FUNCTIONS_H

#include <cmath>
#include <utility>
#include <iostream>
#include "analysis_utils.h"
#include "TMath.h"
#include "TSpline.h"

class TriggerEfficiencyCalculator
{
  public:
    TriggerEfficiencyCalculator(std::string triggerEfficiencyFileName)
    {
        TFile triggerEfficiencyFile(triggerEfficiencyFileName.c_str());
      
        fSplineDataEfficiency_Double90Quad30_QuadCentralJet30   = getSplineFromFile(triggerEfficiencyFile, "SingleMuon_Double90Quad30_Spline_QuadCentralJet30"  );
        fSplineDataEfficiency_Double90Quad30_DoubleCentralJet90 = getSplineFromFile(triggerEfficiencyFile, "SingleMuon_Double90Quad30_Spline_DoubleCentralJet90");
        fSplineDataEfficiency_Quad45_QuadCentralJet45           = getSplineFromFile(triggerEfficiencyFile, "SingleMuon_Quad45_Spline_QuadCentralJet45"          );
        fSplineDataEfficiency_And_QuadCentralJet45              = getSplineFromFile(triggerEfficiencyFile, "SingleMuon_And_Spline_QuadCentralJet45"             );

        fSplineMcEfficiency_Double90Quad30_QuadCentralJet30     = getSplineFromFile(triggerEfficiencyFile, "TTbar_Double90Quad30_Spline_QuadCentralJet30"       );
        fSplineMcEfficiency_Double90Quad30_DoubleCentralJet90   = getSplineFromFile(triggerEfficiencyFile, "TTbar_Double90Quad30_Spline_DoubleCentralJet90"     );
        fSplineMcEfficiency_Quad45_QuadCentralJet45             = getSplineFromFile(triggerEfficiencyFile, "TTbar_Quad45_Spline_QuadCentralJet45"               );
        fSplineMcEfficiency_And_QuadCentralJet45                = getSplineFromFile(triggerEfficiencyFile, "TTbar_And_Spline_QuadCentralJet45"                  );
    }

    float getDataEfficiency_Double90Quad30_QuadCentralJet30  (float fourthLeadingJet) {return getEfficiency(fSplineDataEfficiency_Double90Quad30_QuadCentralJet30   ,fourthLeadingJet);}
    float getDataEfficiency_Double90Quad30_DoubleCentralJet90(float secondLeadingJet) {return getEfficiency(fSplineDataEfficiency_Double90Quad30_DoubleCentralJet90 ,secondLeadingJet);}
    float getDataEfficiency_Quad45_QuadCentralJet45          (float fourthLeadingJet) {return getEfficiency(fSplineDataEfficiency_Quad45_QuadCentralJet45           ,fourthLeadingJet);}
    float getDataEfficiency_And_QuadCentralJet45             (float fourthLeadingJet) {return getEfficiency(fSplineDataEfficiency_And_QuadCentralJet45              ,fourthLeadingJet);}
    
    float getMcEfficiency_Double90Quad30_QuadCentralJet30    (float fourthLeadingJet) {return getEfficiency(fSplineMcEfficiency_Double90Quad30_QuadCentralJet30     ,fourthLeadingJet);}
    float getMcEfficiency_Double90Quad30_DoubleCentralJet90  (float secondLeadingJet) {return getEfficiency(fSplineMcEfficiency_Double90Quad30_DoubleCentralJet90   ,secondLeadingJet);}
    float getMcEfficiency_Quad45_QuadCentralJet45            (float fourthLeadingJet) {return getEfficiency(fSplineMcEfficiency_Quad45_QuadCentralJet45             ,fourthLeadingJet);}
    float getMcEfficiency_And_QuadCentralJet45               (float fourthLeadingJet) {return getEfficiency(fSplineMcEfficiency_And_QuadCentralJet45                ,fourthLeadingJet);}


  private:
    std::unique_ptr<TSpline> getSplineFromFile(TFile& inputFile, std::string splineName)
    {
        auto* spline = static_cast<TSpline*>(inputFile.Get(splineName.c_str()));
        if(spline == nullptr) throw std::runtime_error("Spline " + splineName + " does not exist in file " + inputFile.GetName());

        return std::unique_ptr<TSpline>(spline);
    }

    float getEfficiency(const std::unique_ptr<TSpline>& theSpline, float variableValue)
    {
        if     (variableValue > theSpline->GetXmax()) variableValue = theSpline->GetXmax();
        else if(variableValue < theSpline->GetXmin()) variableValue = theSpline->GetXmin();
        return theSpline->Eval(variableValue);
    }

    std::unique_ptr<TSpline> fSplineDataEfficiency_Double90Quad30_QuadCentralJet30  ;
    std::unique_ptr<TSpline> fSplineDataEfficiency_Double90Quad30_DoubleCentralJet90;
    std::unique_ptr<TSpline> fSplineDataEfficiency_Quad45_QuadCentralJet45          ;
    std::unique_ptr<TSpline> fSplineDataEfficiency_And_QuadCentralJet45             ;

    std::unique_ptr<TSpline> fSplineMcEfficiency_Double90Quad30_QuadCentralJet30    ;
    std::unique_ptr<TSpline> fSplineMcEfficiency_Double90Quad30_DoubleCentralJet90  ;
    std::unique_ptr<TSpline> fSplineMcEfficiency_Quad45_QuadCentralJet45            ;
    std::unique_ptr<TSpline> fSplineMcEfficiency_And_QuadCentralJet45               ;

};

std::vector<jet_t> bbbb_jets_idxs_BothClosestToDiagonal(const std::vector<jet_t> *presel_jets)
{

    //Do you add b-jet regression into the pairing?
    // bool breg = any_cast<bool>(parameterList_->at("BjetRegression"));

    bool breg = true;
    // float targetHiggsMass1 = any_cast<float>(parameterList_->at("LeadingHiggsMass"));
    // float targetHiggsMass2 = any_cast<float>(parameterList_->at("SubleadingHiggsMass"));
    //float targetHiggsMass1 = 120;
    //float targetHiggsMass2 = 110;
    float targetHiggsMass1 = 125;
    float targetHiggsMass2 = 120;

    std::vector<TLorentzVector> p4s;

    for (uint i = 0; i < presel_jets->size(); ++i)
        for (uint j = i+1; j < presel_jets->size(); ++j)
        {
            TLorentzVector p4sum;

            if (breg)
               p4sum= (presel_jets->at(i).p4_breg + presel_jets->at(j).p4_breg);
            else
               p4sum= (presel_jets->at(i).p4 + presel_jets->at(j).p4);
            p4s.emplace_back(p4sum);
        }

    float m1,m2,m3,m4,m5,m6;

    if(p4s.at(0).Pt() > p4s.at(5).Pt())
    {
      m1 = p4s.at(0).M(); m2 = p4s.at(5).M();
    }
    else
    {
      m1 = p4s.at(5).M(); m2 = p4s.at(0).M();        
    }

    if(p4s.at(1).Pt() > p4s.at(4).Pt())
    {
      m3 = p4s.at(1).M(); m4 = p4s.at(4).M();
    }
    else
    {
      m3 = p4s.at(4).M(); m4 = p4s.at(1).M();       
    }

    if(p4s.at(2).Pt() > p4s.at(3).Pt())
    {
      m5 = p4s.at(2).M(); m6 = p4s.at(3).M();
    }
    else
    {
      m5 = p4s.at(3).M(); m6 = p4s.at(2).M();       
    }

    std::pair<float, float> m_12_34 = std::make_pair(m1,m2);
    std::pair<float, float> m_13_24 = std::make_pair(m3,m4);
    std::pair<float, float> m_14_23 = std::make_pair(m5,m6);

    // Diagonal
    //float d12_34 = TMath::Sqrt(pow(m_12_34.first,2) + pow(m_12_34.second,2) )*fabs( TMath::Sin( TMath::ATan(m_12_34.second/m_12_34.first) - TMath::ATan(targetHiggsMass2/targetHiggsMass1) ) );
    //float d13_24 = TMath::Sqrt(pow(m_13_24.first,2) + pow(m_13_24.second,2) )*fabs( TMath::Sin( TMath::ATan(m_13_24.second/m_13_24.first) - TMath::ATan(targetHiggsMass2/targetHiggsMass1) ) );
    //float d14_23 = TMath::Sqrt(pow(m_14_23.first,2) + pow(m_14_23.second,2) )*fabs( TMath::Sin( TMath::ATan(m_14_23.second/m_14_23.first) - TMath::ATan(targetHiggsMass2/targetHiggsMass1) ) );

    // Mean
    float d12_34 = TMath::Sqrt(pow(m_12_34.first - targetHiggsMass1,2) + pow(m_12_34.second - targetHiggsMass2,2));
    float d13_24 = TMath::Sqrt(pow(m_13_24.first - targetHiggsMass1,2) + pow(m_13_24.second - targetHiggsMass2,2));
    float d14_23 = TMath::Sqrt(pow(m_14_23.first - targetHiggsMass1,2) + pow(m_14_23.second - targetHiggsMass2,2));

    // Mass Difference
    //float d12_34 = fabs(m_12_34.first - m_12_34.second);
    //float d13_24 = fabs(m_13_24.first - m_13_24.second);
    //float d14_23 = fabs(m_14_23.first - m_14_23.second);

    float the_min = std::min({d12_34, d13_24, d14_23});
        
#include "analysis_utils.h"

    std::vector<jet_t> outputJets = *presel_jets;

    if (the_min == d12_34){
        outputJets.at(0) = presel_jets->at(1 - 1);
        outputJets.at(1) = presel_jets->at(2 - 1);
        outputJets.at(2) = presel_jets->at(3 - 1);
        outputJets.at(3) = presel_jets->at(4 - 1);
    }

    else if (the_min == d13_24){
        outputJets.at(0) = presel_jets->at(1 - 1);
        outputJets.at(1) = presel_jets->at(3 - 1);
        outputJets.at(2) = presel_jets->at(2 - 1);
        outputJets.at(3) = presel_jets->at(4 - 1);
    }

    else if (the_min == d14_23){
        outputJets.at(0) = presel_jets->at(1 - 1);
        outputJets.at(1) = presel_jets->at(4 - 1);
        outputJets.at(2) = presel_jets->at(2 - 1);
        outputJets.at(3) = presel_jets->at(3 - 1);
    }

    else
        std::cout << "** [WARNING] : bbbb_jets_idxs_BothClosestToDiagonal : something went wrong with finding the smallest Dhh" << std::endl;

    return outputJets;
}

std::tuple<bool,bool> PairingEfficiencyFlags(std::vector<TLorentzVector> quarks,std::vector<TLorentzVector> jets)
{
    
    //output is two flags
    std::tuple<bool,bool> output;  

    //Let's start assumming that our four preselected jets are not coming the four b-quarks hadronization
    bool fullreconstruction=false;
    //Let's start assumming that our four preselected jets are not correctly paired
    bool fullpairing=false;

    //---------Create vector with the quark p4 and some quark id's
    //qid = 0, gen_H1_b1
    //qid = 1, gen_H1_b2
    //qid = 2, gen_H1_b3
    //qid = 3, gen_H1_b4 
    std::vector<std::tuple<TLorentzVector,int>> quarksandids;
    for (uint k=0;k < quarks.size();k++ ) quarksandids.push_back( std::make_tuple(quarks.at(k),k) );
    //---------Create vector with the quark p4 and qid
 
    //Loop over the jets to match them to the closest quark by dR and its quark id 
    std::vector<std::tuple<TLorentzVector,int>> jetsandquarkids;
    int nreco = 0;
    for (uint x=0;x < jets.size();x++ )
    {
       //Initialize variables
       float maxDeltaR = 0.3;
       bool  matched   = false;
       int   id        = -999;
       int   rem       = -999;
       //Loop over the quarkid array
       for (uint y=0;y < quarksandids.size(); y++ ){
            //Compute dR
            float deltaR = jets[x].DeltaR( std::get<0>(quarksandids[y]) );
            //if it dR<maxDeltaR, then is coming from one of the b-quarks
            if(deltaR < maxDeltaR){maxDeltaR=deltaR; matched=true; id = std::get<1>(quarksandids[y]); rem=y;}
       }
       //If matched, then count it
       if(matched) nreco+=1;
       //Fill the quarkid information associated to jet with the match
       jetsandquarkids.push_back(std::make_tuple(jets.at(x),id));
       //To avoid matching multiple quarks to the same jet, let's erase the matched quark from the list
       if(matched) quarksandids.erase(quarksandids.begin()+rem);
    }

    //Check1: Full Reconstruction flag
    //If one of the jets is not coming from the quark, then reconstuction is not complete (this event cannot be used as denominator).
    if (nreco == 4) fullreconstruction=true;

    //Check2: Full Pairing flag
    //Check that four jets are matched the H1 quarks or the H2 quarks
    int H1_b1_qid = std::get<1>(jetsandquarkids[0]);
    int H1_b2_qid = std::get<1>(jetsandquarkids[1]);
    int H2_b1_qid = std::get<1>(jetsandquarkids[2]);
    int H2_b2_qid = std::get<1>(jetsandquarkids[3]);

    if( (H1_b1_qid+H1_b2_qid==1) && (H2_b1_qid+H2_b2_qid==5) ) fullpairing=true;
    if( (H1_b1_qid+H1_b2_qid==5) && (H2_b1_qid+H2_b2_qid==1) ) fullpairing=true;

    //Let's return the output flags
    output = std::make_tuple(fullreconstruction,fullpairing);

    return output;
}


#endif
