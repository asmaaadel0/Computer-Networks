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
#include <iomanip>

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class Node : public cSimpleModule
{
private:
    std::vector<std::string> originalMsg;
    std::vector<std::bitset<8>> bitsVector;
    std::vector<std::bitset<4>> errors;
    int windowSize;
    simtime_t timeout;
    std::ofstream fout;

    void readFile(const char *filename);
    std::string framing(std::string payload);
    std::string deFraming(std::string framedPayload);

    void printReadingMessage(int m, int nextFrameToSendTemp, bool resend);
    void printTimeoutMessage(int ackExpected);
    void printSendingMessage(MyMessage_Base* message, int bitToModify, std::string lossMsg, int dup, int delay,int m, int nextFrameToSendTemp, bool resend);
//    void printSendingReceiverMessage(std::string payload, std::string lossMsg, std::string isAck, int number);
    void printSendingReceiverMessage(std::string lossMsg, std::string isAck, int number);
    void printReceivedReceiverMessage(std::string payload, int number);

    char calculateChecksum(std::string data) ;
    std::string calculateOnesComplement(const std::string& hexString);
    bool checkMessage(MyMessage_Base* message);

    void sendingMessageHandler(MyMessage_Base *message, const std::bitset<4> currentErrors, int m, int nextFrameToSendTemp, bool resend);
    void FrameSending();

    std::vector<cMessage *> timeoutEvents;
//    std::vector<MyMessage_Base *> sendingMessages;

    int nbuffered;
    int nextFrameToSend;
    int ackExpected;
    int frameExpected;

    void finish() override;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

#endif
