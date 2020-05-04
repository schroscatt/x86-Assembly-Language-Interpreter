#include <iostream>
#include <vector>
#include <fstream>
#include <set>
#include <cmath>
#include <sstream>

using namespace std;
unsigned int counter = 0;
unsigned int memline=0;
int readInput( ifstream& inFile);
string typecheck(string input);
unsigned int hex2dec(string); // cmp,add,sub,mov,logicalex,m
string hex2bin(string hexdec);
void dec2hex(unsigned int);
unsigned int bin2dec(long long n);
string dec2bin(unsigned int n);
int sub(unsigned int to,unsigned int from, int type,int type2,int type3, int type4);
int add(unsigned int loc1, unsigned int loc2, int type, int type2, int type3);
string logicalExp(int type, int size, string num1, string num2);
int logicalEx(unsigned int loc1, unsigned int loc2, int type, int type2, int type3, int exp);
int mov(unsigned int loc1, unsigned int loc2, int type, int type2, int type3);
int cmp(unsigned int loc1, unsigned int loc2, int type, int type2, int type3);
int mov(unsigned int loc1, unsigned int loc2, int type, int type2, int type3);
int intr21();
void pop(unsigned int num,int type);
void push(unsigned int num,int type);
long long int jmp(int loc1,int type);
int rcr2(unsigned int num1, unsigned int left, int bit);
int shr2(unsigned int num1, unsigned int left, int bit);
int shl2(unsigned int num1, unsigned int left, int bit);
int rcl2(unsigned int num1, unsigned int left, int bit);
string toUpper(string a);
int mul(int loc1, int type, int type2);
int div(unsigned locDiv, int type, int sizeType);
int shiftAndRotate(unsigned int loc1, unsigned int loc2, int type, int type2, int type3,int type4);
int incAndDec(unsigned int loc1,int type,int type2,int type3);
unsigned int memory[65536];
vector<pair<string, unsigned int>> labels; // stores the labels name with starting memory location
unsigned int SP = 65534;
string s;
bool ZF=false,CF=false,AF=false,SF=false,OF=false;
//stores registers names and values as pairs
vector<pair<string,unsigned int>> reg={make_pair("AX",0),make_pair("BX",0),make_pair("CX",255),    make_pair("DX",12808), make_pair("DI",65534),make_pair("SP",65534),make_pair("SI",256),make_pair("BP",2334)};

set<string> registers={"AX","BX",  "CX",  "DX",  "DI",  "SP",  "SI",  "BP",  "AH",  "AL",  "BH",  "BL",  "CH",  "CL",  "DH",  "DL"};
set<string> instructions = {   "MOV",  "ADD",     "SUB",    "MUL",  "DIV",     "XOR",    "OR",   "AND",     "NOT",
                               "RCL", "RCR", "SHL",    "SHR", "PUSH",     "POP",    "NOP",  "CMP",     "JZ", "JNZ",  "JE", "JNE", "JA",    "JAE",     "JB",  "JBE",    "JNAE",     "JNB",
                               "JNBE",    "JNC", "JC", "INT" };
vector<pair<int,pair<string,pair<int,unsigned int>>>> variables; //string-> variable's name, pair--> (size, place in memory) //int-> place in code
vector<int> sizeOfVariables;
vector<int> memLocOfVariables;
vector<string> code;// stores the tokens after ReadInput function



//We take input file as arguments and print the output to console !!!!!!!!

int main(int argc, char* argv[]){

    // Command line arguments are malformed
    if (argc == 1) {
        // cerr should be used for errors
        cerr << "Run the code with the command" << endl;
        return 1;
    }

    // Open the input and output files, check for failures
    ifstream inFile(argv[1]);
    if (!inFile) { // operator! is synonymous to .fail(), checking if there was a failure
        cerr << "There was a problem opening \"" << argv[1] << "\" as input file" << endl;
        return 1;
    }

    int res =readInput(inFile);
    // for(int i=0;i<code.size();i++) cout<< code[i]<< endl;
    //  for(int j=0;j<labels.size();j++) cout<< labels[j].first<<endl;
    // cout << labels.size()<<endl;
    if(res==0) {cout<<"Error";return 0;}
    for(int m=0; m<variables.size();m++) {
        int size = variables[m].second.second.first;
        long long int loc = variables[m].first;
        unsigned int memloc = variables[m].second.second.second;
        string value = typecheck(code[loc+1]);
        value.pop_back(); value = value.substr(3,value.size()-1);
        unsigned int val = atoi(value.c_str());
        //  cout << loc << " "<< memloc  << " " << value<< " "<< endl;
        if(size==8) {

            memory[memloc] = val;
        }
        else {
            memory[memloc]= (val)%256;
            memory[memloc+1] =(val)/256;
        }
    }

    string curr;
    int result=1;
    for(unsigned int i=0; i<code.size();i++) {

        //after this part , a string is taken from code vector and with consecutive if statements , our program try to
        //find which operation it is.
        //after finding which operation it is , 1 or 2 more strings are taken from code vector to use as destination and
        //source . In this process destination and source elements are being checked and if there is a syntactic error
        //program gives error . If there is not , by the help of typeCheck function , destinattion and source are turned
        //into arguments that instruction functions can understand and instruction functions are calling for each row.
        //( arguments are explained on the top of the instruction functions)

        curr = code[i];
        curr = toUpper(curr);
        if(curr=="CODE" && toUpper(code[i+1])=="SEGMENT") {i+=1; continue;}

        bool find=true;

        if (curr == "INT") {// this if statements checks the case where the read is "INT"
            if (i > code.size() - 2) {
                cout << "ERROR";
                break;
            }
            i++;curr = code[i];
            if (!(curr != "20h" || curr != "21h")) {
                cout << "Error";
                break;
            }
            if (curr == "20h") {
                break;
            }
            if(curr=="21h")  {
                result= intr21();}
            if(result==0) {
                cout<< "Error";
                break;
            }
        }
        else if(curr=="NOP") continue;
        else if(curr=="MOV" ||curr == "ADD" || curr=="SUB" || curr=="CMP" || curr == "XOR" ||curr == "OR" ||curr == "NOT" || curr =="AND"){
            string to,from;
            if(i>code.size()-5) {
                cout << "Error";
                break;
            }
            i++;to = code[i];
            if(to=="b" ||to=="w") {
                i++; to+= code[i];
                if(to[0]!='[' && to[1]!='[' && to[to.size()-1]!=']' && (to[0]=='b' ||to[0]=='w')) {
                    to =to.substr(1,to.size());
                }
            }
            if(i+2<code.size() ) {

                if((to=="[" ||to== "b[" || to== "w[" ||((to[1]=='[' || to[1]=='[') && to[to.size()-1]!=']' ))) {
                    i++;  string s = code[i];
                    if(s[s.size()-1]!=']') {
                        i++; to+=s+"]";
                    }
                    else to+=s;
                }

            }
            if(curr!="NOT"){
                i++; from = code[i];}
            if(from=="b" ||from=="w") {
                i++; from+= code[i];
                if(from[0]!='[' && from[1]!='[' && from[from.size()-1]!=']'&& (from[0]=='b' ||from[0]=='w')) {
                    from =from.substr(1,from.size());
                }
            }
            if(i+2<code.size() ) {

                if((from=="[" ||from== "b[" || from== "w[" ||((from[1]=='[' || from[1]=='[') && from[from.size()-1]!=']' ))) {
                    i++;  string s = code[i];
                    if(s[s.size()-1]!=']') {
                        i++; from+=s+"]";
                    }
                    else from+=s;
                }
            }
            if(from=="offset") {
                if(i>code.size()-5) {cout<< "Error"; break;}
                i++; from = code[i]; from = typecheck("offset"+from);
            } else from= typecheck(from);
            to = typecheck(to);
            if(to=="Error" ||from=="Error") {
                cout<<"Error";
                break;
            }

            int type=-1;
            unsigned int loc1,loc2, type2,type3;
            if(to.substr(0,3)=="reg" && from.substr(0,3)=="reg")
                type=1;
            else if(to.substr(0,3)=="reg" && (from.substr(0,3)=="mem" || from.substr(0,3)=="con"))
                type=2;
            else if((to.substr(0,3)=="mem" || to.substr(0,3)=="con") && from.substr(0,3)=="reg")
                type=3;
            else if(to.substr(0,3)=="reg" && from.substr(0,3)=="dec")
                type=4;
            else if((to.substr(0,3)=="mem" || to.substr(0,3)=="con") && from.substr(0,3)=="dec")
                type=5;
            loc1 =atoi( to.substr(3,to.length()-2).c_str())/10;
            loc2 =atoi( from.substr(3,from.length()-2).c_str())/10;
            type2 = to[to.size()-1]-48;
            type3 = from[from.size()-1]-48;
            if(curr=="MOV") {
                result=mov(loc1,loc2,type,type2,type3);
            }
            else if(curr=="ADD") {
                result= add(loc1,loc2,type,type2,type3);
            }
            else if(curr=="SUB") {
                result= sub(loc1,loc2,type,type2,type3,1);
            }
            else if(curr=="CMP") {
                result= sub(loc1,loc2,type,type2,type3,2);
            }//1-> and 2-> or 3-> not 4-> xor
            else if(curr=="AND")
                result=  logicalEx(loc1,loc2,type,type2,type3,1);
            else if(curr=="OR")
                result=logicalEx(loc1,loc2,type,type2,type3,2);
            else if(curr=="NOT")
                result=logicalEx(loc1,loc2,type,type2,type3,3);
            else if(curr=="XOR")
                result= logicalEx(loc1,loc2,type,type2,type3,4);
            if(result==0) {
                cout<< "Error";
                break;
            }
        }
        else if(curr=="RCR" ||curr=="RCL" ||curr=="SHL" ||curr=="SHR") {
            string to,from;
            if(i>code.size()-5) {
                cout << "Error";
                break;
            }
            i++;to = code[i];
            if(to=="b" ||to=="w") {
                i++; to+= code[i];
                if(to[0]!='[' && to[1]!='[' && to[to.size()-1]!=']' && (to[0]=='b' ||to[0]=='w')) {
                    to =to.substr(1,to.size());
                }
            }
            if(i+2<code.size() ) {

                if((to=="[" ||to== "b[" || to== "w[" ||((to[1]=='[' || to[1]=='[') && to[to.size()-1]!=']' ))) {
                    i++;  string s = code[i];
                    if(s[s.size()-1]!=']') {
                        i++; to+=s+"]";
                    }
                    else to+=s;
                }}
            i++; from = code[i];
            if(from=="b" ||from=="w") {
                i++; from+= code[i];
                if(from[0]!='[' && from[1]!='[' && from[from.size()-1]!=']'&& (from[0]=='b' ||from[0]=='w')) {
                    from =from.substr(1,from.size());
                }
            }
            if(i+2<code.size() ) {

                if((from=="[" ||from== "b[" || from== "w[" ||((from[1]=='[' || from[1]=='[') && from[from.size()-1]!=']' ))) {
                    i++;  string s = code[i];
                    if(s[s.size()-1]!=']') {
                        i++; from+=s+"]";
                    }
                    else from+=s;
                }}
            to = typecheck(to); from = typecheck(from);
            if(to=="Error" ||from=="Error") {
                cout<<"Error";
                break;
            }
            int type=-1;
            unsigned int loc1,loc2, type2,type3;

            if(to.substr(0,3)=="reg" )
                type=0;
            else if(to.substr(0,3)=="mem"|| to.substr(0,3)=="con")
                type=5;
            if(from.substr(0,3)=="reg" ) {
                if (from.substr(3, 5) != "22") {
                    cout << "Error";
                    break;
                }
                else type3=0;
            }
            else if(from.substr(0,3)=="dec") type3=1;
            else {cout<< "Error"; break;}
            loc1 =atoi( to.substr(3,to.length()-1).c_str())/10;
            loc2 =atoi( from.substr(3,from.length()-1).c_str())/10;
            type2 = to[to.size()-1]-48;
            type3 = from[from.size()-1]-48;
            if(curr=="RCL")
                result=  shiftAndRotate( loc1,  loc2,  type,  type2,  type3, 0);
            else if(curr=="RCR")
                result= shiftAndRotate( loc1,  loc2,  type,  type2,  type3, 1);
            else if(curr=="SHL")
                result=  shiftAndRotate( loc1,  loc2,  type,  type2,  type3, 2);
            else if(curr=="SHR")
                result=  shiftAndRotate( loc1,  loc2,  type,  type2,  type3, 3);
            if(result==0) {
                cout<<"Error";
                break;
            }
        }
        else if(curr=="POP"|| curr=="PUSH") {
            string to,from;
            if(i>code.size()-4) {
                cout << "Error";
                break;
            }
            i++;to = code[i];
            if(to=="b" ||to=="w") {
                i++; to+= code[i];
                if(to[0]!='[' && to[1]!='[' && to[to.size()-1]!=']' && (to[0]=='b' ||to[0]=='w')) {
                    to =to.substr(1,to.size());
                }
            }
            if(i+2<code.size() ) {

                if((to=="[" ||to== "b[" || to== "w[" ||((to[1]=='[' || to[1]=='[') && to[to.size()-1]!=']' ))) {
                    i++;  string s = code[i];
                    if(s[s.size()-1]!=']') {
                        i++; to+=s+"]";
                    }
                    else to+=s;
                }}
            to = typecheck(to);
            if(to=="Error" ||from=="Error") {
                cout<<"Error";
                break;
            }
            int type, type2;
            unsigned int loc1;

            if(to.substr(0,3)=="dec")
                type=0;
            else if(to.substr(0,3)=="reg") {
                type=1;type2=0;
            }
            else if(to.substr(0,3)=="mem" ||to.substr(0,3)=="con") {
                type=2; type2=1;
            }
            if(to[to.size()-1]-48!=0 && type!=0) {
                cout<< "Error"; break;}
            loc1 = atoi(to.substr(3,to.length()-1).c_str())/10;

            if(curr=="PUSH")
                push(loc1,type);
            else if(curr=="POP")
                pop(loc1,type2);

        }
        else if(curr=="MUL"||curr=="DIV") {
            string to;
            if(i>code.size()-5) {
                cout << "Error";
                break;
            }
            i++;to = code[i];
            if(to=="b" ||to=="w") {
                i++; to+= code[i];
                if(to[0]!='[' && to[1]!='[' && to[to.size()-1]!=']' && (to[0]=='b' ||to[0]=='w')) {
                    to =to.substr(1,to.size());
                }
            }
            if(i+2<code.size() ) {

                if((to=="[" ||to== "b[" || to== "w[" ||((to[1]=='[' || to[1]=='[') && to[to.size()-1]!=']' ))) {
                    i++;  string s = code[i];
                    if(s[s.size()-1]!=']') {
                        i++; to+=s+"]";
                    }
                    else to+=s;
                }}
            to = typecheck(to);
            if(to=="Error" ) {
                cout<<"Error";
                break;
            }
            int type=-1;
            unsigned int loc1,type2;
            if(to.substr(0,3)=="reg")
                type=0;
            else if((to.substr(0,3)=="mem" || to.substr(0,3)=="con"))
                type=1;
            if(type==-1) {
                cout<< "Error";
                break;
            }
            loc1 =atoi( to.substr(3,to.length()-1).c_str())/10;
            type2 = to[to.size()-1]-48;
            if(curr=="MUL") result = mul(loc1,type,type2);
            else result= div(loc1,type,type2);
            if(result==0) {
                cout<< "Error";
                break;
            }
        }
        else if(curr=="JNZ" ||curr=="JE" ||curr=="JNE" || curr=="JA" || curr=="JAE" ||curr=="JB" || curr=="JBE" || curr=="JNAE" ||curr=="JNB" || curr=="JNBE" ||
                curr=="JNC" || curr=="JC" ||curr =="JMP") {
            string to;long long int loc=-1;int type;
            if(i>code.size()-4) {
                cout << "Error";
                break;
            }
            i++;to = code[i];
            to = toUpper(to);
            for(int k=0;k<labels.size();k++) {
                if(to==labels[k].first) {
                    loc = k;
                    break;
                }
            }
            if(loc==-1) {
                cout <<    "Error";
                break;
            }
            // "JZ",   "JNZ",  "JE", "JNE", "JA",    "JAE",     "JB",  "JBE",    "JNAE",     "JNB",
            //                                "JNBE",  "JNC", "JC",
            //types start from 0 and represent the type of jump instruction. eg: 0 means JZ (with the order of hw document)
            // loc1 is the index of label in labels vector
            //if the condition does not hold then the function returns -1
            if(curr=="JZ") type=0;
            else if(curr=="JNZ") type=1;
            else if(curr=="JE") type=2;
            else if(curr=="JNE") type=3;
            else if(curr=="JA") type=4;
            else if(curr=="JAE") type=5;
            else if(curr=="JB") type=6;
            else if(curr=="JBE") type=7;
            else if(curr=="JNAE") type=8;
            else if(curr=="JNB") type=9;
            else if(curr=="JNBE") type=10;
            else if(curr=="JNC") type=11;
            else if(curr=="JC") type=12;
            else type=13;
            long long int val=jmp(loc,type);
            if(val==-1) { continue;}
            else{ i=val;continue;}

        }
        else if(curr== "INC" ||curr == "DEC") {
            string to;
            if(i>code.size()-4) {
                cout << "Error";
                break;
            }
            i++;to = code[i];
            if(to=="b" ||to=="w") {
                i++; to+= code[i];
                if(to[0]!='[' && to[1]!='[' && to[to.size()-1]!=']' && (to[0]=='b' ||to[0]=='w')) {
                    to =to.substr(1,to.size());
                }
            }
            if(i+2<code.size() ) {

                if((to=="[" ||to== "b[" || to== "w[" ||((to[1]=='[' || to[1]=='[') && to[to.size()-1]!=']' ))) {
                    i++;  string s = code[i];
                    if(s[s.size()-1]!=']') {
                        i++; to+=s+"]";
                    }
                    else to+=s;
                }}
            to = typecheck(to);
            if(to=="Error") {
                cout<<"Error";
                break;
            }
            int type=-1;
            unsigned int loc1,loc2, type2,type3;
            if(to.substr(0,3)=="reg" )
                type=0;
            else if((to.substr(0,3)=="mem" || to.substr(0,3)=="con"))
                type=1;
            loc1 =atoi( to.substr(3,to.length()-2).c_str())/10;
            type2 = to[to.size()-1]-48;
            if(curr =="INC") {
                result = incAndDec(loc1,type,type2,0);
            }
            else {
                result = incAndDec(loc1,type,type2,1);
            }
            if(result==0) {
                cout<<"Error"; break;
            }
        }
        else {
            bool find2 =false;
            for(int k=0;k<labels.size();k++) {
                if(curr==labels[k].first) find2 = true;
            }
            if(find2==false) find=false;
        }
        if(find==false) {cout << "Error";break;}
    }

    return 0;
}

//this function is used to checking destination and source elements and return their features as an concateneted string
//for example, typecheck(ax) returns "AX00"
//"AX"---> name of register
//"0" ---> AX register's location in reg vector
//"0" ---> because it is an "X" register ( not "L" or "H")
//this function also can handle cosntants
string typecheck(string input) {

    if( input.size()==3) {
        char a = input[0]; char b = input[2];
        if(a==b && (a==44 || a==34 || a==39) ) { int c = input[1];
            return "dec" + to_string(c)+to_string(1);}
    }
    input = toUpper(input);
    vector<string>::iterator it2;
    int l=0;
    if(input=="AX") {
        return "reg00";
    }if(input=="AH") {
        return "reg01";
    }if(input=="AL") {
        return "reg02";
    }if(input=="BX") {
        return "reg10";
    }if(input=="BH") {
        return "reg11";
    }if(input=="BL") {
        return "reg12";
    }if(input=="CX") {
        return "reg20";
    }if(input=="CH") {
        return "reg21";
    }if(input=="CL") {
        return "reg22";
    }if(input=="DX") {
        return "reg30";
    }if(input=="DH") {
        return "reg31";
    }if(input=="DL") {
        return "reg32";
    }if(input=="DI") {
        return "reg40";
    }if(input=="SP") {
        return "reg50";
    }if(input=="SI") {
        return "reg60";
    }if(input=="BP") {
        return "reg70";
    }

    if(input[0]=='[' && input[input.length()-1]==']') {
        input.pop_back();
        string p  = input.substr(1,input.size());
        bool bosluk = true; bool val=false;
        input.clear();
        for(int q=0;q<p.size();q++){
            if(p[q]==' ') bosluk = true;
            if(p[q]!=' ')  {bosluk = false; val=true; input+=p[q];}
            if(bosluk==true && val==true ) {return "Error";}
        }

        if(input=="BX") {
            if(reg[1].second<memline)
                return "Error";

            return "mem"+to_string(reg[1].second)+to_string(2);
        }
        if(input=="DI") {
            if(reg[4].second<memline)
                return "Error";
            return "mem"+to_string(reg[4].second)+to_string(2);
        }
        if(input=="SI") {
            if(reg[6].second<memline)
                return "Error";
            return "mem"+to_string(reg[6].second)+to_string(2);
        }
        if(input=="BP") {
            if(reg[7].second<memline)
                return "Error";
            return "mem"+to_string(reg[7].second)+to_string(2);
        }

        if(input[input.length()-1]=='H') {
            for(unsigned int k = 0;k<input.length()-1;k++) {
                if(!(input[k]!='0' || input[k]!='1' || input[k]!='2' || input[k]!='3' || input[k]!='4' || input[k]!='5' || input[k]!='6' || input[k]!='7' || input[k]!='8' || input[k]!='9' || input[k]!='A' || input[k]!='B' || input[k]!='C' || input[k]!='D' || input[k]!='E' || input[k]!='F') ) {
                    return "Error";
                }
            }
            input.pop_back();
            unsigned int  val = (hex2dec(input));
            if(val<memline) {
                return "Error";
            }
            if(val>=65536) return "Error";
            return "con"+to_string(val)+to_string(2);
        }
        if(input[0]=='0' && input[input.length()-1]!='D') {
            for(unsigned int k = 0;k<input.length();k++) {
                if(!(input[k]!='0' || input[k]!='1' || input[k]!='2' || input[k]!='3' || input[k]!='4' || input[k]!='5' || input[k]!='6' || input[k]!='7' || input[k]!='8' || input[k]!='9' || input[k]!='A' || input[k]!='B' || input[k]!='C' || input[k]!='D' || input[k]!='E' || input[k]!='F') ) {
                    return "Error";
                }
            }
            unsigned int  val = (hex2dec(input));
            if(val<memline) {
                return "Error";
            }
            if(val>=65536) return "Error";
            return "con"+to_string(val)+to_string(2);
        }
        else {
            bool dec=true;
            if(input[input.length()-1]=='D') input.pop_back();
            for(unsigned int n=0;n<input.length();n++) {
                if(!(input[n]!='0' || input[n]!='1' || input[n]!='2' || input[n]!='3' || input[n]!='4' || input[n]!='5' || input[n]!='6' || input[n]!='7' || input[n]!='8' || input[n]!='9')) {
                    dec=false;
                }
            }
            unsigned int val= atoi(input.c_str());
            if(val<memline) {
                return "Error";

            }   if(val>=65536) return "Error";
            if(dec) return "con"+to_string(val)+to_string(2);
        }


        return "Error";

    }
    if(input[0]=='W') {
        if(input[1]=='[' && input[input.length()-1]==']') {
            input.pop_back();
            string p  = input.substr(2,input.size());
            bool bosluk = true; bool val=false;
            input.clear();
            for(int q=0;q<p.size();q++){
                if(p[q]==' ') bosluk = true;
                if(p[q]!=' ')  {bosluk = false; val=true; input+=p[q];}
                if(bosluk==true && val==true ) {return "Error";}
            }
            if(input=="BX") {
                if(reg[1].second<memline)
                    return "Error";
                return "mem"+to_string(reg[1].second)+to_string(0);
            }
            if(input=="DI") {
                if(reg[4].second<memline)
                    return "Error";
                return "mem"+to_string(reg[4].second)+to_string(0);
            }
            if(input=="SI") {
                if(reg[6].second<memline)
                    return "Error";
                return "mem"+to_string(reg[6].second)+to_string(0);
            }
            if(input=="BP") {
                if(reg[7].second<memline)
                    return "Error";
                return "mem"+to_string(reg[7].second)+to_string(0);
            }
            if(input[input.length()-1]=='H') {
                for(unsigned int k = 0;k<input.length()-1;k++) {
                    if(!(input[k]!='0' || input[k]!='1' || input[k]!='2' || input[k]!='3' || input[k]!='4' || input[k]!='5' || input[k]!='6' || input[k]!='7' || input[k]!='8' || input[k]!='9' || input[k]!='A' || input[k]!='B' || input[k]!='C' || input[k]!='D' || input[k]!='E' || input[k]!='F' )) {
                        return "Error";
                    }
                }
                input.pop_back();
                unsigned int  val = (hex2dec(input));
                if(val>=65536) return "Error";
                if(val<memline)
                    return "Error";
                return "con"+to_string(val)+to_string(0);
            }
            if(input[0]=='0' && input[input.length()-1]!='D') {
                for(unsigned int k = 0;k<input.length();k++) {
                    if(!(input[k]!='0' || input[k]!='1' || input[k]!='2' || input[k]!='3' || input[k]!='4' || input[k]!='5' || input[k]!='6' || input[k]!='7' || input[k]!='8' || input[k]!='9' || input[k]!='A' || input[k]!='B' || input[k]!='C' || input[k]!='D' || input[k]!='E' || input[k]!='F') ) {
                        return "Error";
                    }
                }
                unsigned int  val = (hex2dec(input));
                if(val<memline) {
                    return "Error";
                }
                if(val>=65536) return "Error";
                return "con"+to_string(val)+to_string(0);
            }
            else {
                bool dec=true;
                if(input[input.length()-1]=='D') input.pop_back();
                for(unsigned int n=0;n<input.length();n++) {
                    if(!(input[n]!='0' || input[n]!='1' || input[n]!='2' || input[n]!='3' || input[n]!='4' || input[n]!='5' || input[n]!='6' || input[n]!='7' || input[n]!='8' || input[n]!='9')) {
                        dec=false;
                    }
                }
                unsigned int val= atoi(input.c_str());
                if(val>=65536) return "Error";
                if(val<memline) {
                    return "Error";
                } if(dec) return "con"+to_string(val)+to_string(0);
            }
            return "Error";
        }
    }
    if(input[0]=='B') {
        if(input[1]=='[' && input[input.length()-1]==']') {
            input.pop_back();
            string p  = input.substr(2,input.size());
            bool bosluk = true; bool val=false;
            input.clear();
            for(int q=0;q<p.size();q++){
                if(p[q]==' ') bosluk = true;
                if(p[q]!=' ')  {bosluk = false; val=true; input+=p[q];}
                if(bosluk==true && val==true ) {return "Error";}
            }
            if(input=="BX") {
                if(reg[1].second<memline)
                    return "Error";
                return "mem"+to_string(reg[1].second)+to_string(1);
            }
            if(input=="DI") {
                if(reg[4].second<memline)
                    return "Error";
                return "mem"+to_string(reg[4].second)+to_string(1);
            }
            if(input=="SI") {
                if(reg[6].second<memline)
                    return "Error";
                return "mem"+to_string(reg[6].second)+to_string(1);
            }
            if(input=="BP") {
                if(reg[7].second<memline)
                    return "Error";
                return "mem"+to_string(reg[7].second)+to_string(1);
            }
            if(input[input.length()-1]=='H') {
                for(unsigned int k = 0;k<input.length()-1;k++) {
                    if(!(input[k]!='0' || input[k]!='1' || input[k]!='2' || input[k]!='3' || input[k]!='4' || input[k]!='5' || input[k]!='6' || input[k]!='7' || input[k]!='8' || input[k]!='9' || input[k]!='A' || input[k]!='B' || input[k]!='C' || input[k]!='D' || input[k]!='E' || input[k]!='F' )) {
                        return "Error";
                    }
                }
                input.pop_back();
                unsigned int  val = (hex2dec(input));
                if(val>=65536) return "Error";
                if(val<memline)
                    return "Error";
                return "con"+to_string(val)+to_string(1);
            }
            if(input[0]=='0' && input[input.length()-1]!='D') {
                for(unsigned int k = 0;k<input.length();k++) {
                    if(!(input[k]!='0' || input[k]!='1' || input[k]!='2' || input[k]!='3' || input[k]!='4' || input[k]!='5' || input[k]!='6' || input[k]!='7' || input[k]!='8' || input[k]!='9' || input[k]!='A' || input[k]!='B' || input[k]!='C' || input[k]!='D' || input[k]!='E' || input[k]!='F') ) {
                        return "Error";
                    }
                }
                unsigned int  val = (hex2dec(input));
                if(val<memline) {
                    return "Error";
                }
                if(val>=65536) return "Error";
                return "con"+to_string(val)+to_string(1);
            }
            else {
                bool dec=true;
                if(input[input.length()-1]=='D') input.pop_back();
                for(unsigned int n=0;n<input.length();n++) {
                    if(!(input[n]!='0' || input[n]!='1' || input[n]!='2' || input[n]!='3' || input[n]!='4' || input[n]!='5' || input[n]!='6' || input[n]!='7' || input[n]!='8' || input[n]!='9')) {
                        dec=false;
                    }
                }
                unsigned int val= atoi(input.c_str());
                if(val>=65536) return "Error";
                if(val<memline) {
                    return "Error";
                } if(dec) return "con"+to_string(val)+to_string(1);
            }
            return "Error";
        }
    }
    bool offset=false;
    if(input.substr(0,6)=="OFFSET") {
        offset = true;
        input = input.substr(6,input.length());
    }


    for(unsigned int j=0; j<variables.size();j++) {
        int size = variables[j].second.second.first;
        unsigned int loc = variables[j].second.second.second;

        if(variables[j].second.first==input && offset==true) {
            if(size==8) return "dec"+to_string(loc)+to_string(1);
            if(size==16) return "dec"+to_string(loc)+to_string(0); }
        else if(variables[j].second.first==input && offset==false) {
            unsigned int value;
            if(loc<memline) return "Error";
            if(loc>=65536) return "Error";
            if(size==8) return "mem"+to_string(loc)+to_string(1);
            if(size==16) return "mem"+to_string(loc)+to_string(0);

        }
    }
    if(input.size()>=2)
        /* if(input.size()>2) {
             if(input[0]=='W' && input[1]=='.') {
                 input = input.substr(2,input.size());
             }
         }*/

        if(input[input.length()-1]=='H') {
            for(unsigned int k = 0;k<input.length()-1;k++) {
                if(!(input[k]!='0' || input[k]!='1' || input[k]!='2' || input[k]!='3' || input[k]!='4' || input[k]!='5' || input[k]!='6' || input[k]!='7' || input[k]!='8' || input[k]!='9' || input[k]!='A' || input[k]!='B' || input[k]!='C' || input[k]!='D' || input[k]!='E' || input[k]!='F' )) {

                    return "Error";
                }
            }
            input.pop_back();
            unsigned int val = hex2dec(input);
            if(val>255) {
                return "dec"+to_string(val)+to_string(0);
            }
            else
                return "dec"+to_string(val)+to_string(1);

        }
    if(input[0]=='0' && input[input.length()-1]!='D') {
        for(unsigned int k = 0;k<input.length()-1;k++) {
            if(!(input[k]!='0' || input[k]!='1' || input[k]!='2' || input[k]!='3' || input[k]!='4' || input[k]!='5' || input[k]!='6' || input[k]!='7' || input[k]!='8' || input[k]!='9' || input[k]!='A' || input[k]!='B' || input[k]!='C' || input[k]!='D' || input[k]!='E' || input[k]!='F' )) {

                return "Error";
            }
        }
        unsigned int val = hex2dec(input);
        if(val>255) {
            return "dec"+to_string(val)+to_string(0);
        }
        else
            return "dec"+to_string(val)+to_string(1);

    }
    if(input[input.length()-1]=='D') {
        for(unsigned int m=0; m<input.length()-1;m++) {
            if(!(input[m]!='0' || input[m]!='1' || input[m]!='2' || input[m]!='3' || input[m]!='4' || input[m]!='5' || input[m]!='6' || input[m]!='7' || input[m]!='8' || input[m]!='9')) {
                return "Error";
            }
        }
        input.pop_back();
        unsigned int val= atoi(input.c_str());
        if(val>255) {
            return "dec"+to_string(val)+to_string(0);
        }
        else
            return "dec"+to_string(val)+to_string(1);
    }
    if(input[input.length()-1]=='B') {
        for (unsigned int m = 0; m < input.length() - 1; m++) {
            if (input[m] != '0' || input[m] != '1') {
                return "Error";
            }
        }
        input.pop_back();

        unsigned int val= bin2dec(atoi(input.c_str()));
        if(val>255) {
            return "dec"+to_string(val)+to_string(0);
        }
        else
            return "dec"+to_string(val)+to_string(1);
    }


    else {
        bool dec=true;
        for(unsigned int n=0;n<input.length();n++) {
            if(!(input[n]!='0' || input[n]!='1' || input[n]!='2' || input[n]!='3' || input[n]!='4' || input[n]!='5' || input[n]!='6' || input[n]!='7' || input[n]!='8' || input[n]!='9')) {
                dec=false;
            }
        }

        unsigned int val= atoi(input.c_str());
        if(dec) {
            if(val>255) {
                return "dec"+to_string(val)+to_string(0);
            }
            else
                return "dec"+to_string(val)+to_string(1);}


        return "Error";
    }

}
// this function reads input file and split it
//it push tokens to do code vector
//it is called in main
int readInput( ifstream & in) {
    unsigned int count;
    string read;
    // directory where you create the file

    // Use it to read from a file named data.txt.
    bool tirnak = false;
    while (getline(in, read) ) { //gets line til the end (read = line)
        string prev = ""; //keeps previous variable
        string var = ""; //current variable
        if (read != "") {
            for (int i = 0; i < read.length(); i++) { // starts to read line
                if(read[i]==',') {
                    if(var!="") {
                        code.push_back(var);  //push the variable if char is ','
                        prev=var;} //sets prev and cur
                    var = "";
                    continue;
                }
                int a=0;
                if(var.size()==0 && (read[i]==39 || read[i]==44 || read[i]==34)) {
                    if(i+2>=read.size()) return 0;
                    if( read[i+2]!=read[i]) {
                        return 0;
                    }
                    int a = (int)read[i+1];
                    var = to_string(a);
                    code.push_back(var);
                    i +=3;
                    prev=var;
                    var="";
                }

                if (read[i] == ' ' && !tirnak)  { // checks if we are not inside of the tirnak and if it is space

                    if (var == "db" || var == "dw") {
                        int size =0;
                        if(var=="db") {
                            size = 8;

                        }
                        else {
                            size = 16;

                        }
                        long long int loc = code.size();
                        prev = toUpper(prev);
                        for(int l=0; l<variables.size();l++) {
                            if(variables[l].second.first==prev) return 0;
                        }
                        variables.push_back(make_pair(loc,make_pair(prev,make_pair(size,counter)))); // put variables name



                        code.push_back(var);
                        counter += size/8;
                        prev = var;

                    }
                    else if(var!="") {
                        string s = toUpper(var);//if variable is not empty put it into the code and set prev
                        if(instructions.count(s)!=0) {
                            memline+=6;
                            counter+=6;
                        }

                        code.push_back(var);
                        prev = var;
                    }
                    var = ""; //set var
                    continue;
                } else if (read[i] == ';') { //checks if comment starts
                    if(var!="") {

                        code.push_back(var); //puts the last var before the comment
                    }
                    prev=var;
                    var = "";

                    break;
                }

                else {

                    var += read[i];
                    //else append char to variable.
                }
                if(i==read.length()-1) { //if we arrive the end of the line and not push the last variable, put it in code.
                    if(var!="") {
                        if(read[i]== ':') {
                            string curr =toUpper(prev);
                            if(curr=="JNZ" ||curr=="JE" ||curr=="JNE" || curr=="JA" || curr=="JAE" ||curr=="JB" || curr=="JBE" || curr=="JNAE" ||curr=="JNB" || curr=="JNBE" ||
                               curr=="JNC" || curr=="JC" ||curr =="JMP") {return 0;}
                            unsigned int loc= code.size();
                            string b;
                            if(var==":"){b= prev + ":";loc-=1;}else b=var;
                            var.pop_back();
                            b.pop_back();
                            string s = toUpper(b);
                            for(int l=0; l<labels.size();l++) {

                                if(labels[l].first == s) return 0;
                            }
                            labels.push_back({s,loc});
                            //push into labels if it is label
                        }
                        if(var!="")
                            code.push_back(var);
                    }
                }

            }
        }
    }
    return 1;
}
//loc1 takes the register's index in register vector or memory location
//loc2 takes the register's index in register vector, memory location or a constant
//add <reg>,<reg>
//add <reg>,<mem>
//add <mem>,<reg>
//add <reg>,<con>
//add <mem>,<con>
//type takes values from 1 to 5 for each syntax above
//if loc1 is a register index type2 0-> x, 1-> h 2 -> l. If loc1 is a memory location type2 0-> word size 1-> byte size 2-> empty
//if loc2 is a register index type3 0-> x, 1-> h 2 -> l. If loc2 is a memory location type3 0-> word size 1-> byte size 2-> empty
//type4==1 when the operation is sub , ==2 when it is compare
//this function consists of 5 big if statements each of which for different "type" values
//it sets necessary flags 0 at the beginning then , sets according to result
int sub(unsigned int loc1,unsigned int loc2, int type, int type2, int type3,int type4) {
    OF =0, AF=0, CF =0, ZF =0, SF=0;  // 1-> sub 2-> compare
    unsigned int value1,value2;
    long long int result,res;
    if(type==1) {
        if(type3==0) {
            value2 = reg[loc2].second;
        }if(type3==1) {
            value2 = reg[loc2].second/256;
        }if(type3==2) {
            value2 = reg[loc2].second%256;
        }if(type2==0) {
            if(type3==1 ||type3==2) {
                return 0;
            }
            value1 = reg[loc1].second;
            if(value1==value2) {
                ZF =true;
            }
            else if(value1<value2) {
                SF = true;
                CF = true;
            }
            if(value1%256<value2%256) {
                AF = 1;
            }
            result = value1- value2;
            if(result<0) {
                result +=pow(2,16);
            }
            if(result<pow(2,15) && value1>=pow(2,15) && value2<pow(2,15)) {
                OF=1;
            }if(result>=pow(2,15) && value1<pow(2,15) && value2>=pow(2,15)) {
                OF=1;
            }
            if(type4==1)
                reg[loc1].second=result;
        }
        if(type2==1) {
            if(type3==0) {
                return 0;
            }
            value1 = reg[loc1].second/256;
            if(value1==value2) {
                ZF =true;
                result =0;
                res = reg[loc1].second %256;
            }
            else if(value1<value2) {
                if(value1%16<value2%16) {
                    AF = 1;
                }
                SF = true;
                CF =1;
                result= (value1-value2)+pow(2,8);
                res = result*256+reg[loc1].second%256;
            }
            else {
                if(value1%16<value2%16) {
                    AF = 1;
                }
                result =(value1-value2);
                res =(value1-value2)*256+reg[loc1].second%256;

            }
            if(result<pow(2,7) && value1>=pow(2,7) && value2<pow(2,7)) {
                OF=1;
            }if(result>=pow(2,7) && value1<pow(2,7) && value2>=pow(2,7)) {
                OF=1;
            }
            if(type4==1)
                reg[loc1].second=res;

        }
        if(type2==2) {
            if(type3==0) {
                return 0;
            }
            value1 = reg[loc1].second % 256;

            if (value1 == value2) {
                ZF = true;
                result =0;
                res = reg[loc1].second - value1;

            } else if (value1 < value2) {
                if(value1%16<value2%16) {
                    AF = 1;
                }
                SF = true;
                CF =1;
                result= (value1-value2)+pow(2,8);
                res =  (reg[loc1].second/256)*256+result;
            } else {
                if(value1%16<value2%16) {
                    AF = 1;
                }
                result =(value1-value2);
                res = (reg[loc1].second/256)*256+(value1-value2);

            }
            if(result<pow(2,7) && value1>=pow(2,7) && value2<pow(2,7)) {
                OF=1;
            }if(result>=pow(2,7) && value1<pow(2,7) && value2>=pow(2,7)) {
                OF=1;
            }
            if(type4==1)
                reg[loc1].second=res;
        }

    }
    else if(type==2) {
        if(type2==0) {
            if(type3==1) {
                return 0;
            }
            value2= memory[loc2]+ memory[loc2+1]*256;

            value1 = reg[loc1].second;
            if(value1==value2) {
                ZF =true;
                result= 0;
            }
            else if(value1<value2) {
                if(value1%256<value2%256) {
                    AF = 1;
                }
                SF = true;
                CF =true;

                result = value1-value2+pow(2,16);
            }
            else {
                if(value1%256<value2%256) {
                    AF = 1;
                }
                result= value1-value2;

            }
            if(result<pow(2,15) && value1>=pow(2,15) && value2<pow(2,15)) {
                OF=1;
            }if(result>=pow(2,15) && value1<pow(2,15) && value2>=pow(2,15)) {
                OF=1;
            }
            if(type4==1)
                reg[loc1].second=result;
        }
        if(type2==1) {
            if(type3==0) {
                return 0;
            }

            value2= memory[loc2];

            value1 = reg[loc1].second/256;
            if(value1==value2) {
                ZF =true;
                result =0;
                res = reg[loc1].second %256;

            }
            else if(value1<value2) {
                SF = true;
                CF = true;
                if(value1%16<value2%16) {
                    AF = 1;
                }
                result = (value1-value2)+pow(2,8);
                res =result*256+reg[loc1].second%256;
            }
            else {
                result =(value1-value2);
                res =   (value1-value2)*256+reg[loc1].second%256;

                if(value1%16<value2%16) {
                    AF = 1;
                }
            }
            if(result<pow(2,7) && value1>=pow(2,7) && value2<pow(2,7)) {
                OF=1;
            }if(result>=pow(2,7) && value1<pow(2,7) && value2>=pow(2,7)) {
                OF=1;
            }
            if(type4==1)
                reg[loc1].second=res;

        }
        if(type2==2) {
            if(type3==0) {
                return 0;
            }

            value2= memory[loc2];

            value1 = reg[loc1].second % 256;

            if (value1 == value2) {
                ZF = true;
                SF = false;
                result =0;
                res =reg[loc1].second - value1;
            } else if (value1 < value2) {
                SF = true;
                CF = true;
                if(value1%16<value2%16) {
                    AF = 1;
                }
                result = (value1-value2)+pow(2,8);
                res = (reg[loc1].second/256)*256+result;

            } else {
                result =(value1-value2);
                res =(reg[loc1].second/256)*256+(value1-value2);
                if(value1%16<value2%16) {
                    AF = 1;
                }
            }
            if(result<pow(2,7) && value1>=pow(2,7) && value2<pow(2,7)) {
                OF=1;
            }if(result>=pow(2,7) && value1<pow(2,7) && value2>=pow(2,7)) {
                OF=1;
            }
            if(type4==1)
                reg[loc1].second=res;
        }

    }
    else if(type==3) {
        int value1;
        if(type3==0) {
            value2 = reg[loc2].second;
            if(type2==1) {
                return 0;
            }
            value1= memory[loc1]+ memory[loc1+1]*256;

        }if(type3==1) {
            value2 = reg[loc2].second/256;
            if(type2==0) {
                return 0;
            }
            value1= memory[loc1];
        }if(type3==2) {
            value2 = reg[loc2].second%256;
            if(type2==0) {
                return 0;
            }
            value1= memory[loc1];

        }
        int result;
        if(value1==value2) {
            ZF =true;
            result =0;
        }
        else if(value1<value2) {
            if(type3==0) {
                if (value1 % 256 < value2 % 256) {
                    AF = 1;
                }
                result = value1-value2+pow(2,16);

            }
            else {
                if(value1%16<value2%16) {
                    AF = 1;
                }
                result = value1-value2+pow(2,8);

            }
            SF = true;
            CF = true;
        }
        else {
            if(type3==0) {
                if (value1 % 256 < value2 % 256) {
                    AF = 1;
                }
            }
            else {
                if(value1%16<value2%16) {
                    AF = 1;
                }
            }
            result = value1-value2;

        }

        if(type3==0) {
            if(result<pow(2,15) && value1>=pow(2,15) && value2<pow(2,15)) {
                OF=1;
            }if(result>=pow(2,15) && value1<pow(2,15) && value2>=pow(2,15)) {
                OF=1;
            }
            if(type4==1){
                memory[loc1] = result/256;
                memory[loc1+1] = result%256;}
        }
        else if(type3==1 || type3==2) {
            if(result<pow(2,7) && value1>=pow(2,7) && value2<pow(2,7)) {
                OF=1;
            }if(result>=pow(2,7) && value1<pow(2,7) && value2>=pow(2,7)) {
                OF=1;
            }
            if(type4==1)
                memory[loc1] = result;
        }
    }



    else if(type==4) {
        value2 = loc2;
        if(type2==0) {
            value1 = reg[loc1].second;
            if(value1==value2) {
                ZF =true;
                result= 0;
            }
            else if(value1<value2) {
                if (value1 % 256 < value2 % 256) {
                    AF = 1;
                }
                SF = true;
                CF =  true;
                result = value1-value2+pow(2,16);
            }
            else {
                if (value1 % 256 < value2 % 256) {
                    AF = 1;
                }
                result = value1-value2;

            }
            if(result<pow(2,15) && value1>=pow(2,15) && value2<pow(2,15)) {
                OF=1;
            }if(result>=pow(2,15) && value1<pow(2,15) && value2>=pow(2,15)) {
                OF=1;
            }
            if(type4==1)
                reg[loc1].second=result;
        }
        if(type2==1) {
            if(type3==0) {
                return 0;
            }
            value1 = reg[loc1].second/256;
            if(value1==value2) {

                ZF =true;
                result =0;
                res =reg[loc1].second %256;

            }
            else if(value1<value2) {
                if (value1 % 16 < value2 % 16) {
                    AF = 1;
                }
                SF = true;
                CF = true;
                result = (value1-value2)+pow(2,8);
                res = result*256+reg[loc1].second%256;
            }
            else {
                if (value1 % 16 < value2 % 16) {
                    AF = 1;
                }
                result =(value1-value2);
                res=(value1-value2)*256+reg[loc1].second%256;
            }
            if(result<pow(2,7) && value1>=pow(2,7) && value2<pow(2,7)) {
                OF=1;
            }if(result>=pow(2,7) && value1<pow(2,7) && value2>=pow(2,7)) {
                OF=1;
            }
            if(type4==1)
                reg[loc1].second=res;
        }
        if(type2==2) {
            if(type3==0) {
                return 0;
            }
            value1 = reg[loc1].second % 256;

            if (value1 == value2) {
                ZF = true;
                SF = false;
                result = 0;
                res = reg[loc1].second -value1;

            } else if (value1 < value2) {
                if (value1 % 16 < value2 % 16) {
                    AF = 1;
                }
                SF = true;
                CF = true;
                result = (value1-value2)+pow(2,8);
                res= (reg[loc1].second/256)*256+(result);
            } else {
                if (value1 % 16 < value2 % 16) {
                    AF = 1;
                }
                result =(value1-value2);
                res =(reg[loc1].second/256)*256+(value1-value2);

            }
            if(result<pow(2,7) && value1>=pow(2,7) && value2<pow(2,7)) {
                OF=1;
            }if(result>=pow(2,7) && value1<pow(2,7) && value2>=pow(2,7)) {
                OF=1;
            }
            if(type4==1)
                reg[loc1].second=res;
        }
    }
    else {
        unsigned int result;
        value2 = loc2;
        if(type2==2 && type3==1){return 0;}
        if(type3==0 ) {

            value1 = memory[loc1]+memory[loc1+1]*256;

        }
        else if(type3==1) {

            value1 = memory[loc1];

        }
        if(value1==value2) {
            ZF =true;
            SF = false;
            result= 0;
        }
        else if(value1<value2) {
            if(type3==0) {
                if (value1 % 256 < value2 % 256) {
                    AF = 1;
                }
                result = value1-value2+pow(2,16);

            }
            else {
                if (value2 % 16 < value1 % 16) {
                    AF = 1;
                }
                result = value1-value2+pow(2,8);

            }
            SF = true;
            CF = true;
        }
        else {
            if(type3==0) {
                if (value1 % 256 < value2 % 256) {
                    AF = 1;
                }
            }
            else {
                if (value1 % 16 < value2 % 16) {
                    AF = 1;
                }
            }
            result = value1- value2;
            SF = false;
            ZF = false;
        }
        if(type3==0) {
            if(result<pow(2,15) && value1>=pow(2,15) && value2<pow(2,15)) {
                OF=1;
            }if(result>=pow(2,15) && value1<pow(2,15) && value2>=pow(2,15)) {
                OF=1;
            }
            if(type4==1) {
                memory[loc1] = result/256;
                memory[loc1+1] = result%256;}
        }
        else if(type3==1 || type3==2) {
            if(result<pow(2,7) && value1>=pow(2,7) && value2<pow(2,7)) {
                OF=1;
            }if(result>=pow(2,7) && value1<pow(2,7) && value2>=pow(2,7)) {
                OF=1;
            }
            if(type4==1)
                memory[loc1] = result;
        }
    }
    return 1;

}
// type==0-> reg,
// type==1-> memory,
// type==2-> constant

// register case
// sizeType==0-> x,
// sizeType==1-> h
// sizeType==2 -> l

//memory location case
// sizeType==0-> word size
// sizeType==2-> empty
// sizeType==1-> byte size
//this function does division operation
//set flags,memory array and registers
//if there is a overflow condition, gives prints error
int div(unsigned locDiv, int type, int sizeType) {

    long long int value, divisor,result,remainder;
    if(type==0) {
        if(sizeType==0) {
            value = reg[3].second*pow(2,16) + reg[0].second;
            divisor = reg[locDiv].second;
            if(divisor==0) {
                return 0;
            }
            result = value/divisor;
            remainder = value%divisor;
            if(result>pow(2,16)-1 ||remainder>pow(2,16)-1) {
                return 0;
            }
            reg[0].second = result;
            reg[3].second = remainder;
            if(result ==0 || remainder ==0) {
                ZF =1;
            }

            if(result>pow(2,8)-1 ||remainder > pow(2,8)-1) {
                AF=1;
            }
            if((result>pow(2,15)-1 ||remainder>pow(2,15)-1)) {
                OF =1;
            }
        }
        if(sizeType==1) {
            value = reg[0].second;
            divisor = reg[locDiv].second/256;
            if(divisor==0) {
                return 0;
            }
            result = value/divisor;
            remainder = value%divisor;
            if(result>pow(2,8)-1 ||remainder>pow(2,8)-1) {
                return 0;
            }
            reg[0].second = remainder*256+result;

            if(result ==0 || remainder ==0) {
                ZF =1;
            }

            if(result>pow(2,4)-1 ||remainder > pow(2,4)-1) {
                AF=1;
            }
            if(result>pow(2,7)-1 ||remainder> pow(2,7)-1) {
                OF =1;
            }
        }
        if(sizeType==2) {
            value = reg[0].second;
            divisor = reg[locDiv].second%256;
            if(divisor==0) {
                return 0;
            }
            result = value/divisor;
            remainder = value%divisor;
            if(result>pow(2,8)-1 ||remainder>pow(2,8)-1) {
                return 0;
            }
            reg[0].second = remainder*256+result;

            if(result ==0 || remainder ==0) {
                ZF =1;
            }

            if(result>pow(2,4)-1 ||remainder > pow(2,4)-1) {
                AF=1;
            }
            if(result>pow(2,7)-1 ||remainder> pow(2,7)-1) {
                OF =1;
            }
        }

    }
    if(type==1) {
        if(sizeType==0) {
            value = reg[3].second*pow(2,16) + reg[0].second;
            divisor = memory[locDiv]+memory[locDiv+1];
            if(divisor==0) {
                return 0;
            }
            result = value/divisor;
            remainder = value%divisor;
            if(result>pow(2,16)-1 ||remainder>pow(2,16)-1) {
                return 0;
            }
            reg[0].second = result;
            reg[3].second = remainder;
            if(result ==0 || remainder ==0) {
                ZF =1;
            }

            if(result>pow(2,8)-1 ||remainder > pow(2,8)-1) {
                AF=1;
            }
            if(result>pow(2,15)-1 ||remainder>pow(2,15)-1) {
                OF =1;
            }
        }
        if(sizeType==1) {
            value = reg[0].second;
            divisor = memory[locDiv];
            if(divisor==0) {
                return 0;
            }
            result = value/divisor;
            remainder = value%divisor;
            if(result>pow(2,8)-1 ||remainder>pow(2,8)-1) {
                return 0;
            }
            reg[0].second = remainder*256+result;

            if(result ==0 || remainder ==0) {
                ZF =1;
            }

            if(result>pow(2,4)-1 ||remainder > pow(2,4)-1) {
                AF=1;
            }
            if(result>pow(2,7)-1 ||remainder> pow(2,7)-1) {
                OF =1;
            }
        }
    }
    if(type==2) {
        if(sizeType==0) {
            value = reg[3].second*pow(2,16) + reg[0].second;
            divisor = locDiv;
            if(divisor==0) {
                return 0;
            }
            result = value/divisor;
            remainder = value%divisor;
            if(result>pow(2,16)-1 ||remainder>pow(2,16)-1) {
                return 0;
            }
            reg[0].second = result;
            reg[3].second = remainder;
            if(result ==0 || remainder ==0) {
                ZF =1;
            }

            if(result>pow(2,8)-1 ||remainder > pow(2,8)-1) {
                AF=1;
            }
            if(result>pow(2,15)-1 ||remainder>pow(2,15)-1) {
                OF =1;
            }
        }
        if(sizeType==1) {
            value = locDiv;
            divisor = memory[locDiv];
            if(divisor==0) {
                return 0;
            }
            result = value/divisor;
            remainder = value%divisor;
            if(result>pow(2,8)-1 ||remainder>pow(2,8)-1) {
                return 0;
            }
            reg[0].second = remainder*256+result;

            if(result ==0 || remainder ==0) {
                ZF =1;
            }

            if(result>pow(2,4)-1 ||remainder > pow(2,4)-1) {
                AF=1;
            }
            if(result>pow(2,7)-1 ||remainder> pow(2,7)-1) {
                OF =1;
            }
        }
    }
    OF =0, AF=0, CF =0, ZF =0, SF=0;
    return 1;
}
//loc1 is memory location or index of register in reg vector
//loc2 is memory location or index of register in reg vector or constant
// sub <reg>,<reg>
// sub <reg>,<mem>
// sub <mem>,<reg>
// sub <reg>,<con>
// sub <mem>,<con>
//type refers which one of the conditions above are hold

//for loc1 is a register
//type2==0 means "X" , ==1 means "H" , ==2 means "L"

//for loc1 is memory
//type2==0 means word size , type2==1 means byte size  , type2==2 means there is no label in front of memory location like [1234h]

//type3 is same as type2 but it is using to show loc2's features
//exp acts like type4 in documantation . It shows which logical operation should be done ( or, and ,xor etc.)
//this is an multi-task function . It takes arguments and accordingly can done many logical operations.

int logicalEx(unsigned int loc1, unsigned int loc2, int type, int type2, int type3, int exp) {
    if(exp!=3) {
        CF=0; OF=0; SF=0; ZF=0;}
    unsigned int value1; unsigned int value2; unsigned int result;
    string val1; string val2;
    if(type==1) {
        if(type3==0)
            value2= reg[loc2].second;
        else if(type3==1)
            value2= reg[loc2].second/256;
        else
            value2= reg[loc2].second%256;
        if(type2==0) {
            if(type3==1) {
                return 0;
            }
            value1 = reg[loc1].second;
            val1 = dec2bin(value1);
            val2 = dec2bin(value2);
            result = bin2dec(atoi(logicalExp(exp,16,val1,val2).c_str()));
            if(result==0) { if(exp!=3) ZF=1;}
            if(result>pow(2,15)-1){ if(exp!=3) SF =1; }
            reg[loc1].second= result;

        }
        else if(type2==1) {
            if(type3==0) {
                return 0;
            }
            value1 = reg[loc1].second/256;

            val1 = dec2bin(value1);
            val2 = dec2bin(value2);
            result= bin2dec(atoi(logicalExp(exp,8,val1,val2).c_str()));
            if(result==0) {if(exp!=3) ZF=1;}
            if(result>pow(2,7)-1) {if(exp!=3) SF =1;}
            reg[loc1].second = result*256 + reg[loc1].second%256;
        }
        else if(type2==2) {
            if(type3==0) {
                return 0;
            }
            value1 = reg[loc1].second%256;
            val1 = dec2bin(value1);
            val2 = dec2bin(value2);
            result= bin2dec(atoi(logicalExp(exp,8,val1,val2).c_str()));
            if(result==0){ if(exp!=3) ZF=1;}
            if(result>pow(2,7)-1) { if(exp!=3) SF =1;}
            reg[loc1].second = (reg[loc1].second/256)*256+result;
        }
    }
    else if(type==2) {
        if(type2==0) {
            if(type3==1) {
                return 0;
            }
            value1 = reg[loc1].second;
            value2 = memory[loc2] + memory[loc2+1]*256;
            val1 = dec2bin(value1);
            val2 = dec2bin(value2);
            result = bin2dec(atoi(logicalExp(exp,16,val1,val2).c_str()));
            if(result==0){if(exp!=3) ZF=1;}
            if(result>pow(2,15)-1){ if(exp!=3) SF =1; }
            reg[loc1].second= result;

        }
        else if(type2==1) {
            if(type3==0) {
                return 0;
            }
            value1 = reg[loc1].second/256;
            value2 = memory[loc2];
            val1 = dec2bin(value1);
            val2 = dec2bin(value2);
            result= bin2dec(atoi(logicalExp(exp,8,val1,val2).c_str()));
            if(result==0){if(exp!=3) ZF=1;}
            if(result>pow(2,7)-1){if(exp!=3) SF =1;}
            reg[loc1].second = result*256 + reg[loc1].second%256;
        }
        else if(type2==2) {
            if(type3==0) {
                return 0;
            }
            value1 = reg[loc1].second%256;
            value2 = memory[loc2];
            val1 = dec2bin(value1);
            val2 = dec2bin(value2);
            result= bin2dec(atoi(logicalExp(exp,8,val1,val2).c_str()));
            if(result==0){if(exp!=3) ZF=1;}
            if(result>pow(2,7)-1){if(exp!=3) SF =1;}
            reg[loc1].second = (reg[loc1].second/256)*256+result;
        }
    }
    else if(type==3) {
        if (type3 == 0)
            value2 = reg[loc2].second;
        else if (type3 == 1)
            value2 = reg[loc2].second / 256;
        else
            value2 = reg[loc2].second% 256;
        if (type2 == 0) {
            if (type3 == 1) {
                return 0;
            }
            value1 = memory[loc1]+memory[loc1+1];
            val1 = dec2bin(value1);
            val2 = dec2bin(value2);
            result = bin2dec(atoi(logicalExp(exp, 16, val1, val2).c_str()));
            if (result == 0){if(exp!=3) ZF = 1; }
            if (result > pow(2, 15) - 1) {if(exp!=3) SF = 1; }
            memory[loc1] = result/256;
            memory[loc1+1] = result%256;

        } else if (type2 == 1) {
            if (type3 == 0) {
                return 0;
            }
            value1 = memory[loc1];

            val1 = dec2bin(value1);
            val2 = dec2bin(value2);
            result = bin2dec(atoi(logicalExp(exp, 8, val1, val2).c_str()));
            if (result == 0) {if(exp!=3) ZF = 1; }
            if (result > pow(2, 7) - 1) {if(exp!=3) SF = 1; }
            memory[loc1]=result;
        }
    }
    else if(type==4) {
        value2 = loc2;
        if(type2==0) {
            value1 = reg[loc1].second;
            val1 = dec2bin(value1);
            val2 = dec2bin(value2);
            result = bin2dec(atoi(logicalExp(exp,16,val1,val2).c_str()));
            if(result==0){if(exp!=3) ZF = 1; }
            if(result>pow(2,15)-1){if(exp!=3) SF = 1; }
            reg[loc1].second= result;

        }
        else if(type2==1) {
            if(type3==0) {
                return 0;
            }
            value1 = reg[loc1].second/256;
            val1 = dec2bin(value1);
            val2 = dec2bin(value2);
            result= bin2dec(atoi(logicalExp(exp,8,val1,val2).c_str()));
            if(result==0){if(exp!=3) ZF = 1; }
            if(result>pow(2,7)-1){if(exp!=3) SF = 1; }
            reg[loc1].second = result*256 + reg[loc1].second%256;
        }
        else if(type2==2) {
            if(type3==0) {
                return 0;
            }
            value1 = reg[loc1].second%256;
            val1 = dec2bin(value1);
            val2 = dec2bin(value2);
            result= bin2dec(atoi(logicalExp(exp,8,val1,val2).c_str()));
            if(result==0){if(exp!=3) ZF = 1; }
            if(result>pow(2,7)-1) {if(exp!=3) SF = 1; }
            reg[loc1].second = (reg[loc1].second/256)*256+result;
        }
    }
    else if(type==5) {
        value2 =loc2;
        if(type2==2 && type3==1){return 0;}

        if (type2 == 0) {

            value1 = memory[loc1]+memory[loc1+1];
            val1 = dec2bin(value1);
            val2 = dec2bin(value2);
            result = bin2dec(atoi(logicalExp(exp, 16, val1, val2).c_str()));
            if (result == 0) {if(exp!=3) ZF = 1; }
            if (result > pow(2, 15) - 1){if(exp!=3) SF = 1; }
            memory[loc1] = result/256;
            memory[loc1+1] = result%256;

        } else if (type2 == 1) {
            if (type3 == 0) {
                return 0;
            }
            value1 = memory[loc1];

            val1 = dec2bin(value1);
            val2 = dec2bin(value2);
            result = bin2dec(atoi(logicalExp(exp, 8, val1, val2).c_str()));
            if (result == 0) {if(exp!=3) ZF = 1; }
            if (result > pow(2, 7) - 1) {if(exp!=3) SF = 1; }
            memory[loc1]=result;
        }
    } return 1;
}
string logicalExp(int type, int size, string num1, string num2) { //1-> and 2-> or 3-> not 4-> xor
    string s;
    int a = size-num1.length();
    int b = size-num2.length();
    if(a!=0) {
        for (int k = 0; k < a; k++)
            num1= "0" + num1;

    }
    if(b!=0) {
        for (int m = 0; m < b; m++)
            num2 = "0" + num2;

    }

    for(int i=0; i<size;i++) {
        if(type==1) {
            if(num1[i]=='1' && num2[i]=='1') {
                s+='1';
            } else
                s+='0';
        }
        else if(type==2) {
            if(num1[i]=='1' || num2[i]=='1') {
                s+='1';
            } else
                s+='0';
        }
        else if(type==3) {
            if(num1[i]=='1') {
                s+='0';
            }
            else s+='1';
        }
        else if(type==4) {
            if((num1[i]=='1' && num2[i]=='1') || (num1[i]=='0' && num2[i]=='0')) {
                s+='0';
            } else
                s+='1';
        }
    }
    return s;
}

//we no longer use this function !!!!
int cmp(unsigned int loc1, unsigned int loc2, int type, int type2, int type3) {
    unsigned int val1, val2;

    if(type==1) {
        if(type3==0)
            val2=reg[loc2].second;
        else if(type3==1)
            val2=reg[loc2].second/256;
        else
            val2=reg[loc2].second%256;

        if(type2==0) {
            if(type3==1) {
                return 0;
            }
            val1= reg[loc1].second;
        }
        else if(type2==1) {
            if(type3==0) {
                return 0;
            }
            val1=reg[loc1].second/256;
        }
        else {
            if(type3==0) {
                return 0;
            }
            val1= reg[loc1].second%256;
        }
    }
    else if(type==2) {
        if(type2==0) {
            if(type3==1) {
                return 0;
            }
            val1= reg[loc1].second;
            val2=memory[loc2] + memory[loc1+1]*256;
        }
        else if(type2==1) {
            if(type3==0) {
                return 0;
            }
            val2=memory[loc2];
            val1=reg[loc1].second/256;
        }
        else {
            if(type3==0) {
                return 0;
            }
            val2=memory[loc2];
            val1= reg[loc1].second%256;
        }
    }
    else if(type==3) {
        if(type3==0)
            val2=reg[loc2].second;
        else if(type3==1)
            val2=reg[loc2].second/256;
        else
            val2=reg[loc2].second%256;

        if(type2==0) {
            if(type3==1) {
                return 0;
            }
            val1 = memory[loc1]+ memory[loc1+1]*256;
        }
        else if(type2==1) {
            if (type3 == 0) {
                return 0;
            }
            val1 = memory[loc1];
        }
        else {
            if(type3==0) {
                return 0;
            }
            val1 = memory[loc1];
        }
    }
    else if(type==4) {
        val2=loc2;
        if(type2==0) {
            if(type3==1) {
                return 0;
            }
            val1= reg[loc1].second;
        }
        else if(type2==1) {
            if(type3==0) {
                return 0;
            }
            val1=reg[loc1].second/256;
        }
        else {
            if(type3==0) {
                return 0;
            }
            val1= reg[loc1].second%256;
        }
    }
    else {
        val2=loc2;
        if(type2==2 && type3==1){return 0;}

        if(type2==0) {
            if(type3==1) {
                return 0;
            }
            val1 = memory[loc1]+ memory[loc1+1]*256;
        }
        else if(type2==1) {
            if (type3 == 0) {
                return 0;
            }
            val1 = memory[loc1];
        }
        else {
            if(type3==0) {
                return 0;
            }
            val1 = memory[loc1];
        }
    }


    if(val1<val2) {
        CF=1;
        ZF=0;
    }
    else if(val1>val2) {
        CF=0;ZF=0;
    }
    else
        ZF=1;

    return 1;
}

//it is used to print something to screen
//it checks contents of registers and print correct output to screen
int intr21(){
    char b;
    if(reg[0].second/256==1) {
        cin >> b;
        reg[0].second = (reg[0].second/256)*256+b;
    }
    if(reg[0].second/256==2) {
        int a= reg[3].second%256;
        if(a>127 || a<0) return 0;
        if(a==16) {cout<<""<<endl;s=""; b=16;}
        if(a==10) {
            b=10;
            cout<<""<<endl;
            for(int i=0;i<s.size();i++) {
                cout<<" ";
            }
        }else {
            b=a;
            cout<<b;
            s += b;
        }
        reg[0].second = (reg[0].second/256)*256+b;

    }
    return 1;
}


//loc1 is memory location or index of register in reg vector
//loc2 is memory location or index of register in reg vector or constant
// mov <reg>,<reg>
// mov <reg>,<mem>
// mov <mem>,<reg>
// mov <reg>,<con>
// mov <mem>,<con>
//type refers which one of the conditions above are hold

//for loc1 is a register
//type2==0 means "X" , ==1 means "H" , ==2 means "L"

//for loc1 is memory
//type2==0 means word size , type2==1 means byte size  , type2==2 means there is no label in front of memory location like [1234h]

//type3 work same as type2 but it shows the qualities of loc2
//this function does mov operation in assembly language
//if there is a error condition the it returns 0 else it returns 1

int mov(unsigned int loc1, unsigned int loc2, int type, int type2, int type3) {
    unsigned val1,val2;
    if(type==1) {
        if(type3==0)
            val2= reg[loc2].second;
        else if(type3==1)
            val2 = reg[loc2].second/256;
        else
            val2 = reg[loc2].second%256;
        if(type2==0) {
            if(type3!=0) {
                return 0;
            }
            reg[loc1].second = val2;
        }
        else if(type2==1) {
            if(type3==0) {
                return 0;
            }
            reg[loc1].second = val2*256+reg[loc1].second%256;
        }
        else {
            if(type3==0) {
                return 0;
            }
            reg[loc1].second = (reg[loc1].second/256)*256+val2;

        }

    }
    else if(type==2) {
        if(type3==0)
            val2= memory[loc2]+memory[loc2+1]*256;
        else {
            if(type2==0)
                val2= memory[loc2]+memory[loc2+1]*256;
            else val2 = memory[loc2];
        }
        if(type2==0) {
            if(type3==1) {
                return 0;
            }
            reg[loc1].second = val2;
        }
        else if(type2==1) {
            if(type3==0) {
                return 0;
            }

            reg[loc1].second = val2*256+reg[loc1].second%256;
        }
        else {
            if(type3==0) {
                return 0;
            }
            reg[loc1].second = (reg[loc1].second/256)*256+val2;

        }

    }
    else if(type==3) {
        if(type3==0)
            val2= reg[loc2].second;
        else if(type3==1)
            val2 = reg[loc2].second/256;
        else
            val2 = reg[loc2].second%256;
        if(type3==0) {
            if(type2==1) {
                return 0;
            }
            memory[loc1] = val2%256;
            memory[loc1+1] = val2/256;
        }
        else if(type3==1 || type3==2) {
            if(type2==0) {
                return 0;
            }
            memory[loc1]=val2;
        }


    }
    else if(type==4) {
        val2=loc2;
        if(type2==0) {
            reg[loc1].second = val2;
        }
        else if(type2==1) {
            if(type3==0) {
                return 0;
            }
            reg[loc1].second = val2*256+reg[loc1].second%256;
        }
        else {
            if(type3==0) {
                return 0;
            }
            reg[loc1].second = (reg[loc1].second/256)*256+val2;

        }

    }
    else {
        val2=loc2;
        if(type2==2 && type3==1){return 0;}
        if(type3==0) {
            if(type2==1) return 0;
            memory[loc1] = val2%256;
            memory[loc1+1]=val2/256;
        }
        else if(type3==1) {
            memory[loc1] = val2;
        }

    }
    return 1;
}


//it is an easy helper function
unsigned int bin2dec(long long n)
{
    unsigned int decimal = 0,
            i = 0, remainder;
    while (n!=0)
    {
        remainder = n%10;
        n /= 10;
        decimal += remainder*pow(2,i);
        ++i;
    }
    return decimal;
}

//it is an easy helper function
string dec2bin(unsigned int n)
{
    // array to store binary number
    unsigned int binary[32];

    unsigned int i = 0;
    while (n > 0) {

        binary[i] = n % 2;
        n = n / 2;
        i++;
    }
    string s;
    // printing binary array in reverse order
    for (int j = i - 1; j >= 0; j--)
        s+= to_string(binary[j]);

    return s;
}



//it is an easy helper function
string hex2bin(string hexdec)
{
    long int i = 0;
    string s;
    while (hexdec[i]) {

        switch (hexdec[i]) {
            case '0':
                s+= "0000";
                break;
            case '1':
                s+= "0001";
                break;
            case '2':
                s+= "0010";
                break;
            case '3':
                s+= "0011";
                break;
            case '4':
                s+= "0100";
                break;
            case '5':
                s+= "0101";
                break;
            case '6':
                s+= "0110";
                break;
            case '7':
                s+= "0111";
                break;
            case '8':
                s+= "1000";
                break;
            case '9':
                s+= "1001";
                break;
            case 'A':
            case 'a':
                s+= "1010";
                break;
            case 'B':
            case 'b':
                s+= "1011";
                break;
            case 'C':
            case 'c':
                s+= "1100";
                break;
            case 'D':
            case 'd':
                s+= "1101";
                break;
            case 'E':
            case 'e':
                s+= "1110";
                break;
            case 'F':
            case 'f':
                s+= "1111";
                break;
            default:
                s+=  "\nInvalid hexadecimal digit "
                     +hexdec[i];
        }
        i++;
    }
    return s;
}


//it is an easy helper function
unsigned int hex2dec(string hex){
    unsigned int result = 0;
    for (unsigned int i=0; i<hex.length(); i++) {
        if (hex[i]>=48 && hex[i]<=57)
        {
            result += (hex[i]-48)*pow(16,hex.length()-i-1);
        } else if (hex[i]>=65 && hex[i]<=70) {
            result += (hex[i]-55)*pow(16,hex.length( )-i-1);
        } else if (hex[i]>=97 && hex[i]<=102) {
            result += (hex[i]-87)*pow(16,hex.length()-i-1);
        }
    }
    return result;
}


//loc1 is memory location or index of register in reg vector
//loc2 is memory location or index of register in reg vector or constant
// add <reg>,<reg>
// add <reg>,<mem>
// add <mem>,<reg>
// add <reg>,<con>
// add <mem>,<con>
//type refers which one of the conditions above are hold

//for loc1 is a register
//type2==0 means "X" , ==1 means "H" , ==2 means "L"

//for loc1 is memory
//type2==0 means word size , type2==1 means byte size  , type2==2 means there is no label in front of memory location like [1234h]

//type3 work same as type2 but it shows the qualities of loc2
//as we can uderstand from it's name , this function does add operation in assembly
//there is no comlicated algorithm inside this function
//there are just 5 big if statements for each type of syntax
int add(unsigned int loc1, unsigned int loc2, int type, int type2, int type3) {
    unsigned int val1, val2;
    ZF=0,CF=0,AF=0,SF=0,OF=0;
    // cout<<memory[1234]<<"sadfas";
    if(type==1 || type==2 || type==4 || type==3){
        if(type==1 || type==2 || type==4) {

            if (type2 == 1)
                type2 = 2;
            else if (type2 == 2)
                type2 = 1;
        }
        if(type==1 || type==3) {
            if (type3 == 1)
                type3 = 2;
            else if (type3 == 2)
                type3 = 1;
        }
    }


    if (type == 1) {
        if (type3 == 0)
            val1 = reg[loc2].second;
        if (type3 == 1)
            val1 = reg[loc2].second % 256;
        if (type3 == 2)
            val1 = reg[loc2].second / 256;

        if (type2 == 0) {
            if (type3 == 1 || type3 == 2)
                return 0;

            else {
                val2 = reg[loc1].second;
                if(val1+val2==65536)
                    ZF=1;

                if (val1 == 0 && val2 == 0)
                    ZF = 1;

                if ((val1 + val2) > (pow(2, 16) - 1)) {
                    CF = 1;
                }
                if ((val1/32768==val2/32768) && ((val1+val2)%65536)/32768!=val1/32768) {
                    OF = 1;
                }
                reg[loc1] = {reg[loc1].first, (val1 + val2) % 65536};

                if ((val1 % 256 + val2 % 256) >= 256) {
                    AF = 1;
                }
            }
            if((val1+val2)/32768==1)
                SF=1;
        }

        if (type2 == 1) {
            if (type3 == 0)
                return 0;

            else {
                val2 = reg[loc1].second % 256;

                if(val1+val2==256)
                    ZF=1;
                if (val1 == 0 && val2 == 0)
                    ZF = 1;
                if ((val1 + val2) > (pow(2, 8) - 1)) {
                    CF = 1;
                }if ((val1/128==val2/128) && ((val1+val2)%256)/128!=val1/128) {
                    OF = 1;
                }
                reg[loc1] = {reg[loc1].first, (val1 + val2)%256 + (reg[loc1].second / 256) * pow(2, 8)};

                if ((val1 % 16 + val2 % 16) >= 16) {
                    AF = 1;
                }

                if((val1+val2)/128==1)
                    SF=1;
            }
        }

        if (type2 == 2) {
            if (type3 == 0)
                return 0;

            else {
                val2 = reg[loc1].second / 256;

                if(val1+val2==256)
                    ZF=1;
                if (val1 == 0 && val2 == 0)
                    ZF = 1;
                if ((val1 + val2) > (pow(2, 8) - 1)) {
                    CF = 1;
                } if ((val1/128==val2/128) && ((val1+val2)%256)/128!=val1/128) {
                    OF = 1;
                }
                reg[loc1] = {reg[loc1].first, ((val1 + val2)%256) * pow(2, 8) + reg[loc1].second % 256};

                if ((val1 % 16 + val2 % 16) >= 16) {
                    AF = 1;
                }

                if((val1+val2)/128==1)
                    SF=1;
            }
        }

    }

    if (type == 2) {
        if (type2 == 0) {
            if (type3 == 1)
                return 0;

            val1 = memory[loc2] + memory[loc2 + 1] * pow(2, 8);
            val2 = reg[loc1].second;

            if(val1+val2==65536)
                ZF=1;

            if (val1 == 0 && val2 == 0)
                ZF = 0;

            if ((val1 + val2) > (pow(2, 16) - 1)) {
                CF = 1;
            } if ((val1/32768==val2/32768) && ((val1+val2)%65536)/32768!=val1/32768) {
                OF = 1;
            }
            reg[loc1] = {reg[loc1].first, (val1 + val2)%65536};

            if ((val1 % 256 + val2 % 256) >= 256) {
                AF = 1;
            }

            if((val1+val2)/32768==1)
                SF=1;
        }

        if (type2 == 1) {
            if (type3 == 0)
                return 0;

            val2 = memory[loc2];
            val1 = reg[loc1].second % 256;


            if(val1+val2==256)
                ZF=1;
            if (val1 == 0 && val2 == 0)
                ZF = 0;

            if ((val1 + val2) > (pow(2, 8) - 1)) {
                CF = 1;
            } if ((val1/128==val2/128) && ((val1+val2)%256)/128!=val1/128) {
                OF = 1;
            }
            reg[loc1] = {reg[loc1].first, (val1 + val2)%256 + (reg[loc1].second / 256) * pow(2, 8)};

            if ((val1 % 16 + val2 % 16) >= 16) {
                AF = 1;
            }

            if((val1+val2)/128==1)
                SF=1;
        }

        if (type2 == 2) {
            if (type3 == 0)
                return 0;

            val2 = memory[loc2];
            val1 = reg[loc1].second / 256;

            if(val1+val2==256)
                ZF=1;
            if (val1 == 0 && val2 == 0)
                ZF = 0;

            if ((val1 + val2) > (pow(2, 8) - 1)) {
                CF = 1;
            } if ((val1/128==val2/128) && ((val1+val2)%256)/128!=val1/128) {
                OF = 1;
            }
            reg[loc1] = {reg[loc1].first, ((val1 + val2)%256) * pow(2, 8) + reg[loc1].second % 256};

            if ((val1 % 16 + val2 % 16) >= 16) {
                AF = 1;
            }

            if((val1+val2)/128==1)
                SF=1;
        }
    }


    if (type == 3) {
        if (type2 == 0 || (type2 == 2 && type3 == 0)) {
            if (type2 == 0)
                if (type3 == 1 || type3 == 2)
                    return 0;

            val1 = memory[loc1] + memory[loc1 + 1] * pow(2, 8);
            val2 = reg[loc2].second;


            if(val1+val2==65536)
                ZF=1;
            if (val1 == 0 && val2 == 0)
                ZF = 1;

            if ((val1 + val2) > (pow(2, 16) - 1))
                CF = 1;
            if ((val1 / 32768 == val2 / 32768) && ((val1 + val2) % 65536) / 32768 != val1 / 32768)
                OF = 1;

            memory[loc1 + 1] = ((val1 + val2) % 65536) / 256;
            memory[loc1] = ((val1 + val2) % 65536) % 256;

            if ((val1 % 256 + val2 % 256) >= 256) {
                AF = 1;
            }

            if((val1+val2)/32768==1)
                SF=1;
        }

        if (type2 == 1 || (type2 == 2 && (type3 == 1 || type3 == 2))) {
            if (type2 == 1)
                if (type3 == 0)
                    return 0;
            if (type3 == 1)
                val2 = reg[loc2].second % 256;
            if (type3 == 2)
                val2 = reg[loc2].second / 256;

            val1 = memory[loc1];
            if(val1+val2==256)
                ZF=1;

            if (val1 == 0 && val2 == 0)
                ZF = 1;

            if ((val1 + val2) > (pow(2, 8) - 1))
                CF = 1;
            if ((val1 / 128 == val2 / 128) && ((val1 + val2) % 256) / 128 != val1 / 128)
                OF = 1;

            memory[loc1] = (val1 + val2) % 256;

            if ((val1 % 16 + val2 % 16) >= 16) {
                AF = 1;
            }

            if((val1+val2)/128==1)
                SF=1;
        }
    }

    if (type == 4) {
        if (type2 == 0) {


            val1 = reg[loc1].second;
            val2 = loc2;
            if(val1+val2==65536)
                ZF=1;
            if (val1 == 0 && val2 == 0)
                ZF = 1;

            if ((val1 + val2) > (pow(2, 16) - 1))
                CF = 1;
            if ((val1 / 32768 == val2 / 32768) && ((val1 + val2) % 65536) / 32768 != val1 / 32768)
                OF = 1;

            reg[loc1] = {reg[loc1].first, (val1 + val2) % 65536};

            if ((val1 % 256 + val2 % 256) >= 256) {
                AF = 1;
            }
            if((val1+val2)/32768==1)
                SF=1;
        }

        if (type2 == 1) {
            if (type3 == 0)
                return 0;


            else {
                val1 = reg[loc1].second % 256;
                val2 = loc2;

                if(val1+val2==256)
                    ZF=1;

                if (val1 == 0 && val2 == 0)
                    ZF = 1;

                if ((val1 + val2) > (pow(2, 8) - 1))
                    CF = 1;
                if ((val1 / 128 == val2 / 128) && ((val1 + val2) % 256) / 128 != val1 / 128)
                    OF = 1;

                reg[loc1] = {reg[loc1].first, (val1 + val2) % 256 + (reg[loc1].second / 256) * pow(2, 8)};

                if ((val1 % 16 + val2 % 16) >= 16) {
                    AF = 1;
                }
                if((val1+val2)/128==1)
                    SF=1;
            }
        }

        if (type2 == 2) {
            if (type3 == 0)
                return 0;

            else {
                val2 = loc2;
                val1 = reg[loc1].second / 256;
                if(val1+val2==256)
                    ZF=1;
                if (val1 == 0 && val2 == 0)
                    ZF = 1;
                if ((val1 + val2) > (pow(2, 8) - 1))
                    CF = 1;
                if ((val1 / 128 == val2 / 128) && ((val1 + val2) % 256) / 128 != val1 / 128)
                    OF = 1;

                reg[loc1] = {reg[loc1].first, (val1 + val2) % 256 * pow(2, 8) + reg[loc1].second % 256};

                if ((val1 % 16 + val2 % 16) >= 16) {
                    AF = 1;
                }
                if((val1+val2)/128==1)
                    SF=1;
            }
        }
    }


    if (type == 5) {
        if (type2 == 0 || (type2 == 2 && type3 == 0)) {



            val1 = memory[loc1] + memory[loc1 + 1] * pow(2, 8);
            val2 = loc2;


            if(val1+val2==65536)
                ZF=1;
            if (val1 == 0 && val2 == 0)
                ZF = 1;

            if ((val1 + val2) > (pow(2, 16) - 1))
                CF = 1;
            if ((val1 / 32768 == val2 / 32768) && ((val1 + val2) % 65536) / 32768 != val1 / 32768)
                OF = 1;

            memory[loc1 + 1] = ((val1 + val2) % 65536) / 256;
            memory[loc1] = ((val1 + val2) % 65536) % 256;


            if ((val1 % 256 + val2 % 256) >= 256) {
                AF = 1;
            }

            if((val1+val2)/32768==1)
                SF=1;
        }

        if (type2 == 1 || (type2 == 2 && (type3 == 1))) {
            if (type2 == 1)
                if (type3 == 0)
                    return 0;
            if (type3 == 1)
                val2 = loc2;



            val1 = memory[loc1];

            if(val1+val2==256)
                ZF=1;

            if (val1 == 0 && val2 == 0)
                ZF = 1;

            if ((val1 + val2) > (pow(2, 8) - 1))
                CF = 1;
            if ((val1 / 128 == val2 / 128) && ((val1 + val2) % 256) / 128 != val1 / 128)
                OF = 1;

            memory[loc1] = (val1 + val2) % 256;


            if ((val1 % 16 + val2 % 16) >= 16) {
                AF = 1;
            }
            if((val1+val2)/128==1)
                SF=1;


        }

    }
    return 1;

}

//loc1 is memory location or index of register in reg vector
// mul <reg>
// mul <mem>
// mul <con>
//type refers which one of the conditions above are hold

//for loc1 is a register
//type2==0 means "X" , ==1 means "H" , ==2 means "L"

//for loc1 is memory
//type2==0 means word size , type2==1 means byte size  , type2==2 means there is no label in front of memory location like [1234h]

//this function does multiplaction in assembly language
//it has 3 big if statements for the different values of "type"
//At the beggining this function equals affected flags to zero , then set according to result
int mul(int loc1, int type, int type2) {
    unsigned int val1;
    unsigned int val2;
    ZF=0,CF=0,AF=0,SF=0,OF=0;

    if(type==0 ){
        if(type2==1)
            type2=2;
        else if(type2==2)
            type2=1;
    }



    if (type == 0) {
        if (type2 == 0) {
            val1 = reg[0].second;
            val2 = reg[loc1].second;


            if ((val1 * val2) / 65536 > 0) {
                CF = 1;
                OF = 1;
            } else {
                CF = 0;
                OF = 0;
            }

            reg[0] = {reg[0].first, (val1 * val2) % 65536};
            reg[3] = {reg[3].first, (val1 * val2) / 65536};

        }

        if (type2 == 1 || type2 == 2) {
            if (type2 == 1) {
                val1 = reg[loc1].second % 256;
                val2 = reg[0].second % 256;
            } else {
                val1 = reg[loc1].second / 256;
                val2 = reg[0].second % 256;
            }

            if ((val1 * val2) / 256 > 0) {
                CF = 1;
                OF = 1;
            } else {
                CF = 0;
                OF = 0;
            }
            reg[0] = {reg[0].first, (val1 * val2)};
        }
    }
    if (type == 1) {
        if (type2 == 0) {
            val1 = memory[loc1] + memory[loc1 + 1] * pow(10, 8);

            val2 = reg[0].second;

            if ((val1 * val2) / 65536 > 0) {
                CF = 1;
                OF = 1;
            } else {
                CF = 0;
                OF = 0;
            }

            reg[0] = {reg[0].first, (val1 * val2) % 65536};
            reg[3] = {reg[3].first, (val1 * val2) / 65536};

        }

        if (type2 == 1) {

            val1 = reg[0].second % 256;
            val2 = memory[loc1];

            if ((val1 * val2) / 256 > 0) {
                CF = 1;
                OF = 1;
            } else {
                CF = 0;
                OF = 0;
            }
            reg[0] = {reg[0].first, (val1 * val2)};


        }
    }
    return 1;
}

/*I will explain rcr2 , rcl2 ,shl2 and shr2 here
 * They are called from ShiftAndRotate function
 * they works recursively
 * num1 is the number that they should shift or rotate
 * left is how many times we need to shift or rotate this number
 * and bit is 8 or 16 depending on num1
 * in this functions CF is setting and num1 is shifting or rotating
 * by 1
 * at the end of this functions , they recursively call itself by new
 * num1 and left-1
 * there are 2 big if in this functions :
 * one of them for 8 bit shift and rotates
 * other one for 16 bit shift and rotates
 */

int rcl2(unsigned int num1, unsigned int left, int bit) {
    if (left == 0)
        return num1;

    if (bit == 8) {
        int a;
        int b = pow(2, 7);
        a = num1 % b;
        a = a * 2;
        a += CF;
        CF = num1 / b;
        return rcl2(a, left - 1, 8);
    } else {
        int a;
        int b = pow(2, 15);
        a = num1 % b;
        a = a * 2;
        a += CF;
        CF = num1 / b;
        return rcl2(a, left - 1, 16);
    }

}

int shl2(unsigned int num1, unsigned int left, int bit) {
    if (left == 0)
        return num1;

    if (bit == 8) {
        int a;
        int b = pow(2, 7);
        CF=num1/b;
        a=num1%b;
        a = a * 2;
        return shl2(a, left - 1, 8);
    } else {
        int a;
        int b = pow(2, 15);
        CF=num1/b;
        a=num1%b;
        a = a * 2;
        return shl2(a, left - 1, 16);
    }

}

int shr2(unsigned int num1, unsigned int left, int bit) {
    if (left == 0)
        return num1;

    if (bit == 8) {
        int a;
        int b = pow(2, 7);
        CF=num1%2;
        a=num1/2;
        return shr2(a, left - 1, 8);
    } else {
        int a;
        int b = pow(2, 15);
        CF=num1%2;
        a=num1/2;
        return shr2(a, left - 1, 16);
    }

}

int rcr2(unsigned int num1, unsigned int left, int bit) {
    if (left == 0)
        return num1;

    if (bit == 8) {
        int a;
        int b = pow(2, 7);
        a = num1 / 2;
        a += b * CF;
        CF = num1 % 2;
        return rcr2(a, left - 1, 8);
    } else {
        int a;
        int b = pow(2, 15);
        a = num1 / 2;
        a += b * CF;
        CF = num1 % 2;
        return rcr2(a, left - 1, 16);
    }

}
//type==0 means loc1 is a register, ==1 means it is a memory location
//rcl does not accept a register except cl for loc2 . So we will give an error message in main for other registers
//type2==0 means word size,==1 is byte size (for memory location case in loc1) , ==0 means x ,==1 l , ==2 h (for register case in loc1)
//type3==0 means loc2 is cl ,==1 means it is constant.
//type4 refer the action type 0==rcl ,1==rcr,2==shl,3==shr
//it takes values from registers or memory and send them to other recursive functions
//there is a trick in this function . First it calls recursive functions above for left==1
//and set the OF
//then calls recursive functions for left= counter-1
//and sets other flags , registers or memory
int shiftAndRotate(unsigned int loc1, unsigned int loc2, int type, int type2, int type3,int type4) {
    unsigned int val1;
    unsigned int val2;
    unsigned int val3;
    if(type==0){
        if(type2==1)
            type2==2;
        else if(type2==2)
            type2==1;
    }
    if(type==1 && type2==2 )
        return 0;

    if(type4==2 || type4==3)
        ZF=0;
    int label = 8;
    if (type3 == 0)
        val2 = reg[2].second % 256;
    else
        val2 = loc2;


    if (type == 0) {
        if (type2 == 0) {
            val1 = reg[loc1].second;
            label = 16;
        }
        if (type2 == 1)//
            val1 = reg[loc1].second / 256;
        if (type2 == 2)//
            val1 = reg[loc1].second % 256;
    }
    if (type == 1) {
        if (type2 == 0) {
            val1 = memory[loc1] + memory[loc1 + 1] * pow(2, 8);
            label = 16;
        }
        if (type2 == 1)
            val1 = memory[loc1];
    }


    if(val2>31)
        return 0;

    if(type4==0)
        val3 = rcl2(val1, 1, label);
    if(type4==1)
        val3=rcr2(val1,1,label);
    if(type4==2)
        val3=shl2(val1,1,label);
    if(type4==3)
        val3=shr2(val1,1,label);


    OF=0;
    if (label == 8) {
        if ((val1 / 128) != (val3 / 128))
            OF = 1;

    }

    else {
        if ((val1 / 32768) != (val3 / 32768))
            OF = 1;

    }



    if(type4==0)
        val3 = rcl2(val3, val2-1, label);
    if(type4==1)
        val3=rcr2(val3,val2-1,label);
    if(type4==2)
        val3=shl2(val3,val2-1,label);
    if(type4==3)
        val3=shr2(val3,val2-1,label);


    if((type4==2 || type4==3 ) && val3==0)
        ZF=1;
    else
        ZF=0;


    if (type == 0) {
        if (type2 == 0)
            reg[loc1] = {reg[loc1].first, val3};
        if (type2 == 2)//
            reg[loc1] = {reg[loc1].first, (reg[loc1].second / 256) * 256 + val3};
        if (type2 == 1)//
            reg[loc1] = {reg[loc1].first, (reg[loc1].second % 256) + val3 * 256};
    }
    if (type == 1) {
        if (type2 == 0) {
            memory[loc1] = val3 % 256;
            memory[loc1 + 1] = val3 / 256;
        }
        if (type2 == 1)
            memory[loc1] = val3;
    }
    return 1;
}


//type: 0==constant, 1== register , 2==memory
//if the register is 8 bit, in main, program will give error
//if the memory location is byte size (b[1234h]) ,in main , program will give error
//it is an basic push function
void push(unsigned int num,int type){
    unsigned int val1=reg[5].second;
    if(type==0){
        memory[val1]=num%256;
        memory[val1+1]=num/256;
    }
    if(type==1){
        memory[val1]=reg[num].second%256;
        memory[val1+1]=reg[num].second/256;
    }
    if(type==2){
        memory[val1]=memory[num];
        memory[val1+1]=memory[num+1];
    }
    reg[5]={reg[5].first,val1-2};

}

//type: 0==register, 1== memory
//if the register is 8 bit, in main, program will give error
//if the memory location is byte size (b[1234h]) ,in main , program will give error
//it is an basic pop function
void pop(unsigned int num,int type){
    unsigned int val1=reg[5].second;
    if(type==0){
        reg[num]={reg[num].first,memory[val1+2]+memory[val1+3]*256};
    }
    if(type==1){
        memory[num]=memory[val1+2];
        memory[num+1]=memory[val1+3];
    }
    reg[5]={reg[5].first,val1+2};


}

//types start from 0 and represent the type of jump instruction. eg: 0 means JZ (with the order of hw document)
//loc1 is the index of label in labels vector
//if the condition does not hold then the function returns -1
//if jump condition holds it returns starting memory location of label from labels vector
long long int jmp(int loc1,int type){
    if(type==0 || type==2){
        if(ZF)
            return labels[loc1].second;
    }
    if(type==1 || type==3){
        if(!ZF)
            return labels[loc1].second;
    }
    if(type==6 || type==8 || type==12){
        if(CF)
            return labels[loc1].second;
    }
    if(type==5 || type==9 || type==11){
        if(!CF)
            return labels[loc1].second;
    }
    if(type==4 || type==10){
        if(!ZF && !CF)
            return labels[loc1].second;
    }
    if(type==7){
        if(ZF || CF)
            return labels[loc1].second;
    }
    if(type==13)
        return labels[loc1].second;

    return -1;



}

string toUpper(string a) {
    string s;
    for(int i=0; i<a.length();i++) {
        char t = a[i];
        s+= toupper(t);
    }
    return s;
}

//loc1 is register index or memory location
//type==0 means loc1 is register, ==1 means loc1 is memory
//type2==0 means x , ==1 means h, ==2 means l (for type==0)   , type2==0 means w[1234h] ,==1 means b[1234h] , ==2 means [1234h]
//if type3==0 , it is inc , ==1 it is dec
//loc1 is register index or memory location
//type==0 means register, ==1 means memory
//type2==0 means x , ==1 means h, ==2 means l (for type==0)   , type2==0 means w[1234h] ,==1 means b[1234h] , ==2 means [1234h]
//if type3==0 , it is inc , ==1 it is dec

//does just basic increment and decrement in assembly language
//there are 2 big if statement for different syntax types
int incAndDec(unsigned int loc1,int type,int type2,int type3){
    unsigned int val1;
    unsigned int val2;
    unsigned int val3;
    unsigned int val4;
    unsigned int val5;
    bool a=false;
    ZF=0,AF=0,SF=0,OF=0;
    if(type==0){
        if(type2==0) {
            val1 = reg[loc1].second;
            if(type3==0) {
                val2 = (val1 + 1) % 65536;
                if(val1==32767)
                    OF=1;
            }
            if(type3==1) {
                val2 = (val1 - 1);
                if(val1==0)
                    val2=65535;
                if(val1==32768)
                    OF=1;
            }
            if(type3==0)
                val3=val2%256;
            if(type3==1)
                val3=val1%256;
            val4=val1/32768;
            val5=val2/32768;
        }
        if(type2==1) {
            val1 = reg[loc1].second / 256;
            if(type3==0) {
                val2 = (val1 + 1) % 256;
                if(val1==127)
                    OF=1;
            }
            if(type3==1) {
                val2 = (val1 - 1);
                if(val1==0)
                    val2=255;
                if(val1==128)
                    OF=1;
            }
            if(type3==0)
                val3=val2%16;
            if(type3==1)
                val3=val1%16;
            val4=val1/128;
            val5=val2/128;
        }
        if(type2==2) {
            val1 = reg[loc1].second % 256;
            if(type3==0) {
                val2 = (val1 + 1) % 256;
                if (val1 == 127)
                    OF = 1;
            }
            if(type3==1) {
                val2 = (val1 - 1);
                if(val1==0)
                    val2=255;
                if(val1==128)
                    OF=1;
            }
            if(type3==0)
                val3=val2%16;
            if(type3==1)
                val3=val1%16;
            val4=val1/128;
            val5=val2/128;
        }
        if(val2==0)
            ZF=1;
        if(val3==0)
            AF=1;
        if(val4!=val5)
            OF=1;
        if(val5==1)
            SF=1;

        // if(type3==1 && )

        if(type2==0) {
            reg[loc1]={reg[loc1].first,val2};
        }
        if(type2==1) {
            reg[loc1]={reg[loc1].first,val2*256+reg[loc1].second%256};
        }
        if(type2==2) {
            reg[loc1]={reg[loc1].first,val2+(reg[loc1].second/256)*256};
        }
    }

    if(type==1){
        if(type2==2)
            return 0;
        if(type2==0){
            val1=memory[loc1]+memory[loc1+1]*256;
            if(type3==0) {
                val2 = (val1 + 1) % 65536;
                if(val1==32767)
                    OF=1;
            }
            if(type3==1) {
                val2 = (val1 - 1);
                if(val1==0)
                    val2=65535;
                if(val1==32768)
                    OF=1;
            }
            if(type3==0)
                val3=val2%256;
            if(type3==1)
                val3=val1%256;
            val4=val1/32768;
            val5=val2/32768;
        }
        if(type2==1) {
            val1 = memory[loc1];
            if(type3==0) {
                val2 = (val1 + 1) % 256;
                if(val1==127)
                    OF=1;
            }
            if(type3==1) {
                val2 = (val1 - 1);
                if(val1==0)
                    val2=255;
                if(val1==128)
                    OF=1;
            }
            if(type3==0)
                val3=val2%16;
            if(type3==1)
                val3=val1%16;
            val4=val1/128;
            val5=val2/128;
        }
        if(val2==0)
            ZF=1;
        if(val3==0)
            AF=1;
        if(val4!=val5)
            OF=1;
        if(val5==1)
            SF=1;

        if(type2==0) {
            memory[loc1]=val2%256;
            memory[loc1+1]=val2/256;
        }
        if(type2==1) {
            memory[loc1]=val2;
        }

    }
    return 1;
}
