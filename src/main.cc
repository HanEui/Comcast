/**
 * ECE408 
 * comcast.cpp
 * Purpose: Network Simulator Entry Point
 * 
 * @author Kangqiao Lei
 * @version 0.3.0 04/26/16
 */

//#define NDEBUG // Comment out to turn on debug information and assertions

#include "event_manager.h"
#include "global.h"
#include <cassert>
#include <cstdio>
#include <string>
#include <algorithm>
#include <unistd.h>

#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/error/en.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "host.h"
#include "router.h"
#include "network.h"
#include "flow.h"
#include "packet.h"
#include "link.h"

//#include <fstream>

//#include "rapidjson/prettywriter.h" // for stringify JSON
//TODO: Examine How things are parsed
bool debug = false;
std::ostream *debugSS = &std::cout;
std::ostream *errorSS = &std::cerr;
std::ostream *logger = &std::clog;
std::string inputFile;
std::string outputFolder;

EventManager event_manager;

void parseInputs() {
  Network &net = event_manager.Net();
  rapidjson::Document root; // root is a JSON value represents the root of DOM.
  #ifndef NDEBUG
    *debugSS << "Parse a JSON file to document root." << std::endl;
  #endif
  FILE *input = fopen(inputFile.c_str(), "rb"); // "r" for non-Windows
  if (input!=NULL) {
    char readBuffer[65536];
    rapidjson::FileReadStream json(input, readBuffer, sizeof(readBuffer));
    root.ParseStream(json);
    fclose(input);
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    root.Accept(writer);
    *debugSS << "Original JSON:\n" << buffer.GetString() << std::endl;
  }
//		if (root.ParseStream(json).HasParseError()) {
//			fprintf(errorSS, "\nError(offset %u): %s\n", 
//				(unsigned)root.GetErrorOffset(),
//				GetParseError_En(root.GetParseError()));
//			return 1;
//		}
//	} else {
//		errorSS << "Unable to open file " << inputFile << std::endl; 
//		return -1;
//	}
  #ifndef NDEBUG
    *debugSS<< "Parsing to root succeeded." <<"\n\n" << std::endl;
  #endif
    rapidjson::Value::MemberIterator itr;

    {
	
      itr = root.FindMember("end"); // assert(root.HasMember("hosts")); // Old version
      global::MAX_SIMULATION_TIME = (itr != root.MemberEnd()) ? itr->value.GetDouble() : 10;
      #ifndef NDEBUG
        *debugSS << "Set end time of simulator: " << global::MAX_SIMULATION_TIME << std::endl;
      #endif	
    }	

    {
      assert(root.HasMember("hosts"));
      const rapidjson::Value& hosts = root["hosts"]; 
      for (rapidjson::SizeType i = 0; i < hosts.Size(); ++i) {
        const rapidjson::Value& chosts = hosts[i];
        net.AddHost(chosts.GetString()); 
        #ifndef NDEBUG
          *debugSS << "Added Host " << chosts.GetString() << std::endl;
        #endif
      }
    }

    #ifndef NDEBUG
      *debugSS << "Finished Adding Hosts." << std::endl;
    #endif	

    {
      const rapidjson::Value& routers = root["routers"]; 
      for (rapidjson::SizeType i = 0; i < routers.Size(); ++i) {
        const rapidjson::Value& crouter = routers[i];
        net.AddRouter(crouter.GetString());
        #ifndef NDEBUG
          *debugSS << "Added Router " << crouter.GetString() << std::endl;
        #endif
      }
    }

    #ifndef NDEBUG
     *debugSS << "Finished Adding Routers." << std::endl;
    #endif	


    {
      const rapidjson::Value& links = root["links"]; 

      for (rapidjson::SizeType i = 0; i < links.Size(); ++i) {
        const rapidjson::Value& clink = links[i];
        net.AddLink(clink["id"].GetString(), clink["endpoints"][0].GetString(), 
                    clink["endpoints"][1].GetString(), clink["rate"].GetDouble(), 
                    clink["buffer"].GetDouble(), clink["delay"].GetDouble());
        #ifndef NDEBUG
          *debugSS <<"Added Link " << clink["id"].GetString() << std::endl;
        #endif
      }
    }

    #ifndef NDEBUG
      *debugSS << "Finished Adding Links." << std::endl;
    #endif	


    {
      const rapidjson::Value& flows = root["flows"]; 
      std::string tcp_enum;
      for (rapidjson::SizeType i = 0; i < flows.Size(); ++i) {
        const rapidjson::Value& cflow = flows[i];
        net.AddFlow(cflow["id"].GetString(), cflow["start"].GetDouble(), 
                    cflow["size"].GetInt(), cflow["src"].GetString(), 
                    cflow["dst"].GetString(), cflow["protocol"].GetString());
        #ifndef NDEBUG
          *debugSS << "Added Flow " << cflow["id"].GetString() << std::endl;
        #endif
      }
    }

    #ifndef NDEBUG
      *debugSS << "Finished Adding Flows." << std::endl;
    #endif
	
}


int main(int argc, char *argv[]) {
  int c = -1, b = 0; // getopt options
  static char usageInfo[] = "[-i input_file] [-o output_file] [-d]\n"; // Prompt on invalid input
	
  #ifndef NDEBUG
    *debugSS << "Parsing options if they exist." << std::endl;
  #endif
/*	while ((c = getopt(argc, argv, "i:o:d")) != -1) {
		switch (c) {
            case 'i':
				inputFile = optarg;
                break;
			case 'o':
				outputFile = optarg;
				break;
			case 'd':
            	debug = true;
            	break;
			case '?':
                fprintf(errorSS, "Error Invalid option: %c\n", c);
                return -1;
				break;
            default:
                fprintf(errorSS, "Usage: %s %s", argv[0], usageInfo);
		}
	}
	if (inputFile.empty()) {
		cout << "Please specify the network topology input file:\n>";
		getline(cin, inputFile);
	}
*/
  std::cout<<"input file name :"<<std::endl;
  std::cin>>inputFile;
  std::string logFile = "./logs"+inputFile.substr(inputFile.rfind('/'), inputFile.rfind('.') - inputFile.rfind('/')) + ".log";
  std::streambuf* clogbuf = std::clog.rdbuf(logger->rdbuf());
  std::ofstream outputSS(logFile, std::ofstream::out);
  outputFolder = "./output";
  std::clog.rdbuf(outputSS.rdbuf());
  parseInputs();
	// Create Network Simulator object 
  #ifndef NDEBUG
    *debugSS << "Created Network Simulator object." << std::endl;
  #endif	
	
	// Load JSON Input File
  #ifndef NDEBUG
    *debugSS << "Loaded Network Topology." << std::endl;
  #endif
  event_manager.Setup();
  event_manager.Run();
  std::clog.rdbuf(clogbuf);
  outputSS.close();
  *debugSS<<"Output Log wrote to "<<logFile<<std::endl;
  
  //debugSS<<"queue size seen from main: "<<event_manager.queue_size()<<std::endl;
  
  //debugSS<<"queue size seen from main: "<<event_manager.queue_size()<<std::endl;
  //event_manager.Run();
  //debugSS<<"queue size seen from main: "<<event_manager.queue_size()<<std::endl;
  //EventManager event_manager("./out.txt", net);
  //EventManager eveman("./out.txt", net);
  //globall::simulator = eveman;
  //global::simulator.Setup();
  //global::simulator.Run();
  return 0;
}
