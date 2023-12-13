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

#ifndef __NETWORK_PROJECT_NODE_H_
#define __NETWORK_PROJECT_NODE_H_

#include "MyMessage_m.h"
#include <omnetpp.h>
#include <vector>
#include <string>
#include <bitset>
#include <fstream>

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class Node : public cSimpleModule
{
private:
    std::vector<std::string> originalMsg;
    std::vector<std::bitset<8>> bitsVector; //later: is originalMsg enough ?
    std::vector<std::bitset<4>> errors;
//    std::vector<int> errors;
    int windoSize;
    std::ofstream fout;


//public:
    void ReadFile(const char *filename);
//    void messageHandler(MyFrame_Base *message /*, const char *code, int modifyIndex */ , int &index, int &nextFrameToSendTemp);
    void FrameSending(MyMessage_Base *message /*, const char *code, int modifyIndex  , int &index, int &nextFrameToSendTemp*/ );

    //------------samaa -----------//
//    std::vector<std::string> errors;
//    simtime_t timeout;
//    std::vector<cMessage *> timeoutEvents;
//    int nextFrameToSend;
//    int ackExpected;
//    int frameExpected;
//    int nbuffered;
//    std::string starternode;
//    int starternodeid;
//    int starttime;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

#endif
