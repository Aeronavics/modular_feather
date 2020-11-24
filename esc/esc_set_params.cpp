#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <math.h>
#include <vector>
#include <uavcan/uavcan.hpp>

using namespace std;

/*
 * Remote reconfiguration services.
 */
#include <uavcan/protocol/param/GetSet.hpp>
#include <uavcan/protocol/param/ExecuteOpcode.hpp>

extern uavcan::ICanDriver& getCanDriver(std::string interface);
extern uavcan::ISystemClock& getSystemClock();

constexpr unsigned NodeMemoryPoolSize = 16384;

/**
 * Convenience function for blocking service calls.
 */
template <typename T>
std::pair<int, typename T::Response> performBlockingServiceCall(uavcan::INode& node,
                                                                uavcan::NodeID remote_node_id,
                                                                const typename T::Request& request)
{
    bool success = false;
    typename T::Response response;  // Generated types have zero-initializing constructors, always.

    uavcan::ServiceClient<T> client(node);
    client.setCallback([&](const uavcan::ServiceCallResult<T>& result)
        {
            success = result.isSuccessful();
            response = result.getResponse();
        });

    const int call_res = client.call(remote_node_id, request);
    if (call_res >= 0)
    {
        while (client.hasPendingCalls())
        {
            const int spin_res = node.spin(uavcan::MonotonicDuration::fromMSec(2));
            if (spin_res < 0)
            {
                return {spin_res, response};
            }
        }
        return {success ? 0 : -uavcan::ErrFailure, response};
    }
    return {call_res, response};
}

int main(int argc, const char** argv)
{
    if (argc < 4)
    {
        std::cerr << "Usage: " << argv[0] << " <get/set> <interface> <node-id> <remote-node-id> <filepath>" << std::endl;
        return 1;
    }

    const std::string mode = argv[1];
    const std::string interface = argv[2];
    const uavcan::NodeID self_node_id = std::stoi(argv[3]);
    const uavcan::NodeID remote_node_id = std::stoi(argv[4]);
    const std::string filepath = argv[5];

    if (mode != "get" && mode != "set")
    {
	std::cerr << "Not set to either get or set mode" << std::endl;
	std::cerr << "Usage: " << argv[0] << " <get/set> <interface> <node-id> <remote-node-id> <filepath>" << std::endl;
        return 1;
    }


    uavcan::Node<NodeMemoryPoolSize> node(getCanDriver(interface), getSystemClock());
    node.setNodeID(self_node_id);
    node.setName("org.uavcan.tutorial.configurator");

    const int node_start_res = node.start();
    if (node_start_res < 0)
    {
        throw std::runtime_error("Failed to start the node; error: " + std::to_string(node_start_res));
    }

    node.setModeOperational();

    std::vector<uavcan::protocol::param::GetSet::Response> remote_params;

    if (mode == "get")
    {
	// Open the file to print to
	ofstream filestream(filepath);
	while (true)
        {
            uavcan::protocol::param::GetSet::Request req;
            req.index = remote_params.size();
	    // Make call to get parameter
            auto res = performBlockingServiceCall<uavcan::protocol::param::GetSet>(node, remote_node_id, req);
	    // Check for an error code
            if (res.first < 0)
            {
		throw std::runtime_error("Failed to get param: " + std::to_string(res.first));
            }
	    if (res.second.name.empty())  // Empty name means no such param, which means we're finished
            {
                break;
            }
	    // Print the data to the file
            filestream << res.second << std::endl << std::endl;
            remote_params.push_back(res.second);
        }

        return 0;
    }
    else if (mode == "set")
    {
        /*
         * Setting all parameters to their maximum values, if applicable. Access by name.
         */
        std::string line;
	// Open the file
        fstream ymlfile(filepath);
        if (ymlfile.is_open())
        {
	    // Skip to the second line of the file as the first is not needed
	    getline(ymlfile, line);
	    getline(ymlfile, line);
	    // While we have not hit the end of the file
	    while(!ymlfile.eof()){
		bool floater = 0;
		bool inter = 0;
		bool booler = 0;
		float floatVal = 0;
		int intVal = 0;
		bool boolVal = 0;
		std::string name = "";

		uavcan::protocol::param::GetSet::Request req;

		// Check if the value type is an integer
		if(line.find("integer") != std::string::npos){
		    floater = 0;
                    inter = 1;
                    booler = 0;
		    // Remove the characters before the value
		    line.erase(0,17);
		    intVal = std::stoi(line);
		    req.value.to<uavcan::protocol::param::Value::Tag::integer_value>() = intVal;
		}
		// Check if the value type is a float
		else if(line.find("real") != std::string::npos){
		    floater = 1;
                    inter = 0;
                    booler = 0;
		    // Remove the characters before the value
                    line.erase(0,14);
                    floatVal = std::stof(line);
		    req.value.to<uavcan::protocol::param::Value::Tag::real_value>() = floatVal;
		}
		// Check if the value type is a string
                else if(line.find("boolean") != std::string::npos){
		    floater = 0;
                    inter = 0;
                    booler = 1;
		    // Remove the characters before the value
                    line.erase(0,17);
		    boolVal = std::stoi(line);
		    req.value.to<uavcan::protocol::param::Value::Tag::boolean_value>() = boolVal;
		}

		// Skip forward to the line containin the name of the parameter
		for(int i = 0; i < 7; i++)
		{
		    getline(ymlfile, line);
		}
		// Get the name of the parameter from the line
		line.erase(0,7);
		name = line.substr(0,line.size()-1);
                req.name = name.c_str();
		
                auto res = performBlockingServiceCall<uavcan::protocol::param::GetSet>(node, remote_node_id, req);
		// Check that there is not an error code returned
                if (res.first < 0)
                { 
                    throw std::runtime_error("Failed to set param: " + std::to_string(res.first));
                }
		// Check if the parameter was updated to the new value
		if (inter){
			if (res.second.value.to<uavcan::protocol::param::Value::Tag::integer_value>() != intVal){
				cout << "Failed to update param: " << res.second.name.c_str() << endl;
			}
		}
		else if (floater){
                        if (res.second.value.to<uavcan::protocol::param::Value::Tag::real_value>() != floatVal){
                                cout << "Failed to update param: " << res.second.name.c_str() << endl;
                        }
                }
		else if (booler){
                        if (res.second.value.to<uavcan::protocol::param::Value::Tag::boolean_value>() != boolVal){
                                cout << "Failed to update param: " << res.second.name.c_str() << endl;
                        }
                }

		// Skip to the value of the next parameter
		for(int i = 0; i < 3; i++)
                {
                    getline(ymlfile, line);
                }
            }
        }
        else std::cout << "Unable to open file";

        return 0;
    }
}
