#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/bridge-helper.h"
#include "ns3/internet-module.h"
#include "ns3/config-store-module.h"
#include "ns3/netanim-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include <vector>
#include <stdint.h>
#include <sstream>
#include <fstream>

using namespace ns3;

int main(int argc, char *argv[]){
	uint32_t nAp = 3;
        uint32_t nstas = 2; 
        LogComponentEnable ("UdpClient", LOG_LEVEL_INFO);
  	LogComponentEnable ("UdpServer", LOG_LEVEL_INFO);
	NodeContainer apNode;
	apNode.Create(nAp);
        
	NodeContainer serverNode;
	serverNode.Create(1);

	NodeContainer switchNode;
	switchNode.Create(1);
	
	NetDeviceContainer *csmaDevices = new NetDeviceContainer[nAp + 1];

	CsmaHelper csma, csma2;
	for(uint32_t i = 0; i < nAp; i++){ 
    		csmaDevices[i] = csma.Install (NodeContainer(apNode.Get(i), switchNode.Get(0)));
  	}
	csmaDevices[nAp] = csma.Install (NodeContainer(serverNode.Get(0), switchNode.Get(0)));

	BridgeHelper switch0;
	//BridgeHelper bridge0;
  	NetDeviceContainer switchDev; 
  	for(uint32_t i = 0; i < nAp + 1; i++){ 
    		switchDev.Add(csmaDevices[i].Get(1)); 
  	}
  	switch0.Install(switchNode.Get(0), switchDev); 


	YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  	//wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO); 

  	YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  	//wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
  	//wifiChannel.AddPropagationLoss("ns3::LogDistancePropagationLossModel");
  	phy.SetChannel (wifiChannel.Create ());

	Ssid ssid =  Ssid ("Ap-1");

	NodeContainer stationNode1;
	stationNode1.Create(nstas);

	WifiHelper wifi1;
  	wifi1.SetStandard (WIFI_PHY_STANDARD_80211n_2_4GHZ);
	wifi1.SetRemoteStationManager ("ns3::IdealWifiManager", "RtsCtsThreshold", UintegerValue (999999));

	WifiMacHelper mac1;

	mac1.SetType ("ns3::StaWifiMac","Ssid", SsidValue (ssid),"ActiveProbing", BooleanValue (true));

	NetDeviceContainer staDevices1;
  	staDevices1 = wifi1.Install (phy, mac1, stationNode1);

 	mac1.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));

  	NetDeviceContainer apDevices1;
  	apDevices1 = wifi1.Install (phy, mac1, apNode.Get(0));

	//NetDeviceContainer bridgeDev1;
  	//bridgeDev1 = bridge0.Install (apNode.Get(0),NetDeviceContainer(apDevices1,csmaDevices[0]));

	NodeContainer stationNode2;
	stationNode2.Create(nstas);

	WifiHelper wifi2;
  	wifi2.SetStandard (WIFI_PHY_STANDARD_80211n_2_4GHZ);
 wifi2.SetRemoteStationManager ("ns3::IdealWifiManager", "RtsCtsThreshold", UintegerValue (999999));

	WifiMacHelper mac2;

	mac2.SetType ("ns3::StaWifiMac","Ssid", SsidValue (ssid),"ActiveProbing", BooleanValue (true));

	NetDeviceContainer staDevices2;
  	staDevices2 = wifi2.Install (phy, mac2, stationNode2);

 	mac2.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));

  	NetDeviceContainer apDevices2;
  	apDevices2 = wifi2.Install (phy, mac2, apNode.Get(1));

	NodeContainer stationNode3;
	stationNode3.Create(1);

	WifiHelper wifi3;
  	wifi3.SetStandard (WIFI_PHY_STANDARD_80211n_2_4GHZ);
	wifi3.SetRemoteStationManager ("ns3::IdealWifiManager", "RtsCtsThreshold", UintegerValue (999999));

	WifiMacHelper mac3;

	mac3.SetType ("ns3::StaWifiMac","Ssid", SsidValue (ssid),"ActiveProbing", BooleanValue (true));

	NetDeviceContainer staDevices3;
  	staDevices3 = wifi1.Install (phy, mac3, stationNode3);

 	mac3.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));

  	NetDeviceContainer apDevices3;
  	apDevices3 = wifi3.Install (phy, mac3, apNode.Get(2));

	//NetDeviceContainer bridgeDev2;
  	//bridgeDev2 = bridge0.Install (apNode.Get(1),NetDeviceContainer(apDevices2,csmaDevices[1]));


	InternetStackHelper internetstack;
	
  	internetstack.Install(serverNode); 
  	internetstack.Install(switchNode);
	internetstack.Install (apNode);
	internetstack.Install(stationNode1);
	internetstack.Install(stationNode2);
	internetstack.Install(stationNode3);

  	Ipv4AddressHelper ip;
  	Ipv4InterfaceContainer serverInterface;
  	serverInterface = ip.Assign(csmaDevices[nAp].Get(0));

	Ipv4InterfaceContainer apInterface1;
	apInterface1 = ip.Assign(apDevices1);
	Ipv4InterfaceContainer apInterface2;
	apInterface2 = ip.Assign(apDevices2);
	Ipv4InterfaceContainer apInterface3;
	apInterface3 = ip.Assign(apDevices3);
	Ipv4InterfaceContainer staInterface1;
	staInterface1 = ip.Assign(staDevices1);


	Ipv4InterfaceContainer staInterface2;
	staInterface2 = ip.Assign(staDevices2);


	Ipv4InterfaceContainer staInterface3;
	staInterface3 = ip.Assign(staDevices3);

	MobilityHelper mobility;

  	mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (100.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

	mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  	mobility.Install (apNode);
        mobility.Install (switchNode);
	mobility.Install (serverNode);
	mobility.Install (stationNode2);
	mobility.Install (stationNode3);
	//mobility.Install (stationNode1);

	mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
	
  	mobility.Install (stationNode1);
	
  

        Ptr<ConstantVelocityMobilityModel> mobile;
	mobile = (stationNode1.Get(0))->GetObject<ConstantVelocityMobilityModel>();
	mobile->SetPosition(Vector(0.0,125.0,0.0));
	mobile->SetVelocity(Vector(15.0,0.0,0.0));

	mobile = (stationNode1.Get(1))->GetObject<ConstantVelocityMobilityModel>();
	mobile->SetPosition(Vector(0.0,110.0,0.0));
	mobile->SetVelocity(Vector(15.0,0.0,0.0));

	/*mobile = (stationNode2.Get(0))->GetObject<ConstantVelocityMobilityModel>();
	mobile->SetPosition(Vector(145.0,145.0,0.0));
	mobile->SetVelocity(Vector(10.0,0.0,0.0));

	mobile = (stationNode2.Get(1))->GetObject<ConstantVelocityMobilityModel>();
	mobile->SetPosition(Vector(155.0,155.0,0.0));
	mobile->SetVelocity(Vector(15.0,0.0,0.0));

	mobile = (stationNode3.Get(0))->GetObject<ConstantVelocityMobilityModel>();
	mobile->SetPosition(Vector(220.0,220.0,0.0));
	mobile->SetVelocity(Vector(15.0,0.0,0.0));*/
  	UdpServerHelper srv(9);  
  	ApplicationContainer srv_apps = srv.Install (stationNode1.Get(1)); //server = UDP recv 
  	srv_apps.Start (Seconds (0.5));
  	srv_apps.Stop (Seconds (15.0));

  	UdpClientHelper client(staInterface1.GetAddress (0), 9); // dest: IP,port
  	client.SetAttribute("MaxPackets",UintegerValue (64707202));
  	client.SetAttribute("Interval",TimeValue (Time ("0.01"))); 
  	client.SetAttribute("PacketSize",UintegerValue (1450));    
  	ApplicationContainer cln_apps = client.Install (serverNode.Get(0)); //cli = UDP send
  	cln_apps.Start (Seconds (0.5));
  	cln_apps.Stop (Seconds (60.0));
	Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  	AnimationInterface anim("HANDOFF_MODIFIED.xml");
        anim.UpdateNodeColor(apNode.Get(0),255,255,0);
	anim.UpdateNodeColor(apNode.Get(1),255,255,0);
	anim.UpdateNodeColor(apNode.Get(2),255,255,0);

	anim.UpdateNodeColor(switchNode.Get(0),0,255,0);
	anim.UpdateNodeColor(serverNode.Get(0),0,255,0);
	
	anim.UpdateNodeColor(stationNode1.Get(0),0,255,255);
	anim.UpdateNodeColor(stationNode1.Get(1),0,255,255);

	anim.UpdateNodeColor(stationNode2.Get(0),0,255,255);
	anim.UpdateNodeColor(stationNode2.Get(1),0,255,255);

	anim.UpdateNodeColor(stationNode3.Get(0),0,255,255);
	anim.UpdateNodeColor(stationNode1.Get(1),0,255,255);

	anim.UpdateNodeDescription(apNode.Get(0),"Ap-1");
        anim.SetConstantPosition(apNode.Get(0),0.0,100.0);
	anim.UpdateNodeDescription(apNode.Get(1),"Ap-2");
	anim.SetConstantPosition(apNode.Get(1),448.1,100.0);
	anim.UpdateNodeDescription(apNode.Get(2),"Ap-3");
	anim.SetConstantPosition(apNode.Get(2),896.2,100.0);

	anim.UpdateNodeDescription(switchNode.Get(0),"Switch Node");
	anim.SetConstantPosition(switchNode.Get(0),448.1,50.0);
	anim.UpdateNodeDescription(serverNode.Get(0),"Server Node");
	anim.SetConstantPosition(serverNode.Get(0),448.1,0.0);

	anim.UpdateNodeDescription(stationNode3.Get(0),"Station-3-1 node");
	anim.SetConstantPosition(stationNode3.Get(0),896.2,200.0);

	anim.UpdateNodeDescription(stationNode2.Get(0),"Station-2-1 node");
	anim.SetConstantPosition(stationNode2.Get(0),448.1,200.0);

	anim.UpdateNodeDescription(stationNode2.Get(1),"Station-2-2 node");
	anim.SetConstantPosition(stationNode2.Get(1),0.0,200.0);

	anim.UpdateNodeDescription(stationNode1.Get(0),"Station-1-1 node");
	anim.UpdateNodeDescription(stationNode1.Get(1),"Station-1-2 node");


	//anim.SetConstantPosition(stationNode1.Get(1),0.0,525.0);
	//anim.SetConstantPosition(serverNode.Get(0),110.0,110.0);
  	//anim.SetMaxPktsPerTraceFile(9999999999999999);
	Simulator::Stop(Seconds(60.0));

  	Simulator::Run ();
  	Simulator::Destroy ();
	return 0;
}



