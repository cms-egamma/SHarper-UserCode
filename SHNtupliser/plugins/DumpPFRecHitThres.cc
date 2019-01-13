
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "CondFormats/EcalObjects/interface/EcalPFRecHitThresholds.h"
#include "CondFormats/DataRecord/interface/EcalPFRecHitThresholdsRcd.h"

#include "TTree.h"
#include "TFile.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>


class DumpPFRecHitThres : public edm::EDAnalyzer { 


public:
  explicit DumpPFRecHitThres(const edm::ParameterSet& iPara);
  virtual ~DumpPFRecHitThres(){}
  
  DumpPFRecHitThres(const DumpPFRecHitThres& rhs)=delete;
  DumpPFRecHitThres& operator=(const DumpPFRecHitThres& rhs)=delete;

 private:
  template<typename T>
  void getToken(edm::EDGetTokenT<T>& token,const edm::ParameterSet& iPara,const std::string& paraName){
    token = consumes<T>(iPara.getParameter<edm::InputTag>(paraName));
  }
  
  virtual void beginJob()override{}
  virtual void beginRun(const edm::Run& run,const edm::EventSetup& iSetup)override{}
  virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) override;
  virtual void endJob()override{}
  
  void dumpBarrelThres(const EcalPFRecHitThresholds& thresholds,int iPhi);
  void dumpEndcapThres(const EcalPFRecHitThresholds& thresholds,int iY, int iZ);

private:
  bool written_;
  std::string logPrefix_;
};


DumpPFRecHitThres::DumpPFRecHitThres(const edm::ParameterSet& iPara):
  written_(false),
  logPrefix_("pfrechit thres")
{


}

void DumpPFRecHitThres::dumpBarrelThres(const EcalPFRecHitThresholds& thresholds,int iPhi)
{
  if(iPhi<1 || iPhi>360) return;

  for(int iEta = -85; iEta <= 85; ++iEta){
    if(iEta==0) continue;
    EBDetId detId(iEta,iPhi);
    auto thresItr =  thresholds.find(detId);
    if(thresItr == thresholds.end()){
      std::cout <<logPrefix_<<" warning missing threshold for "<<iEta<<" "<<iPhi<<std::endl;
    }else{
      std::cout <<logPrefix_<<" iEta: "<<iPhi<<" iPhi "<<iPhi<<" : "<<*thresItr<<std::endl;
    }
  }
  
}

void DumpPFRecHitThres::dumpEndcapThres(const EcalPFRecHitThresholds& thresholds,int iY, int iZ)
{
  if(iY<1 || iY>100) return;

  for(int iX = 1; iX <= 100; ++iX){
    if(!EEDetId::validDetId(iX,iY,iZ)) continue;
    EEDetId detId(iX,iY,iZ);
    auto thresItr =  thresholds.find(detId);
    if(thresItr == thresholds.end()){
      std::cout <<logPrefix_<<"warning missing threshold for "<<iX<<" "<<iY<<" "<<iZ<<std::endl;
    }else{
      std::cout <<logPrefix_<<"iX: "<<iX<<" iY "<<iY<<" iZ "<<iZ<<" : "<<*thresItr<<std::endl;
    }
  }
  
}

void DumpPFRecHitThres::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  if(!written_){
    //edm::Service<TFileService> fs;
    edm::ESHandle<EcalPFRecHitThresholds> thresHandle;
    iSetup.get<EcalPFRecHitThresholdsRcd>().get(thresHandle);
    
    dumpBarrelThres(*thresHandle,1);
    dumpEndcapThres(*thresHandle,50,1);
   
       //fs->file().WriteObject(gbrHandle.product(),name.c_str());
    written_=true;
  }
}


DEFINE_FWK_MODULE(DumpPFRecHitThres);
