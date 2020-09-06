/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Manuel Requena <manuel.requena@cttc.es>
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/config-store.h"
#include <ns3/buildings-helper.h>
#include "ns3/flow-monitor-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/netanim-module.h"
//#include "ns3/gtk-config-store.h"
 

using namespace ns3;
static ns3::GlobalValue g_macroEnbTxPowerDbm ("macroEnbTxPowerDbm",
                                               "TX power [dBm] used by macro eNBs",
                                               ns3::DoubleValue (46.0),
  ns3::MakeDoubleChecker<double> ());
  
  static ns3::GlobalValue g_homeEnbTxPowerDbm ("homeEnbTxPowerDbm",
                                             "TX power [dBm] used by HeNBs",
                                             ns3::DoubleValue (26.0),
                                             ns3::MakeDoubleChecker<double> ());
  
                                             
  static ns3::GlobalValue g_macroEnbDlEarfcn ("macroEnbDlEarfcn",
                                             "DL EARFCN used by macro eNBs",
                                             ns3::UintegerValue (100),
  ns3::MakeUintegerChecker<uint16_t> ());
  
  static ns3::GlobalValue g_homeEnbDlEarfcn ("homeEnbDlEarfcn",
                                           "DL EARFCN used by HeNBs",
                                           ns3::UintegerValue (100),
                                           ns3::MakeUintegerChecker<uint16_t> ());
  
  
                                           
  static ns3::GlobalValue g_macroEnbBandwidth ("macroEnbBandwidth",
                                              "bandwidth [num RBs] used by macro eNBs",
                                              ns3::UintegerValue (25),
  ns3::MakeUintegerChecker<uint16_t> ()); // 5 MHz 
  
 static ns3::GlobalValue g_homeEnbBandwidth ("homeEnbBandwidth",
                                            "bandwidth [num RBs] used by HeNBs",
                                            ns3::UintegerValue (25),
                                            ns3::MakeUintegerChecker<uint16_t> ());
                                            
    static ns3::GlobalValue g_srsPeriodicity ("srsPeriodicity",
                                           "SRS Periodicity (has to be at least "
                                           "greater than the number of UEs per eNB)",
                                           ns3::UintegerValue (160),
                                           ns3::MakeUintegerChecker<uint16_t> ());                                          

NS_LOG_COMPONENT_DEFINE ("LTE-Throughput");



uint64_t lastTotalRx =0;

void
CalculateThroughput (uint64_t cur)
{
  Time now = Simulator::Now ();                                         /* Return the simulator's virtual time. */
  std::cout<<"Hello"<<std::endl;
  // double cur = (rlcStats->GetDlRxPackets  (1, 3) - lastTotalRx) * (double) 8/1e5;     /* Convert Application RX Packets to MBits. */
  // double cur = (sink->GetTotalRx()) * (double) 8/1e5;
  std::cout << now.GetSeconds () << "s: \t" << cur << " MB" << std::endl;
  // std::cout << rlcStats->GetDlRxPackets  (1, 3)<<std::endl;
  // lastTotalRx = cur;
  Simulator::Schedule (MilliSeconds (100), &CalculateThroughput,cur);
}




int main (int argc, char *argv[])
{	
  // LogComponentEnable ("RadioBearerStatsCalculator", LOG_LEVEL_ALL);
  uint16_t henb = 1;
  CommandLine cmd;
  cmd.Parse (argc, argv);
	UintegerValue uintegerValue;
    IntegerValue integerValue;
    DoubleValue doubleValue;
    BooleanValue booleanValue;
   StringValue stringValue;
   
   GlobalValue::GetValueByName ("macroEnbTxPowerDbm", doubleValue);
   double macroEnbTxPowerDbm = doubleValue.Get ();
   
 GlobalValue::GetValueByName ("homeEnbTxPowerDbm", doubleValue);
  double homeEnbTxPowerDbm = doubleValue.Get ();
   
   GlobalValue::GetValueByName ("macroEnbDlEarfcn", uintegerValue);
   uint32_t macroEnbDlEarfcn = uintegerValue.Get ();
   
   GlobalValue::GetValueByName ("homeEnbDlEarfcn", uintegerValue);
  uint16_t homeEnbDlEarfcn = uintegerValue.Get ();
  
   GlobalValue::GetValueByName ("macroEnbBandwidth", uintegerValue);
   uint16_t macroEnbBandwidth = uintegerValue.Get ();
   
   GlobalValue::GetValueByName ("homeEnbBandwidth", uintegerValue);
  uint16_t homeEnbBandwidth = uintegerValue.Get ();
     
     GlobalValue::GetValueByName ("srsPeriodicity", uintegerValue);
   uint16_t srsPeriodicity = uintegerValue.Get ();
   
    Config::SetDefault ("ns3::LteEnbRrc::SrsPeriodicity", UintegerValue (srsPeriodicity));
  // to save a template default attribute file run it like this:
  // ./waf --command-template="%s --ns3::ConfigStore::Filename=input-defaults.txt --ns3::ConfigStore::Mode=Save --ns3::ConfigStore::FileFormat=RawText" --run src/lte/examples/lena-first-sim
  //
  // to load a previously created default attribute file
  // ./waf --command-template="%s --ns3::ConfigStore::Filename=input-defaults.txt --ns3::ConfigStore::Mode=Load --ns3::ConfigStore::FileFormat=RawText" --run src/lte/examples/lena-first-sim

  ConfigStore inputConfig;
  inputConfig.ConfigureDefaults ();

  // Parse again so you can override default values from the command line
  cmd.Parse (argc, argv);

  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  lteHelper->SetFadingModel("ns3::TraceFadingLossModel");
  lteHelper->SetFadingModelAttribute ("TraceFilename", StringValue ("src/lte/model/fading-traces/fading_trace_EPA_3kmph.fad"));
	lteHelper->SetFadingModelAttribute ("TraceLength", TimeValue (Seconds (10.0)));
	lteHelper->SetFadingModelAttribute ("SamplesNum", UintegerValue (10000));
	lteHelper->SetFadingModelAttribute ("WindowSize", TimeValue (Seconds (0.5)));
	lteHelper->SetFadingModelAttribute ("RbNum", UintegerValue (100));

  // Uncomment to enable logging
  // lteHelper->EnableLogComponents ();

  // Create Nodes: eNodeB and UE
  NodeContainer enbNodes;
  NodeContainer ueNodes;
  enbNodes.Create (1);
  ueNodes.Create (75);
 
  Config::SetDefault ("ns3::LteEnbPhy::TxPower", DoubleValue (macroEnbTxPowerDbm));
   lteHelper->SetEnbAntennaModelType ("ns3::ParabolicAntennaModel");
   lteHelper->SetEnbAntennaModelAttribute ("Beamwidth",   DoubleValue (70));
   lteHelper->SetEnbAntennaModelAttribute ("MaxAttenuation",     DoubleValue (20.0));
   lteHelper->SetEnbDeviceAttribute ("DlEarfcn", UintegerValue (macroEnbDlEarfcn)); //2120 MHz
   lteHelper->SetEnbDeviceAttribute ("UlEarfcn", UintegerValue (macroEnbDlEarfcn + 18000)); //1930 MHz
   lteHelper->SetEnbDeviceAttribute ("DlBandwidth", UintegerValue (macroEnbBandwidth));
   lteHelper->SetEnbDeviceAttribute ("UlBandwidth", UintegerValue (macroEnbBandwidth));
   lteHelper->SetSchedulerType("ns3::PfFfMacScheduler");
  
   
  NodeContainer homeEnbs;
   homeEnbs.Create (henb);
   Config::SetDefault ("ns3::LteEnbPhy::TxPower", DoubleValue (homeEnbTxPowerDbm));
  lteHelper->SetEnbAntennaModelType ("ns3::IsotropicAntennaModel");
  lteHelper->SetEnbDeviceAttribute ("DlEarfcn", UintegerValue (homeEnbDlEarfcn));
  lteHelper->SetEnbDeviceAttribute ("UlEarfcn", UintegerValue (homeEnbDlEarfcn + 18000)); //2120 MHz
  lteHelper->SetEnbDeviceAttribute ("DlBandwidth", UintegerValue (homeEnbBandwidth));//1930 MHz
  lteHelper->SetEnbDeviceAttribute ("UlBandwidth", UintegerValue (homeEnbBandwidth));
  lteHelper->SetEnbDeviceAttribute ("CsgId", UintegerValue (1));
  lteHelper->SetEnbDeviceAttribute ("CsgIndication", BooleanValue (true));
  
  // Install Mobility Model
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (enbNodes);
  mobility.Install (homeEnbs);
  BuildingsHelper::Install (enbNodes);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (ueNodes);
  BuildingsHelper::Install (ueNodes);
  
  enbNodes.Get(0)->GetObject<MobilityModel>()->SetPosition(Vector(0.0,0.0,0.0));
   
   
   for ( uint32_t i = 0 ; i<75; i++)
   {
   
   ueNodes.Get(i)->GetObject<MobilityModel>()->SetPosition(Vector(126.0,50+i, 0.0));
   
   }
   
    homeEnbs.Get(0)->GetObject<MobilityModel>()->SetPosition(Vector(130, 0.0 ,0.0));

  // Create Devices and install them in the Nodes (eNB and UE)
  NetDeviceContainer enbDevs;
  NetDeviceContainer ueDevs;
  NetDeviceContainer homeEnbsDevs ;
  enbDevs = lteHelper->InstallEnbDevice (enbNodes);
  ueDevs = lteHelper->InstallUeDevice (ueNodes);
  homeEnbsDevs= lteHelper->InstallEnbDevice(homeEnbs);

  std::cout<<"\n\t\t**** Starting Simulation *****\n"<< std::endl;
  std::cout<<"Algo. \tTime(s)  \t Throughput(Kbps)" <<std::endl; 
  
  for(uint64_t i=0;i<3;++i){

          if(i==0){
         
          lteHelper->Attach (ueDevs, enbDevs.Get (0));
			lteHelper->SetSchedulerType ("ns3::TdTbfqFfMacScheduler");	
          // Activate a data radio bearer
          enum EpsBearer::Qci q = EpsBearer::GBR_GAMING;
          EpsBearer bearer (q);
          lteHelper->ActivateDataRadioBearer (ueDevs, bearer);
  
          lteHelper->EnableTraces ();
          Ptr<RadioBearerStatsCalculator> rlcStats = lteHelper->GetRlcStats ();

        
  
          Simulator::Stop (Seconds (0.5));
         // uint64_t count=0;
         // for(Time i =Simulator::;i!= 0.5;i+0.25)
         // count++;

          Simulator::Run ();
   
          Time now = Simulator::Now (); 
          std::cout << "TD-TBFQ(GBR_GAMING) \t"<< now.GetSeconds () << "\t"<< "  \t "<< rlcStats->GetDlRxData(1,3)*8/1000/now.GetSeconds() << std::endl;
  
         // std::cout<<rlcStats->GetDlRxData(1,3)<<std::endl;
         // GtkConfigStore config;
         // config.ConfigureAttributes ();
      }
  
  else if(i==1){
  
         lteHelper->SetSchedulerType ("ns3::RrFfMacScheduler");  
  
         lteHelper->Attach (ueDevs, enbDevs.Get (0));
  
         enum EpsBearer::Qci q = EpsBearer::NGBR_MC_DATA;
         EpsBearer bearer (q);
         lteHelper->ActivateDataRadioBearer (ueDevs, bearer);
  
         Ptr<RadioBearerStatsCalculator> rlcStats = lteHelper->GetRlcStats ();
   
         Simulator::Stop (Seconds (0.5)); 
         Simulator::Run ();  
   
         Time now = Simulator::Now ();    
         std::cout << "Rr(NGBR_MC_DATA) \t"<< now.GetSeconds () << "\t"<< "  \t "<< rlcStats->GetDlRxData(1,3)*8/1000/(now.GetSeconds()-0.5) << std::endl;
         // std::cout << now.GetSeconds () << "s: \t" << rlcStats->GetDlRxData(1,3)*8/1000/(now.GetSeconds()-0.5) << " MB" << std::endl;
  
     }

  else if(i==2){

         lteHelper->SetSchedulerType ("ns3::PssFfMacScheduler");  
  
         lteHelper->Attach (ueDevs, enbDevs.Get (0));
  
         enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
         EpsBearer bearer (q);
         lteHelper->ActivateDataRadioBearer (ueDevs, bearer);
  
 
         Ptr<RadioBearerStatsCalculator> rlcStats = lteHelper->GetRlcStats ();
 
  
         Simulator::Stop (Seconds (0.5)); 
         Simulator::Run ();  
   
         Time now = Simulator::Now (); 
         std::cout << "PSS(GBR_CONV-VOICE) \t"<< now.GetSeconds () << "\t"<< "  \t "<< rlcStats->GetDlRxData(1,3)*8/1000/(now.GetSeconds()-0.5) << std::endl;   
        // std::cout << now.GetSeconds () << "s: \t" << rlcStats->GetDlRxData(1,3)*8/1000/(now.GetSeconds()-0.5) << " MB" << std::endl;
  
    }
  

   }
 
   Simulator::Destroy ();
   // std::cout<<rlcStats->GetEpoch ()<<std::endl; 
   //std::cout << "Rx packets: " << RX_PACKETS << "/" << TX_PACKETS << std::endl;
   return 0;

}
