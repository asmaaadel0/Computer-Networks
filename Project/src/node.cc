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

#include "node.h"
#include "MyMessage_m.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>


Define_Module(Node);



void Node::readFile(const char *filename)
{
    std::ifstream fstream;
    std::string line;

    fstream.open(filename, std::ifstream::in);

    if (fstream)
    {
        while (getline(fstream, line))
        {
            errors.push_back(std::bitset<4>((line.substr(0, 4))));
            originalMsg.push_back(line.substr(5, line.length() - 5));
        }

    }
    else
    {
        throw cRuntimeError("Error opening file");

    }
    fstream.close();
}

std::string Node::framing(std::string payload)
{
    std::string frame = "";

    frame += '$';
    for (auto character : payload)
    {
        if (character == '$' || character == '/')
            frame += '/';

        frame += character;
    }

    frame += '$';

    return frame;
}

std::string Node::deFraming(std::string framedPayload)
{
    std::string payload = "";
    size_t i = 1;
    while (i < framedPayload.length() - 1)
    {
        char currentChar = framedPayload[i];

        if (currentChar == '/')
        {
            i++;
            payload += framedPayload[i];
        }
        else
        {
            payload += currentChar;
        }

        i++;
    }

    return payload;
}


void Node::printReadingMessage(int m, int nextFrameToSendTemp)
{
    fout << "At time [" << simTime() + par("PT").doubleValue() * (m - nextFrameToSendTemp) << "]," << " Node[" << getIndex()
            << "] , Introducing channel error with code =[" << errors[m] << "] ." << endl;

    EV << "At time [" << simTime() + par("PT").doubleValue() * (m - nextFrameToSendTemp) << "]," << " Node[" << getIndex()
            << "] , Introducing channel error with code = [" << errors[m] << "] ." << endl;
}

void Node::printTimeoutMessage(int ackExpected)
{
    fout << "Time out event at time [" << simTime() << "], at Node[" << getIndex() << "] for frame with seq_num= [" << ackExpected % windowSize << "]" << endl;

    EV << "Time out event at time [" << simTime() << "], at Node[" << getIndex() << "] for frame with seq_num= [" << ackExpected % windowSize << "]" << endl;

}

void Node::printSendingMessage(MyMessage_Base* message, int bitToModify, std::string lossMsg, int dup, int delay,int m, int nextFrameToSendTemp) {
    omnetpp::simtime_t time = simTime() + par("PT").doubleValue() * (m + 1 - nextFrameToSendTemp);
    if(dup == 2) {
        time  += par("DD").doubleValue();
    }
    std::bitset<8> trailer(message->getTrailer());
    fout <<  "At time [" << time << "]," <<
        " Node[" << getIndex() << "] [sent] frame with seq_num=[" << message->getHeader() << "] and payload=[" << message->getPayload() << "]" <<
        " and trailer=["<< trailer<< "] , Modified [" << bitToModify << "] "
        ", Lost [" << lossMsg << "], Duplicate [" << dup << "], "
        "Delay [" << (delay ? par("ED").doubleValue() : 0) << "]. "<< endl;

    EV <<  "At time [" << time << "]," <<
        " Node[" << getIndex() << "] [sent] frame with seq_num=[" << message->getHeader() << "] and payload=[" << message->getPayload() << "]" <<
        " and trailer=["<< trailer<< "] , Modified [" <<bitToModify << "] "
        ", Lost [" << lossMsg << "], Duplicate [" << dup << "], "
        "Delay [" << (delay ? par("ED").doubleValue() : 0) << "]. "<< endl;
}

void Node::printSendingReceiverMessage(std::string payload, std::string lossMsg, std::string isAck, int number) {
    EV << "At time [" << simTime() + par("PT").doubleValue() << "], Node[" << getIndex() << "] Sending [" << isAck
            << "] with number [" << number << "] ,loss [" << lossMsg << "] the original message [" << payload << "]." << endl;

    fout << "At time [" << simTime() + par("PT").doubleValue() << "], Node[" << getIndex() << "] Sending [" << isAck
            << "] with number [" << number << "] ,loss [" << lossMsg << "] the original message [" << payload << "]." << endl;

}


std::vector<std::bitset<8>> stringToVector(std::string message)
{
    std::vector<std::bitset<8>> bitsVector;

    bitsVector.push_back(std::bitset<8>('$'));
    for (int i = 0; i < message.size(); i++)
    {
        //if it is flag or escape
        if (message[i] == '$' || message[i] == '/')
        {
            bitsVector.push_back(std::bitset<8>('/'));
        }

        bitsVector.push_back(std::bitset<8>(message[i]));
    }
    bitsVector.push_back(std::bitset<8>('$'));

    return bitsVector;
}


char Node::calculateChecksum(std::string data) {
    std::stringstream hexString;
    hexString << std::hex;

    // Convert characters to hexadecimal string
    for (char ch : data) {
        hexString << std::setw(2) << std::setfill('0') << static_cast<int>(ch);
    }

    std::string paddedHexString = hexString.str();

    std::string sumHex = "00";

    for (size_t i = 0; i < paddedHexString.length(); i += 2) {
        std::string group = paddedHexString.substr(i, 2);
        std::stringstream ss;
        int sum = std::stoi(sumHex, nullptr, 16) + std::stoi(group, nullptr, 16);
        ss << std::hex << sum;
        sumHex = ss.str();
        // Ensure the result is 2 characters
        if (sumHex.length() > 2) {
            std::string lastDigit = sumHex.substr(0,1);
            std::stringstream ss;
            sumHex = sumHex.substr(1, sumHex.size());
            int sum = std::stoi(sumHex, nullptr, 16) + std::stoi(lastDigit, nullptr, 16);
            ss << std::hex << sum;
            sumHex = ss.str();
        }
    }

    std::string onesComplement = calculateOnesComplement(sumHex);
    onesComplement = onesComplement.substr(6,8);
    int intValue = std::stoi(onesComplement, nullptr, 16);

    return static_cast<char>(intValue);
}

std::string Node::calculateOnesComplement(const std::string& hexString) {
    // Convert the hexadecimal string to an integer
    std::stringstream ss(hexString);
    int hexValue;
    ss >> std::hex >> hexValue;
    int onesComplementValue = ~hexValue;
    std::stringstream result;
    result << std::hex << onesComplementValue;

    return result.str();
}

bool Node::checkMessage(MyMessage_Base* message)
{
    std::stringstream hexString;
    hexString << std::hex;

    std::stringstream hexa;
    hexa << std::hex;
    std::string data = message->getPayload();
    for (char ch : data) {
        hexString << std::setw(2) << std::setfill('0') << static_cast<int>(ch);
    }

    std::string paddedHexString = hexString.str();

    hexa << std::setw(2) << std::setfill('0') << static_cast<int>(message->getTrailer());
    std::string sumHex = hexa.str();
//    EV << "message->getTrailer() = " << sumHex << endl;
    sumHex = sumHex.substr(sumHex.length()-2, sumHex.length());

    for (size_t i = 0; i < paddedHexString.length(); i += 2) {
        std::string group = paddedHexString.substr(i, 2);
        std::stringstream ss;
        int sum = std::stoi(sumHex, nullptr, 16) + std::stoi(group, nullptr, 16);
        ss << std::hex << sum;
        sumHex = ss.str();
    }
    if (sumHex.length() > 2) {
        std::string lastDigit = sumHex.substr(0,1);
        std::stringstream ss;
        sumHex = sumHex.substr(1, sumHex.size());
        int sum = std::stoi(sumHex, nullptr, 16) + std::stoi(lastDigit, nullptr, 16);
        ss << std::hex << sum;
        sumHex = ss.str();
    }
//    EV << "sumHex = " << sumHex << endl;
    return (sumHex == "ff");
}


void Node::initialize()
{
    windowSize = par("WS").intValue();

    nbuffered = 0;
    nextFrameToSend = 0;
    ackExpected = 0;
    frameExpected = 0;

    std::string outputFileName = "output" + std::to_string(getIndex()) + ".txt";
    fout.open(outputFileName);

}

void Node::sendingMessageHandler(MyMessage_Base *message, const std::bitset<4> currentErrors, int &m, int &nextFrameToSendTemp)
{
    int modify = currentErrors[3] ;
    int loss = currentErrors[2];
    int dup = currentErrors[1] ;
    int delay = currentErrors[0] ;


    double time = 0;
    int bitToModify = -1;
    std::string lossMsg = "No";
    time = par("PT").doubleValue()*(m + 1 - nextFrameToSendTemp) + par("TD").doubleValue() ;

    std::string sendMsg;
    std::vector<std::bitset<8>> currentMsgbits;
    currentMsgbits = stringToVector(originalMsg[m]);

    if (loss)
    {
        lossMsg = "Yes";
    }
    if (modify)
    {
        bitToModify = int(uniform(0, currentMsgbits.size() * 8));
        currentMsgbits[bitToModify/8].flip(bitToModify%8);
    }
    if(delay)
    {
        time += par("ED").doubleValue();
    }

    for(int i =0; i<currentMsgbits.size();i++)
    {
       sendMsg += (char)currentMsgbits[i].to_ulong();
    }
    message->setPayload(sendMsg.c_str());

    printSendingMessage(message, bitToModify, lossMsg, dup, delay, m, nextFrameToSendTemp);
    if(dup)
    {
        MyMessage_Base *dupFrame = message->dup();
        std::bitset<8> trailer2(dupFrame->getTrailer());
        printSendingMessage(dupFrame, bitToModify, lossMsg, 2, delay, m, nextFrameToSendTemp);
        sendDelayed(dupFrame, (time+par("DD").doubleValue()), "out");
    }

    if(!loss)
    {
        sendDelayed(message, time , "out");
    }
}

void Node::handleMessage(cMessage *msg)
{
//    If the Master is the sender, the sender start sending messages:-
    if (strcmp(msg->getName(), "GO!") == 0)
    {
        int me = getIndex();
        if(me == 0 )
        {
           readFile("input0.txt");
        }
        else
        {
            readFile("input1.txt");
        }

        if (windowSize > originalMsg.size())
        {
            windowSize = originalMsg.size();
        }

        for (int m = 0; m < windowSize; m++)
        {
            MyMessage_Base *message = new MyMessage_Base();

            std::bitset<4> currentErrors = errors[m];

            printReadingMessage(m, 0);

            message->setHeader(m);
            message->setTrailer(calculateChecksum(framing(originalMsg[m])));
            message->setFrameType(2);


            cMessage *timeoutEvent = new cMessage("timeoutEvent");
            timeoutEvents.push_back(timeoutEvent);
            scheduleAt(simTime() + par("TO").doubleValue() + (par("PT").doubleValue() * (m + 1)), timeoutEvents[m]);

            nbuffered += 1;
            nextFrameToSend = nextFrameToSend + 1;

            int next = 0;
            sendingMessageHandler(message, currentErrors, m, next);
        }
    }
//    If it's timeout message, so re-send messages again:-
    else if (strcmp(msg->getName(), "timeoutEvent") == 0)
    {
        printTimeoutMessage(ackExpected);

        nextFrameToSend = ackExpected;
        int nextFrameToSendTemp = nextFrameToSend;
        nbuffered = 0;
        for (int m = nextFrameToSendTemp; m < nextFrameToSendTemp + (windowSize) && m < originalMsg.size(); m++)
        {
            MyMessage_Base *message = new MyMessage_Base();

            message->setHeader(m % windowSize);
            message->setTrailer(calculateChecksum(framing(originalMsg[m])));
            message->setFrameType(2);

            nbuffered += 1;
            nextFrameToSend = nextFrameToSend + 1;

            cancelEvent(timeoutEvents[m]);
            scheduleAt(simTime() + par("TO").doubleValue() + (par("PT").doubleValue() * (m + 1 - nextFrameToSendTemp)), timeoutEvents[m]);
            if (m == nextFrameToSendTemp)
            {
                std::bitset<4> currentErrors = 0000;
                sendingMessageHandler(message, currentErrors, m, nextFrameToSendTemp);
            }
            else
            {
                sendingMessageHandler(message, errors[m], m, nextFrameToSendTemp);
            }
        }
    }
//    If the message is from sender or receiver:-
    else if (typeid(*msg) == typeid(MyMessage_Base)) {
        MyMessage_Base *message = check_and_cast<MyMessage_Base *>(msg);
        // If it's a ACK message, so cancel timeout event:-
        if (message->getFrameType() == 1 && message->getAckNack() == (ackExpected + 1) % (windowSize)){
            nbuffered = nbuffered - 1;
            cancelEvent(timeoutEvents[ackExpected]);
            ackExpected = ackExpected + 1;
        }
        // If there is a place in the buffer, so send more messages:-
        if (!(message->getFrameType() == 2) && nbuffered < windowSize)
        {
            if (nextFrameToSend < originalMsg.size())
            {
                int nextFrameToSendTemp = nextFrameToSend;
                for (int m = nextFrameToSendTemp; m < nextFrameToSendTemp + (windowSize - nbuffered) && m < originalMsg.size(); m++)
                {
                    MyMessage_Base *newMessage = new MyMessage_Base();

                    std::bitset<4> currentErrors = errors[m];
                    printReadingMessage(m, nextFrameToSendTemp);

                    newMessage->setHeader(m % windowSize);
                    newMessage->setTrailer(calculateChecksum(framing(originalMsg[m])));
                    newMessage->setFrameType(2);

                    nbuffered += 1;
                    nextFrameToSend = nextFrameToSend + 1;

                    cMessage *timeoutEvent = new cMessage("timeoutEvent");
                    timeoutEvents.push_back(timeoutEvent);
                    scheduleAt(simTime() + par("TO").doubleValue() + (par("PT").doubleValue() * (m + 1 - nextFrameToSendTemp)), timeoutEvents[m]);

                    sendingMessageHandler(newMessage, currentErrors, m, nextFrameToSendTemp);
                }
            }
        }
        // If it's a data message, so check if it's true or not:-
        if (message->getFrameType() == 2 && message->getHeader() == frameExpected % (windowSize))
        {
            MyMessage_Base *newMessage = message->dup();
            bool isTrue = checkMessage(message);
            bool isLost = uniform(0, 100) < (int)par("LP").doubleValue();

            int number = 0;
            std::string isAck = "ACK";
            std::string lossMsg = "No";
            newMessage->setFrameType(1);

            if (isTrue && !isLost) {
                frameExpected = frameExpected + 1;
                number = frameExpected % windowSize;
                newMessage->setAckNack(number);
            }
            else if (isTrue && isLost) {
                number = (frameExpected + 1) % windowSize;
                newMessage->setAckNack(number);
                newMessage->setFrameType(0);
                lossMsg = "Yes";
            }
            else if (!isTrue && !isLost) {
                number = frameExpected % windowSize;
                newMessage->setAckNack(number);
                newMessage->setFrameType(0);
                isAck = "NACK";
            }
            else if (!isTrue && isLost) {
                newMessage->setAckNack(frameExpected % windowSize);
                number = frameExpected % windowSize;
                lossMsg = "Yes";
                isAck = "NACK";
            }
            printSendingReceiverMessage(deFraming(newMessage->getPayload()), lossMsg, isAck, number);

            if (!isLost)
                sendDelayed(newMessage,  par("PT").doubleValue() + par("TD").doubleValue(), "out");
        }
        cancelAndDelete(message);
    }
}

void Node::finish()
{
    EV << "End of simulation" << endl;

    for (auto messages : timeoutEvents)
    {
        cancelAndDelete(messages);
    }
    fout.close();
}

