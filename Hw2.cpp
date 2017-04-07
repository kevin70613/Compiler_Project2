#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include<cctype>

using namespace std;

fstream fileIn;     // read grammar.txt
fstream fileIn2;    // read main.c
fstream fileOut;    // write set.txt
fstream fileOut2;   // write LLtable.txt
fstream fileOut3;   // write tree.txt

 // this node structure => linked list -- stack
class Node {
   private:
      string data;
      int num;
   public:
      Node(string data,int num);
      string getData();
      int getNum();
      void setData(string inputData);
      void setNum(int intNum);
};

Node::Node(string data,int num){
   setData(data);
   setNum(num);
}

void Node::setData(string data){
   this->data = data;
}

void Node::setNum(int num){
   this->num = num;
}

string Node::getData(){
   return this->data;
}

int Node::getNum(){
   return this->num;
}

vector<string> mainC;  // store main.c

 // lexical analyzer
void numDFA(string inputstr);
void idDFA(string inputstr);

int main(){
   // line for input string
  string line;
   // use lhsCOunt to record the number of left hand side( nonterminal )
  int lhsCount = 0; 
  vector<string> nonTerminals;
  vector<string> rhs;
  vector<string> terminals;
   // use array of vectors to store lhs corresponding rhs
  vector<string> derRule[500]; // still has problem => change to unknown size
 
  fileIn.open("grammar.txt",ios::in);
  fileIn2.open("main.c",ios::in);
  fileOut.open("set.txt",ios::out);
  fileOut2.open("LLtable.txt",ios::out);
  fileOut3.open("tree.txt",ios::out); 
 
  //=====  preprocessing ======
  while( getline(fileIn,line) ){
     string tab = "	";
     if(line[0] != tab[0] && line.size() != 0){  //check line is not start with white space and not null
        nonTerminals.push_back(line);
        lhsCount++;
     }
     else{  // right hand size
        if(line.size() != 0){ //check line is not null
           string rhsString = line.substr(line.find_first_not_of("	"),line.size()-line.find_first_not_of(" "));
           derRule[lhsCount-1].push_back(rhsString); // push rhs to corresponding lhs 
           rhs.push_back(rhsString);
        }
    }
  }
  fileIn.close();
   // find the terminals
  for(int i=0;i<rhs.size();i++){
     int head = 0, tail = 0; // head -> noncheck string head ; tail -> noncheck string tail
     while( (head + tail) < rhs[i].size() ){  // head + tail is the length of checked string
        if( rhs[i].substr(head+tail,1).compare(" ") != 0 ){
           tail++;
        }else{  // case find the candidate
           int flag = 1; // use to check substr is not nonterminals
           string candiTer = rhs[i].substr(head,tail);
           for(int j=0;j<nonTerminals.size();j++){
               if(candiTer.compare(nonTerminals[j]) == 0)
                  flag *= 0;
               else 
                  flag *= 1;
           }
           if(flag == 1){ //flag is 1 => terminals ,0 => nonterminals
               int testRepit = 1;
               if(terminals.size() == 0 )  //Terminals doesn't have anything
                  terminals.push_back(candiTer);
               else{   // Terminals have something, must check repi terminal not to store it again
                  for(int k=0;k<terminals.size();k++){
                     if(candiTer.compare(terminals[k]) == 0 )
                         testRepit *= 0; // already store this terminals to Terminals
                     else
                         testRepit *= 1;
                  }
                  if(testRepit == 1)
                     terminals.push_back(candiTer);
               }    
           }
             // update the head and tail
           head = head + tail + 1;  // update the head to new uncheck string 
           tail = 0;
        }
         // case meet the end of rhs string ( length = head + tail )
        if( (head + tail) == rhs[i].size()) {
           int flag = 1; // use to check substr is not nonterminals
           string candiTer = rhs[i].substr(head,tail);
           for(int j=0;j<nonTerminals.size();j++){
               if(candiTer.compare(nonTerminals[j]) == 0)
                  flag *= 0;
               else
                  flag *= 1;
           }
           if(flag == 1){ //flag is 1 => terminals ,0 => nonterminals
               int testRepit = 1;
               if(terminals.size() == 0 )  //Terminals doesn't have anything
                  terminals.push_back(candiTer);
               else{   // Terminals have something, must check repi terminal not to store it again
                  for(int k=0;k<terminals.size();k++){
                     if(candiTer.compare(terminals[k]) == 0 )
                         testRepit *= 0; // already store this terminals to Terminals
                     else
                         testRepit *= 1;
                  }
                  if(testRepit == 1)
                     terminals.push_back(candiTer);
               }
           }
             // update the head and tail
           head = head + tail + 1;  // update the head to new uncheck string 
           tail = 0;
        }
     }
  }

  // ==  find the first set ==
  vector<string> nullableSet;  // this vector store the nullable set of nonterminals 
  // this array of vectors use to store the first set of terminals and nonterminals
  vector<string> firstSet[nonTerminals.size()+terminals.size()];
  // this array of vectors yse to store the process which nonTerminals to find terminals first set
  vector<string> LLTableNontoTer[nonTerminals.size()];

  // find the nullable set
  for(int i=0;i<nonTerminals.size();i++){
     for(int j=0;j<derRule[i].size();j++){
          // find every nonterminals rhs has epsilon or not
         if(derRule[i][j].compare("epsilon") == 0)
             nullableSet.push_back(nonTerminals[i]);
     }
  }
  
  for(int i=0;i<nonTerminals.size();i++){
     for(int j=0;j<derRule[i].size();j++){
          // find every nonterminals rhs has epsilon or not
         for(int k=0;k<nullableSet.size();k++){
             if(derRule[i][j].compare(nullableSet[k]) == 0){
                 int noRepi = 1;
                 for(int l=0;l<nullableSet.size();l++){
                     if(nonTerminals[i].compare(nullableSet[l])==0)
                        noRepi *= 0;
                 }
                 if(noRepi == 1)
                     nullableSet.push_back(nonTerminals[i]);
             }
         }
     }
  }

  // first part is for terminals
  for(int i=0;i<terminals.size();i++)
     firstSet[i].push_back(terminals[i]);

  // second part is for nonterminals
  // first time to find first set
  int nonTerIndex = terminals.size();  // get the start index of nonterminals

  for(int i=0;i<nonTerminals.size();i++){
      for(int j=0;j<derRule[i].size();j++){
         int head = 0, tail = 0;
         int findFirst = 1; // this variable use to find the first token of string
         while(findFirst){
            if( derRule[i][j].substr(head+tail,1).compare(" ") != 0 ){
                tail++;
            }else{  // case find the candidate
                int nullFlag = 1;  // this flag test the nullable or not
                string candiTer = derRule[i][j].substr(head,tail);
                 //test this candidate is nullable or not
                for(int k=0;k<nullableSet.size();k++){
                     if(candiTer.compare(nullableSet[k]) == 0)
                          nullFlag *= 0;
                     else 
                          nullFlag *= 1;
                }
                if(nullFlag == 0){ // this is nullable
                      // update new head and tail and continuous to run while loop
                     firstSet[nonTerIndex].push_back(candiTer);  //
                     LLTableNontoTer[i].push_back(candiTer);     //
                     head = head + tail + 1;
                     tail = 0;               
                }else{ // not nullalbe
                     firstSet[nonTerIndex].push_back(candiTer);
                     LLTableNontoTer[i].push_back(candiTer);
                     findFirst = 0; // break while loop
                }
            }
             // case meet the end of rhs string ( length = head + tail )
            if( (head + tail) == derRule[i][j].size()) {
                string candiTer = derRule[i][j].substr(head,tail);
                firstSet[nonTerIndex].push_back(candiTer);
                LLTableNontoTer[i].push_back(candiTer);
                findFirst = 0;
            }
         }
      }
      nonTerIndex++; // go to next index to store the next nonterminals first set
  }
   // change the result which has nonterminals found first iteration to all terminals
  int allTer = 1; // this flag check all firstSet is terminals
  int jCount = 0;
  while(allTer){
    int allTerCount = 0; // this variable count the number of terminals
     for(int i=0;i<nonTerminals.size();i++){
        int hasNonTer = 1;
        jCount = 0;
        for(int j=0;j<firstSet[terminals.size()+i].size();j++){
             // test has nonTerminals or not
            for(int k=0;k<nonTerminals.size();k++){
               if(firstSet[terminals.size()+i][j].compare(nonTerminals[k]) == 0){ // get the nonTerminals
                   for(int l=0;l<firstSet[terminals.size()+k].size();l++){
                       // push this nonTerminals first set to the firstSet
                      LLTableNontoTer[i].push_back(firstSet[terminals.size()+k][l]+" "+LLTableNontoTer[i][j+jCount]);
                      int notReplica = 1;
                      for(int m=0;m<firstSet[i].size();m++){ // test  avoid replica
                          if(firstSet[terminals.size()+k][l].compare(firstSet[terminals.size()+i][m])==0)
                             notReplica *= 0;
                          else notReplica *= 1;
                      }
                      if(notReplica == 1)
                         firstSet[terminals.size()+i].push_back(firstSet[ terminals.size()+k ][l]); 
                   }
                   firstSet[terminals.size()+i].erase(firstSet[terminals.size()+i].begin()+j); // delete the jth element
                   j = j-1; // because delete one element the next element will move forward => change pointer  
                   hasNonTer *= 0;
                   jCount++;
                   k=nonTerminals.size(); // break this find nonTerminals loop
               }
            }
        }
        if(hasNonTer == 1){ // this element of first set is all terminals
           allTerCount++;
        }
     }
     if(allTerCount == nonTerminals.size() )   // all nonTerminals first set are terminals
        allTer = 0;
  }
   // output to set.txt
  fileOut << "First" << endl;
  for(int i=0;i<nonTerminals.size();i++){
     fileOut << nonTerminals[i] << "  :  " ;
     for(int j=0;j<firstSet[terminals.size()+i].size();j++)
         fileOut << firstSet[terminals.size()+i][j] << " ";
     fileOut << endl;
  }
  
  // == find the follow set  ==

  vector<string> rhsToken[rhs.size()];  // store each token of rhs
  vector<string> followSet[terminals.size()+nonTerminals.size()];    // store the follow set
  // find right hand side token
  for(int i=0;i<rhs.size();i++){
      // find the terminals
     int head = 0, tail = 0; // head -> noncheck string head ; tail -> noncheck string tail
     while( (head + tail) < rhs[i].size() ){  // head + tail is the length of checked string
        if( rhs[i].substr(head+tail,1).compare(" ") != 0 ){
           tail++;
        }else{  // case find the candidate
           string candi = rhs[i].substr(head,tail);
           rhsToken[i].push_back(candi);
              // update the head and tail
           head = head + tail + 1;  // update the head to new uncheck string 
           tail = 0;
        }
         // case meet the end of rhs string ( length = head + tail )
        if( (head + tail) == rhs[i].size()) {
           string candi = rhs[i].substr(head,tail);
           rhsToken[i].push_back(candi);
             // update the head and tail
           head = head + tail + 1;  // update the head to new uncheck string 
           tail = 0;
        }
     }
  }
  // find the case 1: start symbol contains $
  followSet[terminals.size()].push_back("$");
 
  // find the case 2: A->aXB  first(B)-{epsilon} = follow(X)
  // nonTerminals case
  for(int i=0;i<nonTerminals.size();i++){
     for(int j=0;j<rhs.size();j++){
         for(int k=0;k<rhsToken[j].size();k++){
               //find each nonterminals in this case follow set
              if(rhsToken[j][k].compare(nonTerminals[i])==0){  
                  // check is not the last and can use case 2 
                  if(k < rhsToken[j].size()-1){ // not the right most 
                       //get the first set
                       //handle the terminals case first set doesn't contain epsilon without epsilon
                      for(int m=0;m<terminals.size();m++){ 
                          if(rhsToken[j][k+1].compare(terminals[m])==0 /*&& rhsToken[j][k+1].compare("epsilon")!=0*/){ 
                           // find the next token which is terminals 
                              for(int n=0;n<firstSet[m].size();n++){  // check this terminals token first set
                                   //test not repeat one 
                                  int repiFlag = 1; //this variable test the repeat
                                  for(int p=0;p<followSet[terminals.size()+i].size();p++){
                                      if(firstSet[m][n].compare(followSet[terminals.size()+i][p])==0)
                                           repiFlag *= 0;
                                  } 
                                   // doesn't find before and add to follow set
                                  if(repiFlag == 1 && firstSet[m][n].compare("epsilon") != 0)  
                                     followSet[terminals.size()+i].push_back(firstSet[m][n]);
                              }
                          }
                      }    
                       //handle the nonTerminals case
                      for(int m=0;m<nonTerminals.size();m++){
                          if(rhsToken[j][k+1].compare(nonTerminals[m])==0){ // next token is nonterminals      
                              for(int n=0;n<firstSet[terminals.size()+m].size();n++){
                                   int repiFlag = 1; //this variable test the repeat
                                   for(int p=0;p<followSet[terminals.size()+i].size();p++){
                                       if(firstSet[terminals.size()+m][n].compare(followSet[terminals.size()+i][p])==0)
                                           repiFlag *= 0;
                                   }
                                   // doesn't find before and add to follow set
                                   if(repiFlag == 1 && firstSet[terminals.size()+m][n].compare("epsilon") != 0)
                                      followSet[terminals.size()+i].push_back(firstSet[terminals.size()+m][n]);
                              }
                          }
                      }
                  
                  }
              }
         }
     }
  }
   
  int lastTimeCount = 0;
  int rhsCount = 0;
  
  int count =0;
  while(count <3){
  lastTimeCount = 0;
  rhsCount = 0;
  for(int i=0;i<nonTerminals.size();i++){
      rhsCount += derRule[i].size(); // count every lhs's rhs
      for(int j=lastTimeCount;j<rhsCount;j++){
         int rightMost = rhsToken[j].size()-1;
         // right most is not epsilon
         int hasEpsilon = 1;
         while(hasEpsilon){
            int epsiFlag = 1; // check has epsilon in first set
            if(rightMost>=0 && rhsToken[j][rightMost].compare("epsilon")!=0){
                // right most token is nonTerminals
                int nonTerFG = 1;
                for(int k=0;k<nonTerminals.size();k++){
                    
                    if(rhsToken[j][rightMost].compare(nonTerminals[k])==0){
                        nonTerFG *= 0;
                        // find is order of this nonTerminals
                        int nonIndex = 0; //nonIndex is for nonTerminals
                        for(int m=0;m<nonTerminals.size();m++)
                            if(nonTerminals[k].compare(nonTerminals[m])==0)
                                nonIndex = m;  // asign index of the nonterminals to nonIndex
                        for(int l=0;l<firstSet[terminals.size()+nonIndex].size();l++){
                             if(firstSet[terminals.size()+nonIndex][l].compare("epsilon")==0){
                                epsiFlag *= 0;
                             }
                        }
                        // this case is A->abX , follow(A) = follow(X)
                        for(int l=0;l<followSet[terminals.size()+i].size();l++){
                           //test not repeat one
                           int repiFlag = 1; //this variable test the repeat
                           for(int p=0;p<followSet[terminals.size()+nonIndex].size();p++){
                               if(followSet[terminals.size()+i][l].compare(followSet[terminals.size()+nonIndex][p])==0)
                                   repiFlag *= 0;
                           }
                            // doesn't find before and add to follow set
                           if(repiFlag == 1 && followSet[terminals.size()+i][l].compare("epsilon") != 0){
                                followSet[terminals.size()+nonIndex].push_back(followSet[terminals.size()+i][l]);
                           }
                        }
                        if(epsiFlag == 0){  // right most token first set contain epsilon
                           rightMost--;
                        }else{   // right most token first set don't contain epsilon 
                            // this case is A->abX , follow(A) = follow(X)
                            for(int l=0;l<followSet[terminals.size()+i].size();l++){
                                 //test not repeat one
                                int repiFlag = 1; //this variable test the repeat
                                for(int p=0;p<followSet[terminals.size()+nonIndex].size();p++){
                                    if(followSet[terminals.size()+i][l].compare(followSet[terminals.size()+nonIndex][p])==0)
                                        repiFlag *= 0;
                                }
                                 // doesn't find before and add to follow set
                                if(repiFlag == 1 && followSet[terminals.size()+i][l].compare("epsilon") != 0){
                                    followSet[terminals.size()+nonIndex].push_back(followSet[terminals.size()+i][l]);
                                }
                            }
                        }
                        if(epsiFlag == 1 || rightMost < 0){  // not epsilon or out of size
                           hasEpsilon = 0;
                           k = nonTerminals.size(); // break the for loop
                        }
                    }
                }
                if(nonTerFG == 1){ // right most token is terminals
                    hasEpsilon = 0;
                }
            }
            if(rightMost==0 && rhsToken[j][rightMost].compare("epsilon")==0)
                hasEpsilon = 0;
            //cout << "hasEpsilon =" << hasEpsilon << endl;
        }
        lastTimeCount = rhsCount; // update last time of count
     }
  }
  count++;
  }

  // output follow to set.txt
  fileOut << endl << endl << endl << "Follow " << endl;
  for(int i=0;i<nonTerminals.size();i++){
     fileOut << nonTerminals[i] << "  :  ";
     for(int j=0;j<followSet[terminals.size()+i].size();j++)
         fileOut << followSet[terminals.size()+i][j] << " ";
     fileOut << endl;
  }
  fileOut.close();
  //===== Construct LLTable   =========
  vector<string> LLtableTer;    // store LLtable terminals
  vector<string> LLtableNon;    // store LLtable nonterminals
  vector<string> LLtableRule;   // store LLtable derivation rule

  for(int i=0;i<nonTerminals.size();i++){
      // rule 1 to construct LLtable
     for(int j=0;j<LLTableNontoTer[i].size();j++){
         // find the terminals
        int head = 0, tail = 0; // head -> noncheck string head ; tail -> noncheck string tail
        string candiTer = "";
        string candiNon = "";
         // find the first one(terminals)
        int findFir = 1; 
        while( findFir ){  // head + tail is the length of checked string
            if( LLTableNontoTer[i][j].substr(head+tail,1).compare(" ") != 0 ){
                tail++;
            }else{  // case find the candidate
                candiTer = LLTableNontoTer[i][j].substr(head,tail);
                findFir = 0;
            }
             // case meet the end of rhs string ( length = head + tail )
            if( (head + tail) == LLTableNontoTer[i][j].size()) {
                candiTer = LLTableNontoTer[i][j].substr(head,tail);
                findFir = 0;
            }
        }
         // find the last one(original nonterminals) 
        head = 0;
        tail = 0;
        while( (head + tail) < LLTableNontoTer[i][j].size() ){  // head + tail is the length of checked string
            if( LLTableNontoTer[i][j].substr(head+tail,1).compare(" ") != 0 ){
                tail++;
            }else{  
                candiNon = LLTableNontoTer[i][j].substr(head,tail);
                 // update the head and tail
                head = head + tail + 1;  // update the head to new uncheck string 
                tail = 0;
            }
             // case meet the end of rhs string ( length = head + tail )
            if( (head + tail) == LLTableNontoTer[i][j].size()) {
                candiNon = LLTableNontoTer[i][j].substr(head,tail);
                 // update the head and tail
                head = head + tail + 1;  // update the head to new uncheck string 
                tail = 0;
            }
        }
        for(int k=0;k<terminals.size();k++){
            if(candiTer.compare(terminals[k])==0){
                for(int n=0;n<derRule[i].size();n++){
                     // find the terminals
                    int head = 0, tail = 0; // head -> noncheck string head ; tail -> noncheck string tail
                    string candiTest = "";  // use to get the first of rhs 
                    while((head+tail) < derRule[i][n].size() ){  // head + tail is the length of checked string
                        if( derRule[i][n].substr(head+tail,1).compare(" ") != 0 ){
                            tail++;
                        }else{  // case find the candidate
                            candiTest = derRule[i][n].substr(head,tail);
                            head = derRule[i][n].size(); // break while condition
                        }
                         // case meet the end of rhs string ( length = head + tail )
                        if( (head + tail) == derRule[i][n].size()) {
                            candiTest = derRule[i][n].substr(head,tail);
                            head = derRule[i][n].size(); 
                        }
                    }
                    //test the candiTest equals candiNon or not
                    if(candiNon.compare(candiTest)==0){
                        LLtableNon.push_back(nonTerminals[i]);
                        LLtableTer.push_back(candiTer);
                        LLtableRule.push_back(derRule[i][n]); 
                    }

                }
            }
        }
      
     }
      // rule 2 and 3 to construct LLtable
     for(int n=0;n<derRule[i].size();n++){
        // test follow set has $ or not
        int head = 0, tail = 0; // head -> noncheck string head ; tail -> noncheck string tail
        string candiTest = "";  // use to get the first of rhs 
        while((head+tail) < derRule[i][n].size() ){  // head + tail is the length of checked string
            if( derRule[i][n].substr(head+tail,1).compare(" ") != 0 ){
                tail++;
            }else{  // case find the candidate
                candiTest = derRule[i][n].substr(head,tail);
                head = derRule[i][n].size(); // break while condition
            }
            // case meet the end of rhs string ( length = head + tail )
            if( (head + tail) == derRule[i][n].size()) {
                candiTest = derRule[i][n].substr(head,tail);
                head = derRule[i][n].size();
            }
        }
        for(int t=0;t<nonTerminals.size();t++){
            if(candiTest.compare(nonTerminals[t])==0){  // find what nonterminals the candiTest is 
                for(int v=0;v<firstSet[terminals.size()+t].size();v++){
                    if(firstSet[terminals.size()+t][v].compare("epsilon")==0){   // test first set has epsilon 
                        // rule 2
                       for(int u=0;u<followSet[terminals.size()+i].size();u++){
                           if(followSet[terminals.size()+i][u].compare("$")==0){
                               LLtableNon.push_back(nonTerminals[i]);
                               LLtableTer.push_back("$");
                               LLtableRule.push_back(derRule[i][n]);
                           }
                       }
                        // rule 3
                       for(int w=0;w<followSet[terminals.size()+i].size();w++){
                           LLtableNon.push_back(nonTerminals[i]);
                           LLtableTer.push_back(followSet[terminals.size()+i][w]);
                           LLtableRule.push_back(derRule[i][n]);
                       }
                    }
                }
            }
             //test caniTest is epsilon or not  => rule 3
            if(candiTest.compare("epsilon")==0){ // first of epsilon is epsilon
                for(int w=0;w<followSet[terminals.size()+i].size();w++){
                    LLtableNon.push_back(nonTerminals[i]);
                    LLtableTer.push_back(followSet[terminals.size()+i][w]);
                    LLtableRule.push_back(derRule[i][n]);
                }
            }
               
        }
     }
     
  }
   // output LLtable
  int startFlag = 0;
  int startCount = 0;
  while(!startFlag){
    if(LLtableTer[startCount].compare("epsilon")!=0)
       startFlag = 1;
    else
       startCount++;
  }
  fileOut2 << LLtableNon[startCount] << "   " << LLtableTer[startCount] << "   " << LLtableRule[startCount] << endl;
  for(int i=startCount+1;i<LLtableNon.size();i++){
     int checkFlag = 1;
     for(int j=0;j<i;j++){
        if(LLtableNon[i].compare(LLtableNon[j])==0 && LLtableTer[i].compare(LLtableTer[j])==0 
           && LLtableRule[i].compare(LLtableRule[j])==0)
            checkFlag *= 0;   // has the repeat one then => 0
     }
     if(checkFlag == 1 && LLtableTer[i].compare("epsilon") != 0)
        fileOut2 << LLtableNon[i] << "   " << LLtableTer[i] << "   " << LLtableRule[i] << endl;
  }
  fileOut2.close();
  
  // ============ produce derivation tree  ========
  // ===  hw1 : lexical analyzer
  while( getline(fileIn2,line) ){
      // store the current front of string which doesn't check
     int front = 0;
      // check substring length
     int length = 1;
     int state = 0;
     for(length=1;length<line.size()-front+1;length++ ){
          string subLine = line.substr(front,length);
          if(subLine.size() >= 6 && !state){
               // check the keyword
             if( (subLine.substr(subLine.size()-6,6).compare("double") == 0 ||
                  subLine.substr(subLine.size()-6,6).compare("return") == 0 ||
                  subLine.substr(subLine.size()-6,6).compare("return") == 0) && subLine.size() == 6 ){
                 string nextStr = line.substr(front,length+1);
                   // check the next char is white space or not
                   // use isspace to check whitespace,note: this function needs char not string!
                 if( isspace(nextStr[nextStr.size()-1]) || nextStr.substr(nextStr.size()-1,1).compare("\n")==0 ){
                       // check is number or not
                     numDFA(subLine.substr(0,subLine.size()-6));
                       // check is identifier or not
                     idDFA(subLine.substr(0,subLine.size()-6));
         
                     mainC.push_back(subLine.substr(subLine.size()-6,6));
                     state = 1;
                 }
             }
             else if( (subLine.substr(subLine.size()-6,6).compare("double") == 0 ||
                       subLine.substr(subLine.size()-6,6).compare("return") == 0
                       ) && subLine.size() > 6 ){
                       // identifier which contain those keywords
                       // check is number or not
                     numDFA(subLine.substr(0,subLine.size()-6));
                       // check is identifier or not
                     idDFA(subLine.substr(0,subLine.size()-6));
                    
                     state = 1;
             }
          }
            // check the last five words
          if(subLine.size() >= 5 && !state){
                // check the keywords
             if( (subLine.substr(subLine.size()-5,5).compare("float")==0 || subLine.substr(subLine.size()-5,5).compare("while")==0 ||
                  subLine.substr(subLine.size()-5,5).compare("break")==0 || subLine.substr(subLine.size()-5,5).compare("print")==0)
                  && subLine.size()==5 ){
  
                 string nextStr2 = line.substr(front,length+1);
                   // check the next char is white space or not
                   // use isspace to check whitespace,note: this function needs char not string!
                 if( isspace(nextStr2[nextStr2.size()-1]) || nextStr2.substr(nextStr2.size()-1,1).compare("\n")==0 ){
                       // check is number or not
                     numDFA(subLine.substr(0,subLine.size()-5));
                       // check is identifier or not
                     idDFA(subLine.substr(0,subLine.size()-5));
   
                     mainC.push_back(subLine.substr(subLine.size()-5,5));
                     state = 1;
                 }
             }
             else if( (subLine.substr(subLine.size()-5,5).compare("float")==0 ||
                       subLine.substr(subLine.size()-5,5).compare("while")==0 ||
                       subLine.substr(subLine.size()-5,5).compare("break")==0 ||
                       subLine.substr(subLine.size()-5,5).compare("print")==0 ) && subLine.size()>5 ){
                       // identifier which contain those keywords
                       // check is number or not
                     numDFA(subLine.substr(0,subLine.size()-5));
                       // check is identifier or not
                     idDFA(subLine.substr(0,subLine.size()-5));
   
                     state = 1;
              }
          }
            // check the last four words
          if(subLine.size() >= 4 && !state){
               // check the keywords
             if( (subLine.substr(subLine.size()-4,4).compare("char")==0 || subLine.substr(subLine.size()-4,4).compare("else")==0)
                   && subLine.size() == 4 ){
  
                 string nextStr3 = line.substr(front,length+1);
                   // check the next char is white space or not
                   // use isspace to check whitespace,note: this function needs char not string!
                 if( isspace(nextStr3[nextStr3.size()-1]) || nextStr3.substr(nextStr3.size()-1,1).compare("\n")==0  ){
                        // check is number or not
                      numDFA(subLine.substr(0,subLine.size()-4));
                        // check is identifier or not
                      idDFA(subLine.substr(0,subLine.size()-4));
   
                      mainC.push_back(/*"Keyword "+*/subLine.substr(subLine.size()-4,4));
                      state = 1;
                 }
             }else if((subLine.substr(subLine.size()-4,4).compare("char")==0 || subLine.substr(subLine.size()-4,4).compare("else")==0)
                        && subLine.size() > 4 ){ // identifier which contain those keywords
                     // check is number or not
                   numDFA(subLine.substr(0,subLine.size()-4));
                     // check is identifier or not
                   idDFA(subLine.substr(0,subLine.size()-4));
   
                   state = 1;
             }
          }
            // check the last three words
          if(subLine.size() >= 3 && !state){
               // check the keywords
             if( (subLine.substr(subLine.size()-3,3).compare("int")==0 || subLine.substr(subLine.size()-3,3).compare("for")==0)
                  && subLine.size()==3 ){
  
                 string nextStr4 = line.substr(front,length+1);
                   // check the next char is white space or not
                   // use isspace to check whitespace,note: this function needs char not string!
                 if( isspace(nextStr4[nextStr4.size()-1]) || nextStr4.substr(nextStr4.size()-1,1).compare("\n")==0 ){
                       // check is number or not
                     numDFA(subLine.substr(0,subLine.size()-3));
                       // check is identifier or not
                     idDFA(subLine.substr(0,subLine.size()-3));
   
                     mainC.push_back(subLine.substr(subLine.size()-3,3));
                     state = 1;
                 }
             }else if( (subLine.substr(subLine.size()-3,3).compare("int")==0 || subLine.substr(subLine.size()-3,3).compare("for")==0)
                        && subLine.size() > 3){// identifier which contain those keywords
                      // check is number or not
                    numDFA(subLine.substr(0,subLine.size()-3));
                      // check is identifier or not
                    idDFA(subLine.substr(0,subLine.size()-3));
   
                    state = 1;
             }
          }
           // check the last two words
          if(subLine.size() >= 2 && !state){
               // check the keywords
             if( (subLine.substr(subLine.size()-2,2).compare("if") == 0) && subLine.size() == 2){
  
                 string nextStr5 = line.substr(front,length+1);
                   // check the next char is white space or not
                   // use isspace to check whitespace,note: this function needs char not string!
                 if( isspace(nextStr5[nextStr5.size()-1]) || nextStr5.substr(nextStr5.size()-1,1).compare("\n")==0 ){
                       // check is number or not
                     numDFA(subLine.substr(0,subLine.size()-2));
                       // check is identifier or not
                     idDFA(subLine.substr(0,subLine.size()-2));
   
                     mainC.push_back(subLine.substr(subLine.size()-2,2));
                     state = 1;
                 }
             }else if( (subLine.substr(subLine.size()-2,2).compare("if") == 0) && subLine.size() > 2){
                       // identifier which contain those keywords
                       // check is number or not
                     numDFA(subLine.substr(0,subLine.size()-2));
                       // check is identifier or not
                     idDFA(subLine.substr(0,subLine.size()-2));
   
                     state = 1;
            }
              // check the Operators
            if(subLine.substr(subLine.size()-2,2).compare("==") == 0 || subLine.substr(subLine.size()-2,2).compare("!=") == 0 ||
               subLine.substr(subLine.size()-2,2).compare(">=") == 0 || subLine.substr(subLine.size()-2,2).compare("<=") == 0 ||
               subLine.substr(subLine.size()-2,2).compare("&&") == 0 || subLine.substr(subLine.size()-2,2).compare("||") == 0 ){
                   // check is number or not
                 numDFA(subLine.substr(0,subLine.size()-2));
                   // check is identifier or not
                 idDFA(subLine.substr(0,subLine.size()-2));
   
                 mainC.push_back(subLine.substr(subLine.size()-2,2));
                 state = 1;
             }                 
              // check comment, ignore the comment
             else if(subLine.substr(subLine.size()-2,2).compare("//")==0){
                   // check is number or not
                 numDFA(subLine.substr(0,subLine.size()-2));
                   // check is identifier or not
                 idDFA(subLine.substr(0,subLine.size()-2));
                 length = line.size()-front+1;
                 state = 1;
             }
          }
            // check the last one word       
          if(subLine.size() >= 1 && !state){
              // check the whitespace
              // use isspace to check whitespace,note: this function needs char not string!
             if( isspace(subLine[subLine.size()-1])){
                   // check is number or not
                 numDFA(subLine.substr(0,subLine.size()-1));
                   // check is identifier or not
                 idDFA(subLine.substr(0,subLine.size()-1));
   
                 state = 1;
  
                   // check the case ' '
                 if( line.substr(front,length+1).compare("' '")==0 ){
                    state = 0;
                 }
 
             }
               // check the operators
             else if(subLine.substr(subLine.size()-1,1).compare("=") == 0 || subLine.substr(subLine.size()-1,1).compare("!") == 0 ||
                     subLine.substr(subLine.size()-1,1).compare("+") == 0 || subLine.substr(subLine.size()-1,1).compare("-") == 0 ||
                     subLine.substr(subLine.size()-1,1).compare("*") == 0 || subLine.substr(subLine.size()-1,1).compare("<") == 0 ||
                     subLine.substr(subLine.size()-1,1).compare(">") == 0 ){
                  // check the next char
                 string temp = line.substr(front,length+1);
                 if(!(temp.substr(temp.size()-2,2).compare("==") == 0 || temp.substr(temp.size()-2,2).compare("!=") == 0 ||
                      temp.substr(temp.size()-2,2).compare(">=") == 0 || temp.substr(temp.size()-2,2).compare("<=") == 0 ||
                      temp.substr(temp.size()-2,2).compare("+=") == 0 || temp.substr(temp.size()-2,2).compare("-=") == 0 ||
                      temp.substr(temp.size()-2,2).compare("*=") == 0) ){
                       // check is number or not
                     numDFA(subLine.substr(0,subLine.size()-1));
                       // check is identifier or not
                     idDFA(subLine.substr(0,subLine.size()-1));
   
                     mainC.push_back(subLine.substr(subLine.size()-1,1));
                     state = 1;
                 }
   
             }
                // check the special symbols
             else if(subLine.substr(subLine.size()-1,1).compare("[") == 0 || subLine.substr(subLine.size()-1,1).compare("]") == 0 ||
                     subLine.substr(subLine.size()-1,1).compare("(") == 0 || subLine.substr(subLine.size()-1,1).compare(")") == 0 ||
                     subLine.substr(subLine.size()-1,1).compare("{") == 0 || subLine.substr(subLine.size()-1,1).compare("}") == 0 ||
                     subLine.substr(subLine.size()-1,1).compare(";") == 0 || subLine.substr(subLine.size()-1,1).compare(",") == 0 ){
                   // check is number or not
                 numDFA(subLine.substr(0,subLine.size()-1));
                   // check is identifier or not
                 idDFA(subLine.substr(0,subLine.size()-1));
   
                 mainC.push_back(subLine.substr(subLine.size()-1,1));
                 state = 1;
             }
              // / is the special case, because // and / are ambiguous
             else if(subLine.substr(subLine.size()-1,1).compare("/") == 0){
                 string temp2 = line.substr(front,length+1);
                 if( !(temp2.substr(temp2.size()-2,2).compare("//") == 0)){
                       // check is number or not
                     numDFA(subLine.substr(0,subLine.size()-1));
                       // check is identifier or not
                     idDFA(subLine.substr(0,subLine.size()-1));
   
                     mainC.push_back(subLine.substr(subLine.size()-1,1));
                     state = 1;
                 }
             }
          }
            // check has enter the state or not
          if(state == 1){
             front = front+length;
               // change front to remove subString
             length = 0;
               // length set to 0
             state = 0;
          }
     }
  }
  fileIn2.close();
  //push end symbol $ to mainC stack
  mainC.push_back("$");
  // ===
  vector<Node> stackList;  // this vector use as stack
  stackList.push_back(Node("S",1));
  int stackTop = 0; // point to the top of stack
  int mainTop = 0;
  int treeLevel = stackList[stackTop].getNum();
  while( stackList.size()>0 /*&& mainTop<mainC.size()*/ ){
     for(int i=0;i<LLtableNon.size();i++){
        if(stackList[stackTop].getData().compare(LLtableNon[i])==0){ //get the top of stack
            if(LLtableTer[i].compare(mainC[mainTop]) == 0){  // find the terminals
               vector<string> temp;
               int head = 0, tail = 0; // head -> noncheck string head ; tail -> noncheck string tail
               while( (head + tail) < LLtableRule[i].size() ){  // head + tail is the length of checked string
                   if( LLtableRule[i].substr(head+tail,1).compare(" ") != 0 ){
                      tail++;
                   }else{  // case find the candidate
                      string candi = LLtableRule[i].substr(head,tail);
                      temp.push_back(candi);
                        // update the head and tail
                      head = head + tail + 1;  // update the head to new uncheck string
                      tail = 0;
                   }
                     // case meet the end of rhs string ( length = head + tail )
                   if( (head + tail) == LLtableRule[i].size()) {
                      string candi = LLtableRule[i].substr(head,tail);
                      temp.push_back(candi);
                       // update the head and tail
                      head = head + tail + 1;  // update the head to new uncheck string
                      tail = 0;
                   }
               }
                // pop top element out of stack
               for(int k=0;k<stackList[stackTop].getNum();k++)
                   fileOut3 << "  ";
               fileOut3 << stackList[stackTop].getNum() << "  " << stackList[stackTop].getData() << endl;
               stackList.pop_back(); // pop element out
               stackTop = stackList.size()-1;
               treeLevel++; //update treeLevel
               for(int j=temp.size()-1;j>=0;j--){
                   stackList.push_back( Node(temp[j],treeLevel) );
               }
               stackTop = stackList.size()-1;
            }
        }
        if(stackList[stackTop].getData().compare(mainC[mainTop])==0){
            int specFlag = 0;
            for(int k=0;k<stackList[stackTop].getNum();k++)
                fileOut3 << "  ";
            fileOut3 << stackList[stackTop].getNum() << "  " << stackList[stackTop].getData() << endl;
             //handle id & num case
            if(stackList[stackTop].getData().compare("num")==0 || stackList[stackTop].getData().compare("id")==0){
               specFlag = 1;
               for(int l=0;l<stackList[stackTop].getNum()+1;l++)
                   fileOut3 << "  ";
               fileOut3 << stackList[stackTop].getNum()+1 << "  "; 
            }  
            stackList.pop_back();
             // update the top pointer
            stackTop = stackList.size()-1;
            mainTop++;
            if(specFlag == 1){
               fileOut3 << mainC[mainTop] << endl;
               //if(mainTop < mainC.size()-1)
               mainTop++;
            }
            treeLevel = stackList[stackTop].getNum();
        }
         // handle stack top is epsilon case
        if(stackList[stackTop].getData().compare("epsilon")==0){
            for(int k=0;k<stackList[stackTop].getNum();k++)
                fileOut3 << "  ";
            fileOut3 << stackList[stackTop].getNum() << "  " << stackList[stackTop].getData() << endl;
            stackList.pop_back();
            stackTop = stackList.size()-1; //update
            treeLevel = stackList[stackTop].getNum();
        }
        if(stackList[stackTop].getData().compare("$")==0){
           for(int k=0;k<stackList[stackTop].getNum();k++)
               fileOut3 << "  ";
           fileOut3 << stackList[stackTop].getNum() << "  " << stackList[stackTop].getData() << endl;
           stackList.pop_back();
           stackTop = stackList.size()-1;
           treeLevel = stackList[stackTop].getNum();
        }
        if(stackTop == -1){
           i=LLtableNon.size(); // break for loop 
        }
     }
  }
  fileOut3.close();
}


void numDFA(string inputstr){
     // number string length
    int count = 0;
   
    if(inputstr.substr(0,1).compare("0")==0 || inputstr.substr(0,1).compare("1")==0 || inputstr.substr(0,1).compare("2")==0 ||
       inputstr.substr(0,1).compare("3")==0 || inputstr.substr(0,1).compare("4")==0 || inputstr.substr(0,1).compare("5")==0 ||
       inputstr.substr(0,1).compare("6")==0 || inputstr.substr(0,1).compare("7")==0 || inputstr.substr(0,1).compare("8")==0 ||
       inputstr.substr(0,1).compare("9")==0 ){
        count++;
    }
     // check other of the string is a number or not
    for(int i=1;i<inputstr.length();i++){
       if(inputstr.substr(i,1).compare("0")==0 || inputstr.substr(i,1).compare("1")==0 || inputstr.substr(i,1).compare("2")==0 ||
          inputstr.substr(i,1).compare("3")==0 || inputstr.substr(i,1).compare("4")==0 || inputstr.substr(i,1).compare("5")==0 ||
          inputstr.substr(i,1).compare("6")==0 || inputstr.substr(i,1).compare("7")==0 || inputstr.substr(i,1).compare("8")==0 ||
          inputstr.substr(i,1).compare("9")==0 || inputstr.substr(i,1).compare(".")==0 ){
           count++;
       }
    }
   if(inputstr.size() != 0 && count == inputstr.size()){
       mainC.push_back("num");
       mainC.push_back(inputstr);
   }
}
   
void idDFA(string inputstr){
     // id string length
    int count = 0;
   
    if(inputstr.substr(0,1).compare("a")==0 || inputstr.substr(0,1).compare("b")==0 || inputstr.substr(0,1).compare("c")==0 ||
       inputstr.substr(0,1).compare("d")==0 || inputstr.substr(0,1).compare("e")==0 || inputstr.substr(0,1).compare("f")==0 ||
       inputstr.substr(0,1).compare("g")==0 || inputstr.substr(0,1).compare("h")==0 || inputstr.substr(0,1).compare("i")==0 ||
       inputstr.substr(0,1).compare("j")==0 || inputstr.substr(0,1).compare("k")==0 || inputstr.substr(0,1).compare("l")==0 ||
       inputstr.substr(0,1).compare("m")==0 || inputstr.substr(0,1).compare("n")==0 || inputstr.substr(0,1).compare("o")==0 ||
       inputstr.substr(0,1).compare("p")==0 || inputstr.substr(0,1).compare("q")==0 || inputstr.substr(0,1).compare("r")==0 ||
       inputstr.substr(0,1).compare("s")==0 || inputstr.substr(0,1).compare("t")==0 || inputstr.substr(0,1).compare("u")==0 ||
       inputstr.substr(0,1).compare("v")==0 || inputstr.substr(0,1).compare("w")==0 || inputstr.substr(0,1).compare("x")==0 ||
       inputstr.substr(0,1).compare("y")==0 || inputstr.substr(0,1).compare("z")==0 || inputstr.substr(0,1).compare("A")==0 ||
       inputstr.substr(0,1).compare("B")==0 || inputstr.substr(0,1).compare("C")==0 || inputstr.substr(0,1).compare("D")==0 ||
       inputstr.substr(0,1).compare("E")==0 || inputstr.substr(0,1).compare("F")==0 || inputstr.substr(0,1).compare("G")==0 ||
       inputstr.substr(0,1).compare("H")==0 || inputstr.substr(0,1).compare("I")==0 || inputstr.substr(0,1).compare("J")==0 ||
       inputstr.substr(0,1).compare("K")==0 || inputstr.substr(0,1).compare("L")==0 || inputstr.substr(0,1).compare("M")==0 ||
       inputstr.substr(0,1).compare("N")==0 || inputstr.substr(0,1).compare("O")==0 || inputstr.substr(0,1).compare("P")==0 ||
       inputstr.substr(0,1).compare("Q")==0 || inputstr.substr(0,1).compare("R")==0 || inputstr.substr(0,1).compare("S")==0 ||
       inputstr.substr(0,1).compare("T")==0 || inputstr.substr(0,1).compare("U")==0 || inputstr.substr(0,1).compare("V")==0 ||
       inputstr.substr(0,1).compare("W")==0 || inputstr.substr(0,1).compare("X")==0 || inputstr.substr(0,1).compare("Y")==0 ||
       inputstr.substr(0,1).compare("Z")==0 || inputstr.substr(0,1).compare("_")==0 ){
        count++;
        for(int i=1;i<inputstr.length();i++){
          if(inputstr.substr(i,1).compare("a")==0 || inputstr.substr(i,1).compare("b")==0 || inputstr.substr(i,1).compare("c")==0 ||
             inputstr.substr(i,1).compare("d")==0 || inputstr.substr(i,1).compare("e")==0 || inputstr.substr(i,1).compare("f")==0 ||
             inputstr.substr(i,1).compare("g")==0 || inputstr.substr(i,1).compare("h")==0 || inputstr.substr(i,1).compare("i")==0 ||
             inputstr.substr(i,1).compare("j")==0 || inputstr.substr(i,1).compare("k")==0 || inputstr.substr(i,1).compare("l")==0 ||
             inputstr.substr(i,1).compare("m")==0 || inputstr.substr(i,1).compare("n")==0 || inputstr.substr(i,1).compare("o")==0 ||
             inputstr.substr(i,1).compare("p")==0 || inputstr.substr(i,1).compare("q")==0 || inputstr.substr(i,1).compare("r")==0 ||
             inputstr.substr(i,1).compare("s")==0 || inputstr.substr(i,1).compare("t")==0 || inputstr.substr(i,1).compare("u")==0 ||
             inputstr.substr(i,1).compare("v")==0 || inputstr.substr(i,1).compare("w")==0 || inputstr.substr(i,1).compare("x")==0 ||
             inputstr.substr(i,1).compare("y")==0 || inputstr.substr(i,1).compare("z")==0 || inputstr.substr(i,1).compare("A")==0 ||
             inputstr.substr(i,1).compare("B")==0 || inputstr.substr(i,1).compare("C")==0 || inputstr.substr(i,1).compare("D")==0 ||
             inputstr.substr(i,1).compare("E")==0 || inputstr.substr(i,1).compare("F")==0 || inputstr.substr(i,1).compare("G")==0 ||
             inputstr.substr(i,1).compare("H")==0 || inputstr.substr(i,1).compare("I")==0 || inputstr.substr(i,1).compare("J")==0 ||
             inputstr.substr(i,1).compare("K")==0 || inputstr.substr(i,1).compare("L")==0 || inputstr.substr(i,1).compare("M")==0 ||
             inputstr.substr(i,1).compare("N")==0 || inputstr.substr(i,1).compare("O")==0 || inputstr.substr(i,1).compare("P")==0 ||
             inputstr.substr(i,1).compare("Q")==0 || inputstr.substr(i,1).compare("R")==0 || inputstr.substr(i,1).compare("S")==0 ||
             inputstr.substr(i,1).compare("T")==0 || inputstr.substr(i,1).compare("U")==0 || inputstr.substr(i,1).compare("V")==0 ||
             inputstr.substr(i,1).compare("W")==0 || inputstr.substr(i,1).compare("X")==0 || inputstr.substr(i,1).compare("Y")==0 ||
             inputstr.substr(i,1).compare("Z")==0 || inputstr.substr(i,1).compare("0")==0 || inputstr.substr(i,1).compare("1")==0 ||
             inputstr.substr(i,1).compare("2")==0 || inputstr.substr(i,1).compare("3")==0 || inputstr.substr(i,1).compare("4")==0 ||
             inputstr.substr(i,1).compare("5")==0 || inputstr.substr(i,1).compare("6")==0 || inputstr.substr(i,1).compare("7")==0 ||
             inputstr.substr(i,1).compare("8")==0 || inputstr.substr(i,1).compare("9")==0 || inputstr.substr(i,1).compare("_")==0)
               count++;
       }
    }
      // string is not empty
    if( inputstr.size() != 0 && count == inputstr.size() ){
       mainC.push_back("id");
       mainC.push_back(inputstr);
    }
}
