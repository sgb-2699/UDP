#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/netanim-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Udp-Packet-Analysis");

int main() {
	LogComponentEnable("UdpServer", LOG_LEVEL_INFO);
	LogComponentEnable("UdpClient", LOG_LEVEL_INFO);

	NodeContainer nodes;
	nodes.Create(4);

	PointToPointHelper pointToPoint;
	pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
	pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

	NetDeviceContainer devices01, devices12, devices23;

	devices01 = pointToPoint.Install(nodes.Get(0), nodes.Get(1));
	devices12 = pointToPoint.Install(nodes.Get(1), nodes.Get(2));
	devices23 = pointToPoint.Install(nodes.Get(2), nodes.Get(3));

	InternetStackHelper stack;
	stack.Install(nodes);

	Ipv4AddressHelper address;

	address.SetBase("10.1.1.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces01 = address.Assign(devices01);

	address.SetBase("10.1.2.0", "255.255.255.0");
        Ipv4InterfaceContainer interfaces12 = address.Assign(devices12);

	address.SetBase("10.1.3.0", "255.255.255.0");
        Ipv4InterfaceContainer interfaces23 = address.Assign(devices23);

	UdpServerHelper server(9);
	ApplicationContainer serverApps = server.Install(nodes.Get(3));
	serverApps.Start(Seconds(1.0));
	serverApps.Stop(Seconds(10.0));

	UdpClientHelper client(interfaces23.GetAddress(1), 9);
	client.SetAttribute("PacketSize", UintegerValue(1024));
	client.SetAttribute("MaxPackets", UintegerValue(10));
	client.SetAttribute("Interval", TimeValue(Seconds(0.1)));
	ApplicationContainer clientApps = client.Install(nodes.Get(0));
	clientApps.Start(Seconds(2.0));
	clientApps.Stop(Seconds(10.0));
	
	Ipv4GlobalRoutingHelper::PopulateRoutingTables();

	AsciiTraceHelper ascii;
	pointToPoint.EnableAsciiAll(ascii.CreateFileStream("udp-trace-analysis.tr"));

	pointToPoint.EnablePcapAll("udp-pcap");

	AnimationInterface anim("udp-trace-analysis-prog7.xml");
	anim.SetConstantPosition(nodes.Get(0), 0, 50);
	anim.SetConstantPosition(nodes.Get(1), 25, 50);
	anim.SetConstantPosition(nodes.Get(2), 50, 50);
	anim.SetConstantPosition(nodes.Get(3), 75, 50);

	Simulator::Stop(Seconds(10.0));
	Simulator::Run();
	Simulator::Destroy();

	return 0;
}
