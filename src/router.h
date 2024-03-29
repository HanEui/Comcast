#ifndef ROUTER_H_
#define ROUTER_H_

class Network;
class Packet;
class Link;
class EventManager;
#include "node.h"
#include <map>

extern EventManager event_manager;

class Router : public Node{
  public:
    Router(std::string id);
    virtual void SendPacket(Packet, double);
    virtual void ReceivePacket(Link&, Packet, double);
    bool allowedToTransmit();
    Link& GetRoute(std::string); // looks up the routing table and returns the link (first link ref is the link back to the src)
    void UpdateTable(std::string); // updates the routing table every x time step
    void UpdateCost();
    void SendControl();
    void ReceiveControl(Packet p);
    std::map<std::string, double> RoutingVector() const;
    Link& Greedy(std::string);
    void Init(); //initializes the routing table
  private:
    double last_control_; //keep track of the time last control packet was sent
    Link* received_from_;
    //each row represents each router's dist_ + cost_ vector
    std::map<std::string, std::map<std::string, double> > routing_table_;

    //map <neighbor id, link cost>
    std::map<std::string, double> cost_;
    
    //map <host id, distance int>
    std::map<std::string, int> dist_;

    //map <hst id, neighbor id>
    std::map<std::string, std::string> next_hop_;
};
#endif
