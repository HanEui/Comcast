#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <map>
#include <queue>
#include <iostream>
#include <fstream>
#include <memory>
#include "network.h"
#include "event.h"

//forward declaration
class Flow;
class Link;
class Node;


class EventManager{
  public:
    EventManager();
    void log(std::string);
    void Setup();
    void Run();
    bool isDone() const;
    void push(std::shared_ptr<Event>);
    Network& Net();
    double time();
    int queue_size();
    Node& GetNode(std::string);
    double ReportAvgRTT(double);
    double ReportCWND(double);  

  private:
    std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event> >, EventCmp > queue_;
    std::string output_filename_="";
    std::ofstream out_file_;
    Network net_;
    double time_ = 0;
    int finished_ = 0;
    bool done_=false;
};
#endif
