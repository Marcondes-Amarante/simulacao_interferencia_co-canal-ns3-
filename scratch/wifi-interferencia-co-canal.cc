#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"

#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;

void imprimirMetricas(Ptr<FlowMonitor> flowMonitor, FlowMonitorHelper& flowMonitorHelper){
    
    //força o monitor a monitorar os pacotes ainda em trânsito
    flowMonitor->CheckForLostPackets();

    //rastreia e converte os dados dos fluxos associados a um flowId
    Ptr<Ipv4FlowClassifier> classifier =
        DynamicCast<Ipv4FlowClassifier>(
            flowMonitorHelper.GetClassifier()
        );
    
    //monta dict com id e stats dos fluxos
    std::map<FlowId, FlowMonitor::FlowStats> stats =
        flowMonitor->GetFlowStats();
    
    //itera por fluxo, recupera dados, e calcula métricas
    for (const auto& flow : stats)
    {
        FlowId flowId = flow.first;
        FlowMonitor::FlowStats flowStats = flow.second;

        Ipv4FlowClassifier::FiveTuple t =
            classifier->FindFlow(flowId);

        std::cout << "----------------------------------------"
                  << std::endl;

        std::cout << "Fluxo " << flowId << std::endl;

        std::cout << "Origem: "
                  << t.sourceAddress
                  << std::endl;

        std::cout << "Destino: "
                  << t.destinationAddress
                  << std::endl;

        std::cout << "Pacotes enviados: "
                  << flowStats.txPackets
                  << std::endl;

        std::cout << "Pacotes recebidos: "
                  << flowStats.rxPackets
                  << std::endl;

        std::cout << "Pacotes perdidos: "
                  << flowStats.lostPackets
                  << std::endl;

        double perda = 0.0;

        if (flowStats.txPackets > 0)
        {
            perda =
                100.0 *
                (flowStats.txPackets - flowStats.rxPackets)
                / flowStats.txPackets;
        }

        std::cout << "Taxa de perda: "
                  << perda
                  << "%"
                  << std::endl;

        double throughput = 0.0;

        if (flowStats.timeLastRxPacket >
            flowStats.timeFirstTxPacket)
        {
            throughput =
                flowStats.rxBytes * 8.0 /
                (
                    flowStats.timeLastRxPacket.GetSeconds() -
                    flowStats.timeFirstTxPacket.GetSeconds()
                );
        }

        std::cout << "Throughput: "
                  << throughput / 1e6
                  << " Mbps"
                  << std::endl;

        double atraso = 0.0;

        if (flowStats.rxPackets > 0)
        {
            atraso =
                flowStats.delaySum.GetSeconds()
                / flowStats.rxPackets;
        }

        std::cout << "Atraso medio: "
                  << atraso * 1000
                  << " ms"
                  << std::endl;
    }

}

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


    //config da aplicação UDP
    //Ap das redes como server UDP
    uint16_t portaA = 5000;
    UdpServerHelper serverA(portaA);
    ApplicationContainer serverAppA = serverA.Install(ap1.Get(0));

    uint16_t portaB = 5001;
    UdpServerHelper serverB(portaB);
    ApplicationContainer serverAppB = serverB.Install(ap2.Get(0));

    ApplicationContainer allAppServers;
    allAppServers.Add(serverAppA);
    allAppServers.Add(serverAppB);

    allAppServers.Start(Seconds(1.0));
    allAppServers.Stop(Seconds(10.0));

    //Stations como clients
    UdpClientHelper clientA(ipsRedeA.GetAddress(1), portaA);
    clientA.SetAttribute("MaxPackets", UintegerValue(1000));
    clientA.SetAttribute("Interval", TimeValue(Seconds(0.01)));
    clientA.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientAppA = clientA.Install(sta1.Get(0));

    UdpClientHelper clientB(ipsRedeB.GetAddress(1), portaB);
    clientB.SetAttribute("MaxPackets", UintegerValue(1000));
    clientB.SetAttribute("Interval", TimeValue(Seconds(0.01)));
    clientB.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientAppB = clientB.Install(sta2.Get(0));
    
    ApplicationContainer allAppClients;
    allAppClients.Add(clientAppA);
    allAppClients.Add(clientAppB);

    allAppClients.Start(Seconds(2.0));
    allAppClients.Stop(Seconds(10.0));


    //controle da simulação
    Simulator::Stop(Seconds(11.0));

    //criando monitorador de métricas
    FlowMonitorHelper flowMonitorHelper;
    Ptr<FlowMonitor> flowMonitor = flowMonitorHelper.InstallAll();

    Simulator::Run();
    imprimirMetricas(flowMonitor, flowMonitorHelper);
    Simulator::Destroy();

    return 0;
}