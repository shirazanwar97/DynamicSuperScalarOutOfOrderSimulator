#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "sim_proc.h"
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;
/*  argc holds the number of command line arguments
    argv[] holds the commands themselves

    Example:-
    sim 256 32 4 gcc_trace.txt
    argc = 5
    argv[0] = "sim"
    argv[1] = "256"
    argv[2] = "32"
    ... and so on
*/

vector<Instruction> decodeReg, renameReg, registerReadReg, dispatchReg, issueQueueReg, executeReg, writebackReg, retireReg;
vector<ROBEntry> ROBBuffer;
vector<RMTEntry> RMT;

void printRegData(vector<Instruction> reg){
    for(size_t i=0; i<reg.size(); i++){
        cout << "seqNo = " << reg[i].getSeqNo() << endl;
        cout << "is Valid = " << reg[i].isValid() << endl;        
        // cout << "address = " << reg[i].getAddress() << endl;
        // cout << "operation type = " << reg[i].getOperationType() << endl;
        cout << "destReg = " << reg[i].getDestReg() << endl;
        cout << "source 1 = " << reg[i].getSource1() << endl;
        cout << "source 2 = " << reg[i].getSource2() << endl;

        // cout<< "durationInDE" << reg[i].getDurationInDE() << endl;
        // cout<< "durationInRename" << reg[i].getDurationInRenameReg() << endl;

        cout << "source1 ROB tag" << reg[i].getS1ROBTag() << endl;
        cout << "source2 ROB tag" << reg[i].getS2ROBTag() << endl;
        cout << "dest ROB tag" << reg[i].getDestROBTag() << endl;
    }
}

bool isRegEmpty(vector<Instruction> reg){
    for(size_t i=0; i< reg.size(); i++){
        if(reg[i].isValid())
            return false; //an element in the regWidth is VALID --> not empty
    }
    return true; //no element in the regWidth is VALID --> empty!
}

int getIQAvailableEntries(int issueQueueSize){
    int entries = 0;
    for(size_t i=0; i< issueQueueReg.size(); i++){
        if(issueQueueReg[i].isValid()){
            entries++;
        } //not valid entry --> empty
    }
    return (issueQueueSize - entries -1);
}

void transferFromRegToReg(vector<Instruction> &fromReg, vector<Instruction> &toReg, int index){
        toReg[index].setInstructionSpecific(fromReg[index].getOperationType(), fromReg[index].getDestReg(), fromReg[index].getSource1(), fromReg[index].getSource2(), fromReg[index].getAddress(), fromReg[index].getSeqNo());
        toReg[index].setCycleBeginCount(fromReg[index].getCycleBeginCountForDE(), fromReg[index].getCycleBeginCountForRenameReg(), fromReg[index].getCycleBeginCountForRegRead(), fromReg[index].getCycleBeginCountForIQ(), 
                                                    fromReg[index].getCycleBeginCountForExecute(), fromReg[index].getCycleBeginCountForWriteBack(), fromReg[index].getCycleBeginCountForRetire());
        toReg[index].setDurationCounter(fromReg[index].getDurationInDE(), fromReg[index].getDurationInRenameReg(), fromReg[index].getDurationInRegRead(), fromReg[index].getDurationInIQ(), 
                                                    fromReg[index].getDurationInExecute(), fromReg[index].getDurationInWriteBack(), fromReg[index].getDurationInRetire());
        toReg[index].setROBTag(fromReg[index].getDestROBTag(),fromReg[index].getS1ROBTag(),fromReg[index].getS2ROBTag(), fromReg[index].getS1Ready(), fromReg[index].getS2Ready());
}

bool ifROBHasEnoughSpace(int tail, int robSize, vector<Instruction> reg) {
    if((robSize - tail - 1) >= ((int)reg.size())) //doubtful for -1 part
            return true;
    return false;
}

int updateRegROBTag(int RMTIndexToCheck){
    // cout << "RMTIndexToCheck != (-1) = " << (RMTIndexToCheck != (-1)) << endl;
    // cout << "RMT[RMTIndexToCheck].valid = " << RMT[RMTIndexToCheck].valid << endl;
    if((RMTIndexToCheck != (-1)) && RMT[RMTIndexToCheck].valid){
        return RMT[RMTIndexToCheck].ROBTag;
    }
    return -1; //if no ROBTag --> value is supposed to be read from ARF.
}

void Retire(){
//<seq_no> fu{<op_type>} src{<src1>,<src2>} dst{<dst>} FE{<begin-cycle>,<duration>} DE{…} RN{…} RR{…} DI{…} IS{…} EX{…} WB{…} RT{…}
    
}

void Writeback(){

}

void Execute(){

}

void Issue(int cycleCount, int iqSize, int width){
    if(!isRegEmpty(issueQueueReg)){ //if issue queue has data
        int uptoWidthCounter = 0;
        vector<Instruction> tempVector = vector<Instruction>(); 
        for(size_t i=0; i< issueQueueReg.size(); i++){
            if(issueQueueReg[i].isValid() && issueQueueReg[i].getS1Ready() && issueQueueReg[i].getS2Ready() && uptoWidthCounter<width){
                tempVector.push_back(issueQueueReg[i]);
                uptoWidthCounter++;
            }
        }
        printRegData(tempVector);
        if(uptoWidthCounter == width){ //number of instructions in tempVector not upto width
            //transfer temp vector to execute list
            for(int i=0;i<width;i++){
                executeReg.push_back(tempVector[i]);
                executeReg[i].setCycleBeginCountForIQ(cycleCount);
                executeReg[i].incrementDurationForIQ();
                for(size_t j=0; j<issueQueueReg.size(); j++){
                    if(tempVector[i].getSeqNo() == issueQueueReg[j].getSeqNo()){
                        issueQueueReg.erase(issueQueueReg.begin() + j);
                        break;
                    }
                }
            }
        }
        else {
            for(size_t i=0; i<dispatchReg.size(); i++){
                issueQueueReg[i].incrementDurationForIQ();
            } //wrong!
            //increase iQ duration count
        }
    }
}

void Dispatch(int cycleCount, int width, int iqSize){
    if(!isRegEmpty(dispatchReg)){
        if(getIQAvailableEntries(iqSize) >= width){
            for(size_t i=0; i<dispatchReg.size(); i++){
                issueQueueReg.push_back(dispatchReg[i]);
                issueQueueReg[i].setCycleBeginCountForDispatch(cycleCount);
                issueQueueReg[i].incrementDurationForDispatch();
                dispatchReg[i].clear();
            }
        }
        else {
            for(size_t i=0; i<dispatchReg.size(); i++){
                dispatchReg[i].incrementDurationForDispatch();
            }
        }
    }
}

void RegRead(int cycleCount){
    if(!isRegEmpty(registerReadReg)){ //if renameReg is NOT empty --> contains a bundle
        if(isRegEmpty(dispatchReg)){
        //proccess something
        for(size_t i=0; i<registerReadReg.size(); i++) {
            if((registerReadReg[i].getS1ROBTag() == -1) /*&& (RMT[registerReadReg[i].getSource1()].valid != 1)*/) //RMT.valid should be false
                registerReadReg[i].setS1Ready(true);
            if((registerReadReg[i].getS2ROBTag() == -1) /*&& (RMT[registerReadReg[i].getSource2()].valid != 1)*/) //RMT.valid should be false
                registerReadReg[i].setS2Ready(true);
        }
        //transfer data
        for(size_t i=0; i<registerReadReg.size(); i++){
                transferFromRegToReg(registerReadReg, dispatchReg, i);
                // dispatchReg[i].setROBTag(registerReadReg[i].getDestROBTag(), registerReadReg[i].getSource1(), registerReadReg[i].getSource2(),registerReadReg[i].getS1Ready(), registerReadReg[i].getS2Ready());
                // dispatchReg[i].setSourcesReady(registerReadReg[i].getS1Ready(), registerReadReg[i].getS1Ready());
                dispatchReg[i].setCycleBeginCountForRenameReg(cycleCount);
                dispatchReg[i].incrementDurationForRegRead();
                registerReadReg[i].clear();
            }
        }
        else{
            for(size_t i=0; i<registerReadReg.size(); i++){
                renameReg[i].incrementDurationForRegRead();
            }
        }
    }
}

void Rename(int cycleCount,int &robTail,int robSize){
    if(!isRegEmpty(renameReg)){ //if renameReg is NOT empty --> contains a bundle
        if(isRegEmpty(registerReadReg) && ifROBHasEnoughSpace(robTail, robSize, renameReg)){
                //1. create entry in ROB
            for(size_t i=0; i<(renameReg.size()); i++){
                ROBBuffer[robTail].valid = true;
                ROBBuffer[robTail].ready = false;
                ROBBuffer[robTail].dst = renameReg[i].getDestReg(); //also doing this even if dest is -1
                ROBBuffer[robTail].pc = renameReg[i].getSeqNo();

            // 2. rename sources
                // cout << "for instructionCount = " << renameReg[i].getSeqNo() <<" setting ROB tags as \n1. destROBTag = " << robTail << "\n 2. S1ROBTag = " << updateRegROBTag(renameReg[i].getSource1()) << "\n 3. S2ROBTag = "<< updateRegROBTag(renameReg[i].getSource2()) << endl;
                renameReg[i].setS1Tag(updateRegROBTag(renameReg[i].getSource1()));
                renameReg[i].setS2Tag(updateRegROBTag(renameReg[i].getSource2()));

                if(renameReg[i].getSource1() != (-1) && (RMT[renameReg[i].getSource1()].valid != 1)){
                    renameReg[i].setS1Ready(true);
                }
                if(renameReg[i].getSource2() != (-1) && (RMT[renameReg[i].getSource2()].valid != 1)){
                    renameReg[i].setS2Ready(true);
                }
            //3. rename destination
                //a. set dest tag
                renameReg[i].setDestTag(robTail);
                //b. add/update an entry in the RMT
                if(renameReg[i].getDestReg()!=(-1)){ //if -1, then do nothing as it has no dest Register
                    RMT[renameReg[i].getDestReg()].valid = 1;
                    RMT[renameReg[i].getDestReg()].ROBTag = (robTail);
                }
                robTail++;
            }
            for(size_t i=0; i< renameReg.size(); i++){
                transferFromRegToReg(renameReg, registerReadReg, i);
                // registerReadReg[i].setROBTag(renameReg[i].getDestROBTag(), renameReg[i].getS1ROBTag(), renameReg[i].getS2ROBTag(),renameReg[i].getS1Ready(), renameReg[i].getS2Ready());
                registerReadReg[i].setCycleBeginCountForRegRead(cycleCount);
                registerReadReg[i].incrementDurationForRenameReg();
                renameReg[i].clear();
            }
        }
        else{ //if RenameReg is not empty --> stall data in DecodeReg
            for(size_t i=0; i<renameReg.size(); i++){
                renameReg[i].incrementDurationForRenameReg();
            }
        }
    }
}

void Decode(int cycleCount){
    if(!isRegEmpty(decodeReg)){ //if decodeReg is NOT empty --> contains a bundle
        if(isRegEmpty(renameReg)){ //if renameReg is empty
            for(size_t i=0; i< decodeReg.size(); i++){
                transferFromRegToReg(decodeReg, renameReg, i);
                renameReg[i].increamentDurationForDE();
                decodeReg[i].clear();
            }
        }
        else { //if RenameReg is not empty --> stall data in DecodeReg
            for(size_t i=0; i<decodeReg.size(); i++){
                decodeReg[i].increamentDurationForDE();
            }
        }
    }
}

void Fetch(FILE* fp, uint64_t pc, int op_type, int dest, int src1, int src2, int regWidth, int &instructionCount, int &cycleCount){
    if(!feof(fp) && isRegEmpty(decodeReg)){
        for(int i=0; i< regWidth; i++){
            if(fscanf(fp, "%lx %d %d %d %d", &pc, &op_type, &dest, &src1, &src2) != EOF){
                instructionCount++;
                printf("%lx %d %d %d %d\n", pc, op_type, dest, src1, src2); // actually add the values to DE register
                decodeReg[i].setForDEReg(op_type, dest, src1, src2, pc, instructionCount);
                decodeReg[i].setCycleBeginCountForDE(cycleCount);
            }
        }
    }
}

bool isEndOfPipeline(int width, int iq_size){ //returns if pipeline is empty
    for(int i=0; i<width; i++){
        if(retireReg[i].isValid())
            return false;
        if(registerReadReg[i].isValid())
            return false;
        if(renameReg[i].isValid())
            return false;
        if(decodeReg[i].isValid())
            return false;
        if(dispatchReg[i].isValid())
            return false;
        }
    for(int i=0; i<(5*width); i++){
        if(executeReg[i].isValid())
            return false;
        if(writebackReg[i].isValid())
            return false;
    }
    for(size_t i=0; i<issueQueueReg.size(); i++){ //dont put this to iq_size after changing IQ vector initialisation
        if(issueQueueReg[i].isValid())
            return false;
    }
    return true; //pipeline is empty;
}

bool Advance_Cycle(int &cycleCount, FILE* fp, int width, int iq_size){
    cycleCount++;
    if(feof(fp) && isEndOfPipeline(width, iq_size))
        return false;
    return true; 
}

void initialiseRegisters(int robSize, int issueQueueSize, int RegSize, int RMTSize){
    ROBBuffer = vector<ROBEntry>(robSize);
    RMT = vector<RMTEntry>(RMTSize);
    decodeReg = vector<Instruction>(RegSize);
    renameReg = vector<Instruction>(RegSize);
    registerReadReg = vector<Instruction>(RegSize);
    dispatchReg = vector<Instruction>(RegSize);
    // issueQueueReg = vector<Instruction>(issueQueueSize);
    executeReg = vector<Instruction>();
    writebackReg = vector<Instruction>(RegSize);
    retireReg = vector<Instruction>(RegSize);
}

int main (int argc, char* argv[])
{
    FILE *FP;               // File handler
    char *trace_file;       // Variable that holds trace file name;
    proc_params params;       // look at sim_bp.h header file for the the definition of struct proc_params
    int op_type = 0, dest = 0, src1 = 0, src2 = 0;  // Variables are read from trace file
    uint64_t pc = 0; // Variable holds the pc read from input file
    
    static int noOfRegisters = 68;
    int headROB = 0, tailROB = 0;
    int instructionCount = 0, cycleCount = 0;

    if (argc != 5)
    {
        printf("Error: Wrong number of inputs:%d\n", argc-1);
        exit(EXIT_FAILURE);
    }
    
    params.rob_size     = strtoul(argv[1], NULL, 10);
    params.iq_size      = strtoul(argv[2], NULL, 10);
    params.width        = strtoul(argv[3], NULL, 10);
    trace_file          = argv[4];
    printf("rob_size:%lu "
            "iq_size:%lu "
            "width:%lu "
            "tracefile:%s\n", params.rob_size, params.iq_size, params.width, trace_file);
    // Open trace_file in read mode
    FP = fopen(trace_file, "r");
    
    if(FP == NULL)
    {
        // Throw error and exit if fopen() failed
        printf("Error: Unable to open file %s\n", trace_file);
        exit(EXIT_FAILURE);
    }

    initialiseRegisters(params.rob_size, params.iq_size, params.width, noOfRegisters);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // The following loop just tests reading the trace and echoing it back to the screen.
    //
    // Replace this loop with the "do { } while (Advance_Cycle());" loop indicated in the Project 3 spec.
    // Note: fscanf() calls -- to obtain a fetch bundle worth of instructions from the trace -- should be
    // inside the Fetch() function.
    //
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    // while(fileContent != EOF)
    //     printf("%lx %d %d %d %d\n", pc, op_type, dest, src1, src2); //Print to check if inputs have been read correctly

    

    do {
        Retire();
        Writeback();
        Execute();
        Issue(cycleCount, params.iq_size, params.width);
        Dispatch(cycleCount, params.width, params.iq_size); 
        RegRead(cycleCount);
        Rename(cycleCount, tailROB, params.rob_size); 
        Decode(cycleCount); 
        Fetch(FP, pc, op_type, dest, src1, src2, params.width, instructionCount, cycleCount);
        } while (Advance_Cycle(cycleCount, FP, params.width, params.iq_size));

    // cout << "cycleCount outside the do-while loop = " << cycleCount << endl;
    // // Advance_Cycle performs several functions. First, it advances the simulator cycle. Second, when it becomes known that the pipeline is empty AND the trace is depleted, the function returns “false” to terminate the loop.

    // cout << "printing ROB ---------------------------------------" << endl;
    // for(size_t i=0; i<params.rob_size; i++){
    //     if(ROBBuffer[i].valid)
    //         cout << "robNumber" << i <<"\t pc = " << ROBBuffer[i].pc << "\t dst = " << ROBBuffer[i].dst << "\t ready = " << ROBBuffer[i].ready << endl;
    // }
    // cout << "printing RMT ---------------------------------------" << endl;
    // for(int i=0; i<noOfRegisters; i++){
    //     if(RMT[i].valid)
    //         cout << "RMTNumber" << i <<"\t RMTTag = " << RMT[i].ROBTag << "\t valid = " << RMT[i].valid << endl;
    // }
    // cout << "printing Decode Reg data----------------------------------------" << endl;
    // printRegData(decodeReg);
    // cout << "----------------------------------------------------------------" << endl;
    // cout << "printing rename Reg data----------------------------------------" << endl;
    // printRegData(renameReg);
    // cout << "----------------------------------------------------------------" << endl;
    // cout << "printing regRead Reg data----------------------------------------" << endl;
    // printRegData(registerReadReg);
    // cout << "----------------------------------------------------------------" << endl;
    // cout << "printing dispatch data----------------------------------------" << endl;
    // printRegData(dispatchReg);
    // cout << "----------------------------------------------------------------" << endl;    
    // cout << "printing issueQueue data----------------------------------------" << endl;
    // printRegData(issueQueueReg);
    // cout << "----------------------------------------------------------------" << endl; 
    // cout << "printing execute data----------------------------------------" << endl;
    // printRegData(executeReg);
    // cout << "----------------------------------------------------------------" << endl;        
    return 0;
}