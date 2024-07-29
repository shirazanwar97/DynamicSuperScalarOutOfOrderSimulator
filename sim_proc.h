#ifndef SIM_PROC_H
#define SIM_PROC_H

typedef struct proc_params{
    unsigned long int rob_size;
    unsigned long int iq_size;
    unsigned long int width;
}proc_params;

struct ROBEntry{
    bool valid, ready;
    int dst, exec, mis, pc;
    
    ROBEntry() : valid(false), ready(false), dst(0), exec(0), mis(0), pc(0) {
    }

};

struct RMTEntry{
    bool valid;
    int ROBTag;

    RMTEntry() : valid(false), ROBTag('\0'){
    }
};

class Instruction{
    protected :
        bool valid;
        int operation_type; //0, 1, 2 -->
        int destReg, source1, source2;
        int destROBTag, s1ROBTag, s2ROBTag;
        bool s1Ready, s2Ready;
        bool isDstReady; //for WB stage
        uint64_t address;

        int cycleBeginCountForDE, durationInDE;
        int cycleBeginCountForRenameReg, durationInRenameReg;
        int cycleBeginCountForRegRead, durationInRegRead;
        int cycleBeginCountForDispatch, durationInDispatch;
        int cycleBeginCountForIQ, durationInIQ;
        int cycleBeginCountForExecute, durationInExecute;
        int cycleBeginCountForWriteBack, durationInWriteBack;
        int cycleBeginCountForRetire, durationInRetire;

    // private:
    //     void setInstructionSpecific(int opType, int dest, int s1, int s2, uint64_t address, int instructionCount){
    //         this->valid = true;
    //         this->operation_type = opType;
    //         this->seqNo = instructionCount;
    //         this->destReg = dest;
    //         this->source1 = s1;
    //         this->source2 = s2;
    //         this->address = address;
    //     }

    public :

        int seqNo;
         
        Instruction(){
            valid = false;
            seqNo = -1;
            operation_type = 0; //0, 1, 2 -->
            destReg = source1 = source2 = seqNo = 0;
            destROBTag = s1ROBTag = s2ROBTag =0;
            s1Ready = false, s2Ready = false;
            isDstReady = false; //for WB stage
        }

    // Getter methods---------------------------------------------------------------
        bool isValid(){
            return valid;
        }

        int getOperationType(){
            return operation_type;
        }

        int getDestReg(){
            return destReg;
        }

        int getSource1(){
            return source1;
        }

        int getSource2(){
            return source2;
        }

        int getSeqNo(){
            return seqNo;
        }

        int getDestROBTag(){
            return destROBTag;
        }

        int getS1ROBTag(){
            return s1ROBTag;
        }

        int getS2ROBTag(){
            return s2ROBTag;
        }

        bool getS1Ready(){
            return s1Ready;
        }

        bool getS2Ready(){
            return s2Ready;
        }

        bool isDestinationReady(){
            return isDstReady;
        }

        uint64_t getAddress(){
            return address;
        }

        int getCycleBeginCountForDE() {
            return cycleBeginCountForDE;
        }

        int getDurationInDE() {
            return durationInDE;
        }

        int getCycleBeginCountForRenameReg() {
            return cycleBeginCountForRenameReg;
        }

        int getDurationInRenameReg() {
            return durationInRenameReg;
        }

        int getCycleBeginCountForRegRead(){
            return cycleBeginCountForRegRead;
        }

        int getDurationInRegRead(){
            return durationInRegRead;
        }

        int getCycleBeginCountForDispatch(){
            return cycleBeginCountForDispatch;
        }

        int getDurationInDispatch(){
            return durationInDispatch;
        }

        int getCycleBeginCountForIQ(){
            return cycleBeginCountForIQ;
        }

        int getDurationInIQ(){
            return durationInIQ;
        }

        int getCycleBeginCountForExecute(){
            return cycleBeginCountForExecute;
        }

        int getDurationInExecute(){
            return durationInExecute;
        }

        int getCycleBeginCountForWriteBack(){
            return cycleBeginCountForWriteBack;
        }

        int getDurationInWriteBack(){
            return durationInWriteBack;
        }

        int getCycleBeginCountForRetire(){
            return cycleBeginCountForRetire;
        }

        int getDurationInRetire(){
            return durationInRetire;
        }
//--------------------------------------------------------------------------------------
//Setter methods------------------------------------------------------------------------

    void setCycleBeginCountForDE(int value) {
        cycleBeginCountForDE = value;
    }

    void setCycleBeginCountForRenameReg(int value) {
        cycleBeginCountForRenameReg = value;
    }

    void setCycleBeginCountForRegRead(int value) {
        cycleBeginCountForRegRead = value;
    }

    void setCycleBeginCountForDispatch(int value) {
        cycleBeginCountForDispatch = value;
    }

    void setCycleBeginCountForIQ(int value) {
        cycleBeginCountForIQ = value;
    }

    void setCycleBeginCountForExecute(int value) {
        cycleBeginCountForExecute = value;
    }

    void setCycleBeginCountForWriteBack(int value) {
        cycleBeginCountForWriteBack = value;
    }

    void setCycleBeginCountForRetire(int value) {
        cycleBeginCountForRetire = value;
    }

//--------------------------------------------------------------------------------------
        // int getCycleBeginCount(){
        //     return cycleBeginCount;
        // }

        // void setCycleBeginCount(int setValue){
        //     cycleBeginCount = setValue;
        // }

        // int getTotalDuration(){
        //     return duration;
        // }
//IncreamentDuration method=================================================================================================
        void increamentDurationForDE(){
            durationInDE++;
        }

        void incrementDurationForRenameReg() {
            durationInRenameReg++;
        }

        void incrementDurationForRegRead() {
            durationInRegRead++;
        }

        void incrementDurationForDispatch() {
            durationInDispatch++;
        }

        void incrementDurationForIQ() {
            durationInIQ++;
        }

        void incrementDurationForExecute() {
            durationInExecute++;
        }

        void incrementDurationForWriteBack() {
            durationInWriteBack++;
        }

        void incrementDurationForRetire() {
            durationInRetire++;
        }
//===================================================================================================

        void setInstructionOperands(int dest, int s1, int s2, int pc){
            this->seqNo = pc;
            this->destReg = dest;
            this->source1 = s1;
            this->source2 = s2;
        }

        void clear(){
            this->valid = false;
            this->operation_type = 0;
            this->seqNo = 0;
            this->destReg = 0;
            this->source1 = 0;
            this->source2 = 0;
            this->address = 0;

            this->destROBTag = 0;
            this->s1ROBTag = 0;
            this->s2ROBTag = 0;
        }

        void setInstructionSpecific(int opType, int dest, int s1, int s2, uint64_t address, int instructionCount){
            this->valid = true;
            this->operation_type = opType;
            this->seqNo = instructionCount;
            this->destReg = dest;
            this->source1 = s1;
            this->source2 = s2;
            this->address = address;
        }

        void setForDEReg(int opType, int dest, int s1, int s2, uint64_t addressFromTrace, int instructionCount){
            setInstructionSpecific(opType, dest, s1, s2, addressFromTrace, instructionCount);
            // this->cycleBeginCountForDE = beginCount;
        }

        void setForRenameReg(int opType, int dest, int s1, int s2, uint64_t addressFromTrace, int instructionCount){
            setInstructionSpecific(opType, dest, s1, s2, addressFromTrace, instructionCount);
            // this->cycleBeginCountForRenameReg = beginCount;
        }

        void setROBTag(int destROB, int s1ROB, int s2ROB, bool s1Ready, bool s2Ready){
            this->destROBTag = destROB;
            this->s1ROBTag = s1ROB;
            this->s2ROBTag = s2ROB;
            setSourcesReady(s1Ready, s2Ready);
        }

        void setSourcesReady(bool value1, bool value2){
            this->s1Ready = value1;
            this->s2Ready = value2;
        }

        void setS1Ready(bool value){
            this->s1Ready = value;
        }

        void setS2Ready(bool value){
            this->s2Ready = value;
        }

        void setS1Tag(int value){
            this->s1ROBTag = value;
        }

        void setS2Tag(int value){
            this->s2ROBTag = value;
        }

        void setDestTag(int value){
            this->destROBTag = value;
        }

        void setDurationCounter(int durationCountInDE, int durationCountInRN, int durationCountInRR, int durationCountInIQ, int durationCountInEX, int durationCountInWB, int durationCountInRT){
            this->durationInDE = durationCountInDE;
            this->durationInRenameReg = durationCountInRN;
            this->durationInRegRead = durationCountInRR;
            this->durationInIQ = durationCountInIQ;
            this->durationInExecute = durationCountInEX;
            this->durationInWriteBack = durationCountInWB;
            this->durationInRetire = durationCountInRT;
        }

        void setCycleBeginCount(int BeginCycleCountDE, int BeginCycleCountRN, int BeginCycleCountRR, int BeginCycleCountIQ, int BeginCycleCountEX, int BeginCycleCountWB, int BeginCycleCountRT){
            this->cycleBeginCountForDE = BeginCycleCountDE;
            this->cycleBeginCountForRenameReg = BeginCycleCountRN;
            this->cycleBeginCountForRegRead = BeginCycleCountRR;
            this->cycleBeginCountForIQ = BeginCycleCountIQ;
            this->cycleBeginCountForExecute = BeginCycleCountEX;
            this->cycleBeginCountForWriteBack = BeginCycleCountWB;
            this->cycleBeginCountForRetire = BeginCycleCountRT;
        }
};
// Put additional data structures here as per your requirement

#endif
