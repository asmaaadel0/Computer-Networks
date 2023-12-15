//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "master.h"
#include <iostream>
#include <fstream>
#include <string>

Define_Module(Master);

void Master::initialize()
{
    std::ifstream fstream;
    std::string line;

    fstream.open("coordinator.txt", std::ifstream::in);

    if (fstream)
    {
       if(getline(fstream, line))
       {
           nodeId = atoi(line.substr(0, 1).c_str());
           startTime = atof(line.substr(2, line.length()).c_str());
       }
    }
    else
    {
       throw cRuntimeError("Error opening coordinator.txt");
    }
    fstream.close();

    //send message to myself to start the communication:
    scheduleAt(simTime()+ startTime , new cMessage(""));
}

void Master::handleMessage(cMessage *msg)
{
    if(msg->isSelfMessage())
    {
        msg->setName("GO!");
        send(msg, "out", nodeId);
    }

}
