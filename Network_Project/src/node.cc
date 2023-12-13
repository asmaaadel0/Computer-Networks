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



void Node::ReadFile(const char *filename)
{
    std::ifstream fstream;
    std::string line;

    fstream.open(filename, std::ifstream::in);

    if (fstream)
    {
        while (getline(fstream, line))
        {
            errors.push_back(std::bitset<4>((line.substr(0, 4)))); // or atoi??  //or stoi??
            originalMsg.push_back(line.substr(5, line.length() - 5));
//            std::string liny = line;
//            std::cout << liny <<endl;
//            fout << line << endl;
        }

        //for debugging:-
//        for (int i = 0; i < originalMsg.size(); i++)
//        {
//            std::cout << errors[i] << originalMsg[i] <<endl;
//        }
    }
    else
    {
        throw cRuntimeError("Error opening file");

    }
    fstream.close();

}

void Node::initialize()
{
    // TODO - Generated method body
    //variables initializations:-
//    windoSize = par("WS").intValue();
    windoSize = 6;
//    ReadFile("input0.txt");
    FrameSending(new MyMessage_Base());

    //system outs
    //m.s: we open it in the initialization for both nodes as both will write in the same file
    fout.open("output.txt");

}
//this function return the payload after framing and checksum adding , in the form of std::vector<std::bitset<8>>
std::vector<std::bitset<8>> PayloadBits(std::string message)
{
    std::vector<std::bitset<8>> bitsVector;
    std::bitset<8> checkSum;

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

void Node::FrameSending(MyMessage_Base *message /*, const char *code, int modifyIndex  , int &index, int &nextFrameToSendTemp*/ )
{
    //later: ektb fe el external file b el format el mtlob
    for (int m = 0; m < windoSize; m++)
    {
        MyMessage_Base *dupFrame;

        std::string sendMsg;
        std::vector<std::bitset<8>> currentMsgbits;

        std::string currentMsg = originalMsg[m]; ///------------/later : change this m
        std::bitset<4> currentErrors = errors[m]; ///------------/later : change this m

        //system out
        fout << "At time [" << simTime() + par("PT").doubleValue() << "]," << " Node[" << getIndex()
                << "] , Introducing channel error with code =[" << errors[m] << "] ." << endl;
        EV << "At time [" << simTime() + par("PT").doubleValue() << "]," << " Node[" << getIndex()
        << "] , Introducing channel error with code =[" << errors[m] << "] ." << endl;
//        std::cout << currentErrors << "/////" << currentMsg<< currentErrors[0] <<endl;

        int modify = currentErrors[3] ;
        int loss = currentErrors[2];
        int dup = currentErrors[1] ;
        int delay = currentErrors[0] ;

        int bitToModify = -1;
        double timeDelay = 0;
        std::string lossMsg = "No";

        timeDelay = par("PT").doubleValue()+ par("TD").doubleValue() ; //* (index + 1 - nextFrameToSendTemp) ??

        currentMsgbits = PayloadBits(currentMsg);
        if (loss)
        {
            lossMsg = "Yes";
        }
        if (modify)
        {

            //NOTE: currentMsgbits.size() * 8 : as the size of the vector "currentMsgbits" = number of Bytes in the frame
            bitToModify = int(uniform(0, currentMsgbits.size() * 8));
            currentMsgbits[bitToModify/8].flip(bitToModify%8);

        }

        //prepare the message to be send:-
        for(int i =0; i<currentMsgbits.size();i++)
        {
           sendMsg += (char)currentMsgbits[i].to_ulong();
        }
        message->setPayload(sendMsg.c_str());


        if(delay)
        {
            timeDelay += par("ED").doubleValue();

        }

        //system out
        fout <<  "At time [" << simTime() + par("PT").doubleValue() << "]," <<
            " Node[" << getIndex() << "] [sent] frame with seq_num=[" << message->getHeader() << "] and payload=[" << message->getPayload() << "]" <<
            " and trailer=["<< message->getTrailer()<< "] , Modified [" <<bitToModify << "] "
            ", Lost [" << lossMsg << "], Duplicate [" << dup << "], "
            "Delay [" << timeDelay << "]. "<< endl;

        EV <<  "At time [" << simTime() + par("PT").doubleValue() << "]," <<
            " Node[" << getIndex() << "] [sent] frame with seq_num=[" << message->getHeader() << "] and payload=[" << message->getPayload() << "]" <<
            " and trailer=["<< message->getTrailer()<< "] , Modified [" <<bitToModify << "] "
            ", Lost [" << lossMsg << "], Duplicate [" << dup << "], "
            "Delay [" << timeDelay << "]. "<< endl;


        //check on the delay before the duplication so that the time will be increased to timeDelay += par("ED")
        // and both duplications will have this time but with difference =  par("DD")
        if(dup)
        {
            dupFrame = message->dup();
            //NOTE: do not modify the original msh time
//                timeDelay += par("DD").doubleValue();
            //system out
            fout << "At time [" << simTime() + par("PT").doubleValue() << "]," <<
                " Node[" << getIndex() << "] [sent] frame with seq_num=[" << dupFrame->getHeader() << "] and payload=[" << dupFrame->getPayload() << "]" <<
                " and trailer=["<< dupFrame->getTrailer()<< "] , Modified [" <<bitToModify << "] "
                ", Lost [" << lossMsg << "], Duplicate [" << "2" << "], "
                "Delay [" << timeDelay << "]. " << endl;
            EV << "At time [" << simTime() + par("PT").doubleValue() << "]," <<
                            " Node[" << getIndex() << "] [sent] frame with seq_num=[" << dupFrame->getHeader() << "] and payload=[" << dupFrame->getPayload() << "]" <<
                            " and trailer=["<< dupFrame->getTrailer()<< "] , Modified [" <<bitToModify << "] "
                            ", Lost [" << lossMsg << "], Duplicate [" << "2" << "], "
                            "Delay [" << timeDelay << "]. " << endl;



        }

        if(!loss)
        {
    //            sendDelayed(message, timeDelay , "out");
            sendDelayed(message, timeDelay*(m+1) , "out");
            if(dup)
            {
                sendDelayed(dupFrame, (timeDelay + par("DD").doubleValue())*(m+1), "out");
            }
        }

    }
}

void Node::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
    int me = getIndex();
    if(msg->getName() == "GO!")
    {
        if(me ==0 )
        {
           ReadFile("input0.txt");
        }
        else
        {
            ReadFile("input1.txt");
        }

    }
//    else if timeout
//    {
//
//    }
    else
    {
        msg->get
    }
}
