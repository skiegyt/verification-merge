//
//  sensor.cpp
//  verification-merge
//
//  Created by Shou-pon Lin on 5/1/14.
//  Copyright (c) 2014 Shou-pon Lin. All rights reserved.
//

#include "sensor.h"

Sensor::Sensor( Lookup* msg, Lookup* mac ):StateMachine(msg, mac)
{
    setId(machineToInt(SENSOR_NAME)) ;
    reset();
}

int Sensor::transit(MessageTuple *inMsg, vector<MessageTuple *> &outMsgs, bool &high_prob,
                      int startIdx)
{
    outMsgs.clear();
    high_prob = true;
    string msg = IntToMessage(inMsg->destMsgId());
    string src = IntToMachine(inMsg->subjectId());
    
    if( startIdx != 0 )
        return -1;
    switch (_state) {
        case 0:
            if( msg == SENSORON ) {
                assert(src == MERGE_NAME);
                outMsgs.push_back(createMsg(inMsg, TRBP_NAME, REQUIRE));
                _state = 1;
                return 3;
            }
            else if( msg == DEADLINE )
                return 3;
            else if (msg == TRBPFAIL)
                return 3;
            else
                return -1;
            break;
        case 1:
            if( msg == SENSOROFF ) {
                assert(src == MERGE_NAME) ;
                _state = 0 ;
                return 3;
            }
            else if (msg == TRBPFAIL) {
                assert(src == TRBP_NAME);
                _state = 0;
                return 3;
            }
            else if( msg == DEADLINE )
                return 3;
            else
                return -1;
            break;
        case 2:
            if( msg == SENSOROFF ) {
                assert(src == MERGE_NAME) ;
                _state = 0 ;
                return 3;
            }
            else if (msg == TRBPFAIL) {
                assert(src == TRBP_NAME);
                _state = 0;
                return 3;
            }
            else if( msg == DEADLINE )
                return 3;
            else
                return -1;
            break;
        default:
            return -1;
            break;
    }
}

int Sensor::nullInputTrans(vector<MessageTuple *> &outMsgs, bool &high_prob, int startIdx)
{
    outMsgs.clear() ;
    if( _state != 1 )
        return -1;
    high_prob = true;
    switch (_state) {
        case 0:
            return -1;
        case 1:
            if( startIdx == 1 ) {
                outMsgs.push_back(createMsg(0, MERGE_NAME, GAPREADY));
                _state = 2;
                return 3;
            }
        case 2:
            if (startIdx == 0) {
                outMsgs.push_back(emergency(MERGE_NAME));
                outMsgs.push_back(emergency(FRONT_NAME));
                outMsgs.push_back(emergency(BACK_NAME));
                _state = 0;
                high_prob = false;
                return 1;
            }
            else if (startIdx == 1) {
                outMsgs.push_back(gapTaken(MERGE_NAME));
                outMsgs.push_back(gapTaken(FRONT_NAME));
                outMsgs.push_back(gapTaken(BACK_NAME));
                _state = 0;
                high_prob = false;
                return 2;
            }
            else if (startIdx == 2) {
                outMsgs.push_back(inconsistent(MERGE_NAME));
                outMsgs.push_back(inconsistent(FRONT_NAME));
                outMsgs.push_back(inconsistent(BACK_NAME));
                _state = 0;
                high_prob = false;
                return 3;  
            }
            else {
                return -1;
            }
            break;
        default:
            return -1;
            break;
    }
}

MessageTuple* Sensor::emergency(const string& dest)
{
    return createMsg(0, dest, EMERGENCY);
}

MessageTuple* Sensor::gapTaken(const string& dest)
{
    return createMsg(0, dest, GAPTAKEN);
}

MessageTuple* Sensor::inconsistent(const string& dest)
{
    return createMsg(0, dest, INCONSISTENT);
}

MessageTuple* Sensor::createMsg(MessageTuple *inMsg, const string& dest, string msg)
{
    if( inMsg == 0 )
        return new MessageTuple(0, machineToInt(dest),
                                0, messageToInt(msg), macId()) ;
    else
        return new MessageTuple(inMsg->srcID(), machineToInt(dest),
                                inMsg->srcMsgId(), messageToInt(msg), macId()) ;
}