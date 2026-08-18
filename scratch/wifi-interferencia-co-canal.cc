#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"

#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"

#include <fstream>

using namespace ns3;

void imprimirMetricas(Ptr<FlowMonitor> flowMonitor, FlowMonitorHelper& flowMonitorHelper, std::string nomeArquivo, bool mesmoCanal, uint32_t run){
    
    // força o monitor a processar os pacotes ainda em trânsito antes de ler os dados
    flowMonitor->CheckForLostPackets();

    // rastreia e converte os dados dos fluxos associados a um flowId
    Ptr<Ipv4FlowClassifier> classifier =
        DynamicCast<Ipv4FlowClassifier>(
            flowMonitorHelper.GetClassifier()
        );
    
    // monta mapa com id e stats dos fluxos
    std::map<FlowId, FlowMonitor::FlowStats> stats =
        flowMonitor->GetFlowStats();
    
    //abrindo e verificando arquivo csv
    std::ofstream arquivoCsv;
    std::ifstream checarArquivo(nomeArquivo);
    bool arquivoVazio = true;
    if (checarArquivo.is_open()) {
        if (checarArquivo.peek() != std::ifstream::traits_type::eof()) {
            arquivoVazio = false;
        }
        checarArquivo.close();
    }

    arquivoCsv.open(nomeArquivo, std::ios::out | std::ios::app);
     if (arquivoVazio) {
        arquivoCsv << "Run,MesmoCanal,FlowId,Origem,Destino,PacotesEnviados,PacotesRecebidos,PacotesPerdidos,TaxaPerda,ThroughputMbps,AtrasoMedioMs\n";
    }
    
    // itera por fluxo, recupera dados, e calcula métricas
    for (const auto& flow : stats)
    {
        FlowId flowId = flow.first;
        FlowMonitor::FlowStats flowStats = flow.second;

        Ipv4FlowClassifier::FiveTuple t =
            classifier->FindFlow(flowId);

        std::cout << "----------------------------------------" << std::endl;
        std::cout << "Fluxo " << flowId << std::endl;
        std::cout << "Origem: " << t.sourceAddress << std::endl;
        std::cout << "Destino: " << t.destinationAddress << std::endl;

        // 1. Métricas Base do FlowMonitor
        std::cout << "Pacotes enviados: " << flowStats.txPackets << std::endl;
        std::cout << "Pacotes recebidos: " << flowStats.rxPackets << std::endl;
        std::cout << "Pacotes perdidos: " << flowStats.lostPackets << std::endl;

        // 3. Cálculo Correto da Taxa de Perda (%)
        double perda = 0.0;
        if (flowStats.txPackets > 0)
        {
            // Taxa baseada em tudo que não conseguiu chegar ao destino
            perda = 100.0 * static_cast<double>(flowStats.lostPackets) / flowStats.txPackets;
        }
        std::cout << "Taxa de perda: " << perda << "%" << std::endl;

        // 4. Cálculo do Throughput (Vazão)
        double throughput = 0.0;
        if (flowStats.timeLastRxPacket > flowStats.timeFirstTxPacket)
        {
            // Nota: rxBytes mede os bytes que chegaram com sucesso na camada IP
            throughput = flowStats.rxBytes * 8.0 /
                (
                    flowStats.timeLastRxPacket.GetSeconds() -
                    flowStats.timeFirstTxPacket.GetSeconds()
                );
        }
        std::cout << "Throughput: " << throughput / 1e6 << " Mbps" << std::endl;
        throughput = throughput / 1e6;

        // 5. Cálculo do Atraso Médio (Delay)
        double atraso = 0.0;
        if (flowStats.rxPackets > 0)
        {
            atraso = flowStats.delaySum.GetSeconds() / flowStats.rxPackets;
        }
        std::cout << "Atraso medio: " << atraso * 1000 << " ms" << std::endl;

        //grvando dados no csv
        arquivoCsv << run << ","
                   << mesmoCanal << ","
                   << flowId << ","
                   << t.sourceAddress << ","
                   << t.destinationAddress << ","
                   << flowStats.txPackets << ","
                   << flowStats.rxPackets << ","
                   << flowStats.lostPackets << ","
                   << perda << ","
                   << throughput << ","
                   << atraso << "\n";
    
    }

    arquivoCsv.close();
    std::cout << "Métricas anexadas com sucesso em: " << nomeArquivo << std::endl;
}


int main(int argc, char *argv[]){

    //config parametros de execução da simulação
    double dist = 1.0;
    bool mesmoCanal = true;
    uint32_t run = 1;

    CommandLine cmd;
    cmd.AddValue("dist", "Distancia entre os APs e as respectivas STAs", dist);
    cmd.AddValue("mesmoCanal", "Define se as redes utilizam o mesmo canal", mesmoCanal);
    cmd.AddValue("run", "número da execucao dentro da seed", run);
    cmd.Parse(argc, argv);

    RngSeedManager::SetSeed(1);
    RngSeedManager::SetRun(run);

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
    allNodes.Add(sta1);
    allNodes.Add(ap1);
    allNodes.Add(ap2);
    allNodes.Add(sta2);

    MobilityHelper mobility;
    
    Ptr<ListPositionAllocator> positionAlloc =
        CreateObject<ListPositionAllocator>();

    positionAlloc->Add(Vector(0.0, 0.0, 0.0));   // STA1
    positionAlloc->Add(Vector(1.0, 0.0, 0.0));   // AP1
    positionAlloc->Add(Vector(1.0 + dist, 0.0, 0.0));  // AP2
    positionAlloc->Add(Vector(2.0 + dist, 0.0, 0.0));  // STA2

    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.SetPositionAllocator(positionAlloc);
    mobility.Install(allNodes);



    //config básica do protocolo wifi
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211n);

    //config básica do canal (meio físico)
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    Ptr<YansWifiChannel> canal = channel.Create();

    //config básica da camada física (phy)
    YansWifiPhyHelper phyA;
    phyA.SetChannel(canal);
    phyA.Set("ChannelSettings", StringValue("{1, 20, BAND_2_4GHZ, 0}"));

    YansWifiPhyHelper phyB;
    phyB.SetChannel(canal);

    if(mesmoCanal){
        phyB.Set("ChannelSettings", StringValue("{1, 20, BAND_2_4GHZ, 0}"));
    }else{
        phyB.Set("ChannelSettings", StringValue("{6, 20, BAND_2_4GHZ, 0}"));
    }

    //config básica da camada mac
    WifiMacHelper mac;

    //instalação de net-devices (Apartamento A)
    Ssid ssid1 = Ssid("Apartamento-A");
    mac.SetType(
        "ns3::StaWifiMac",
        "Ssid", SsidValue(ssid1)
    );
    NetDeviceContainer interfaceSta1 = wifi.Install(phyA, mac, sta1);
    mac.SetType(
        "ns3::ApWifiMac",
        "Ssid", SsidValue(ssid1)
    );
    NetDeviceContainer interfaceAp1 = wifi.Install(phyA, mac, ap1);

    NetDeviceContainer interfacesRedeA;
    interfacesRedeA.Add(interfaceSta1);
    interfacesRedeA.Add(interfaceAp1);

    //instalação de net-devices (Apartamento B)
    Ssid ssid2 = Ssid("Apartamento-B");
    mac.SetType(
        "ns3::StaWifiMac",
        "Ssid", SsidValue(ssid2)
    );
    NetDeviceContainer interfaceSta2 = wifi.Install(phyB, mac, sta2);
    mac.SetType(
        "ns3::ApWifiMac",
        "Ssid", SsidValue(ssid2)
    );
    NetDeviceContainer interfaceAp2 = wifi.Install(phyB, mac, ap2);

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
    clientA.SetAttribute("MaxPackets", UintegerValue(80000));
    clientA.SetAttribute("Interval", TimeValue(MicroSeconds(200)));
    clientA.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientAppA = clientA.Install(sta1.Get(0));

    UdpClientHelper clientB(ipsRedeB.GetAddress(1), portaB);
    clientB.SetAttribute("MaxPackets", UintegerValue(80000));
    clientB.SetAttribute("Interval", TimeValue(MicroSeconds(200)));
    clientB.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientAppB = clientB.Install(sta2.Get(0));
    
    ApplicationContainer allAppClients;
    allAppClients.Add(clientAppA);
    allAppClients.Add(clientAppB);

    allAppClients.Start(Seconds(2.0));
    allAppClients.Stop(Seconds(10.0));


    //controle da simulação
    Simulator::Stop(Seconds(13.0));

    //criando monitorador de métricas
    FlowMonitorHelper flowMonitorHelper;
    Ptr<FlowMonitor> flowMonitor = flowMonitorHelper.InstallAll();

    Simulator::Run();
    imprimirMetricas(flowMonitor, flowMonitorHelper, "scratch/dados.csv", mesmoCanal, run);
    Simulator::Destroy();

    return 0;
}