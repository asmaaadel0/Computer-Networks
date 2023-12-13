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
    // TODO - Generated method body
    std::ifstream fstream;
    std::string line;


    fstream.open("coordinator.txt", std::ifstream::in);

    if (fstream)
    {
       if(getline(fstream, line))
       {
           nodeId = atoi(line.substr(0, 1).c_str());
           startTime = atof(line.substr(2, line.length()).c_str());
//           node = line.substr(0, line.find(" "));
//           startTime = line.substr(line.find(" ") + 1, line.length());
       }
    }
    else
    {
       throw cRuntimeError("Error opening coordinator.txt");
    }
    fstream.close();

    //send msg to myself to start the communication:
    scheduleAt(simTime()+ startTime , new cMessage(""));
}

void Master::handleMessage(cMessage *msg)
{
    // TODO - Generated method body

    if(msg->isSelfMessage())
    {
        msg->setName("GO!");
        send(msg, "out", nodeId);
////        msg->setName(send.c_str());
//        char * sended;
//        std::strcat( sended , nodeId);
//        std::strcat( sended , startTime);
//        msg->setName(sended);
////        msg->setName(send.c_str());
////        send(msg, "out", 0);
////        cMessage* duplicate = msg->dup();
////        send(duplicate, "out", 1);
    }

}
