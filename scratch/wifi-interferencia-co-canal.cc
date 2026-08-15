#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"

#include "ns3/applications-module.h"

using namespace ns3;

int main(){

    //criação dos nós
    NodeContainer ap1;
    NodeContainer sta1;

    ap1.Create(1);
    sta1.Create(1);

    NodeContainer ap2;
    NodeContainer sta2;

    ap2.Create(1);
    sta2.Create(1);

    //definindo posição dos nós
    NodeContainer allNodes;
    allNodes.Add(ap1);
    allNodes.Add(sta1);
    allNodes.Add(ap2);
    allNodes.Add(sta2);

    MobilityHelper mobility;
    
    Ptr<ListPositionAllocator> positionAlloc =
        CreateObject<ListPositionAllocator>();

    positionAlloc->Add(Vector(2.0, 0.0, 0.0));   // AP1
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));   // STA1
    positionAlloc->Add(Vector(10.0, 0.0, 0.0));  // AP2
    positionAlloc->Add(Vector(12.0, 0.0, 0.0));  // STA2

    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.SetPositionAllocator(positionAlloc);
    mobility.Install(allNodes);



    //config básica do protocolo wifi
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211n);

    //config básica do canal (meio físico)
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();

    //config básica da camada física (phy)
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());



    //config básica da camada mac
    WifiMacHelper mac;

    //instalação de net-devices (Apartamento A)
    Ssid ssid1 = Ssid("Apartamento-A");
    mac.SetType(
        "ns3::StaWifiMac",
        "Ssid", SsidValue(ssid1)
    );
    NetDeviceContainer interfaceSta1 = wifi.Install(phy, mac, sta1);
    mac.SetType(
        "ns3::ApWifiMac",
        "Ssid", SsidValue(ssid1)
    );
    NetDeviceContainer interfaceAp1 = wifi.Install(phy, mac, ap1);

    NetDeviceContainer interfacesRedeA;
    interfacesRedeA.Add(interfaceSta1);
    interfacesRedeA.Add(interfaceAp1);

    //instalação de net-devices (Apartamento B)
    Ssid ssid2 = Ssid("Apartamento-B");
    mac.SetType(
        "ns3::StaWifiMac",
        "Ssid", SsidValue(ssid2)
    );
    NetDeviceContainer interfaceSta2 = wifi.Install(phy, mac, sta2);
    mac.SetType(
        "ns3::ApWifiMac",
        "Ssid", SsidValue(ssid2)
    );
    NetDeviceContainer interfaceAp2 = wifi.Install(phy, mac, ap2);

    NetDeviceContainer interfacesRedeB;
    interfacesRedeB.Add(interfaceSta2);
    interfacesRedeB.Add(interfaceAp2);

    //adicionando pilha de protocolos TCP/IP aos nós
    InternetStackHelper internet;
    internet.Install(allNodes);

    //atribuindo endereços de ip
    Ipv4AddressHelper address;
    address.SetBase(
        Ipv4Address("192.168.1.0"),
        Ipv4Mask("255.255.255.0")
    );
    Ipv4InterfaceContainer ipsRedeA = address.Assign(interfacesRedeA);

    address.SetBase(
        Ipv4Address("192.168.2.0"),
        Ipv4Mask("255.255.255.0")
    );
    Ipv4InterfaceContainer ipsRedeB = address.Assign(interfacesRedeB);


    //criando aplicação de ping para teste da comunicação intra-rede
    V4PingHelper ping1(ipsRedeA.GetAddress(0));


    return 0;
}