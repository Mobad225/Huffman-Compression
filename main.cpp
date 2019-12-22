#include<bits/stdc++.h>
#define IO ios_base::sync_with_stdio(0); cin.tie(0); cout.tie(0)
using namespace std;

int pow(int base, int exponent){
if(!exponent) return 1;
if(exponent & 1) return base * pow(base, exponent - 1);
int temp = pow(base, exponent/2);
return temp * temp;
}

class Node{
public:
  int value;
  Node *left, *right;
  unsigned char c;

  Node(){
    left = right = nullptr;
  }

  Node(int value): value(value){
    left = right = nullptr;
  }

  Node(int value, Node* left, Node* right): value(value), left(left), right(right){
  }

  Node(int value, unsigned char c): value(value), c(c){
    left = right = nullptr;
  }

  bool isLeaf(){
    return left == nullptr and right == nullptr;
  }

};

class NodeComparator{
public:
    bool operator()(Node* a, Node* b){
      return a->value > b->value;
    }
};

namespace Util{
  string charToBinaryString(unsigned char ch){
    string ret = "";
    for(int i = 7 ; i >= 0 ; i--)
        if(1<<i & ch) ret = ret + "1";
        else ret = ret + "0";
    return ret;
  }
  string* toBinaryString(string *s){
    string * ret = new string();
    for(int i = 0 ; i < s->size() ; ++i){
      *ret += charToBinaryString((*s)[i]);
    }
    return ret;
  }
  void print(string *txt, string fname){
    ofstream out;
    out.open(fname);
    out<<*txt;
    out.close();
  }
  string digitToBinaryString(int dig, int lim = 3){
    string ret = "";
    for(int i = lim - 1 ; i >= 0 ; i--)
      if(1<<i & dig) ret = ret + "1";
      else ret = ret + "0";
    return ret;
  }
  string* fix(string *tot){
    string *ret = new string();
    int curr = 0;
    for(int i =0 ;i < tot->size() ;i++){
      if(i and i%8 == 0){
        *ret += string(1,(unsigned char)(curr));
        curr = 0;
      }
      curr<<=1;
      if((*tot)[i] == '1') curr|=1;
    }
    *ret += string(1,(unsigned char)(curr));
    return ret;
  }
  int checkEqualFiles(string *f1, string *f2){
    if(f1->size() != f2->size()) return 0;
    for(int i = 0 ; i < min(f1->size(),f2->size()) ; i++)
      if((*f1)[i] != (*f2)[i]) return 0;
    return 1;
  }
};

/*
* Current Format:
* First 3 bits determines how many extra padding bits
* Then the Decoded Tree: 0 represents parent, 1 represents leaf and followed by
*     8 bits representing the character.
* Then the data itself.
*/

class HuffmanDecoding{
private:
  string *compressed;
  unordered_map<string, unsigned char> codes;
  Node * root;
  void removeExtraPadding(string *s){
    assert(s->size() > 3);
    int ext = ((*s)[0] - '0')*(1<<2) | ((*s)[1] - '0')*2 | ((*s)[2] - '0');
    assert(s->size() > ext);
    while(ext--) s->pop_back();
  }

  int createHuffmanTree(string *s){
    int textStartIndex = dfsBuild(root, s);
   // getCodes(root);
    return textStartIndex;
  }

  void getCodes(Node *root, string st = ""){
    if(root->isLeaf()){
        codes[st] = root->c;
        return;
    }
    getCodes(root->left, st + "0");
    getCodes(root->right, st + "1");
  }

  unsigned char getChar(string *s, int ind){
    int ret = 0;
    for(int i = 7, j = 0 ; i >= 0 ; j++, i--){
        if((*s)[ind+j] == '1') ret |= (1<<i);
    }
    return (unsigned char)ret;
  }

  pair<int, Node*>* solve(string *s, int ind){
    //first -> 1 if leaf, 0 otherwise. second -> node.
    Node *n = new Node();
    pair<int, Node*> ret = make_pair(0, n);
    if((*s)[ind] == '1'){
        n->c = getChar(s, ind + 1);
        ret.first = 1;
    }
    return new pair<int, Node*> (ret);
  }

  int dfsBuild(Node* root, string *s, int ind = 4){
    pair<int, Node*> *ansL = solve(s, ind);
    root->left = ansL->second;
    if(ansL->first) ind += 9;
    else ind++, ind = dfsBuild(root->left, s, ind);
    pair<int, Node*> *ansR = solve(s, ind);
    root->right = ansR->second;
    if(ansR->first) ind += 9;
    else ind++, ind = dfsBuild(root->right, s, ind);
    return ind;
  }

  void revertAndPrint(string *binaryCompressed, int textStart, string* fileName){
    ofstream out;
    out.open(*fileName);
    Node* curr = root;
    for(int i = textStart ; i < binaryCompressed->size() ; i++){
        if((*binaryCompressed)[i] == '0') curr = curr->left;
        else curr = curr->right;
        if(curr->isLeaf()){
            out<<curr->c;
            curr = root;
        }
    }
    out.close();
    return;
}

public:
  HuffmanDecoding(string *s): compressed(s){
    this->root = new Node();
  }

  void decode(string *fileName){
    string* binaryCompressed = Util::toBinaryString(compressed);
    removeExtraPadding(binaryCompressed);
    int textStartIndex = createHuffmanTree(binaryCompressed);
    revertAndPrint(binaryCompressed, textStartIndex, fileName);
    delete (binaryCompressed);
  }

};

class HuffmanEncoding{
private:
  string* input;
  vector<int> freq;
  vector<string> code;
  priority_queue<Node*, vector<Node*>, NodeComparator > pq;
  Node* root;
  long long startSize, endSize;
  void initialize(){
    for(const unsigned char &c : *input){
      freq[c]++;
    }
    for(int i = 0 ; i < 256 ; i++){
       if(!freq[i]) continue;
       pq.push(new Node(freq[i], i));
    }
  }
  void buildTree(){
    while(pq.size() > 1){
      Node* left = pq.top(); pq.pop();
      Node* right = pq.top(); pq.pop();
      pq.push(new Node(left->value + right->value, left, right));
    }
    root = pq.top(), pq.pop();
  }

  void dfs(Node* root, string curr = ""){
    if(root == nullptr) return;
    if(root->isLeaf()){
      if(curr == "") curr = "0"; // To handle case of only 1 character in the file
      code[root->c] = curr;
      return;
    }
    dfs(root->left, curr + "0");
    dfs(root->right, curr + "1");
  }

  void dfsTree(Node *root, string *ret){
    if(root == nullptr) return;
    if(root->isLeaf()){
        (*ret) += "1" + Util::charToBinaryString(root->c);
        return;
    }
    (*ret) += "0";
    dfsTree(root->left, ret);
    dfsTree(root->right, ret);
  }

public:
  HuffmanEncoding(string* input): input(input){
    freq.resize(256,0);
    code.resize(256);
    startSize = input->size();
    endSize = -1;
    root = nullptr;
  }

  string* printEncodedTree(){
    string *ret = new string();
    dfsTree(this->root, ret);
    return ret;
  }

  void printCodes(){
    for(int i = 0; i < 256; i++){
      if(code[i].size()){
        cout<<(unsigned char)(i)<<" "<<code[i]<<"\n";
      }
    }
  }

  void encode(){
    initialize();
    buildTree();
    dfs(root);
  }

  string* getEncodedString(){
    string *overAll = new string();
    for(const unsigned char & c: *input){
      *overAll += code[c];
    }
    return overAll;
  }

  string* getCompressedText(string *tree, string *text){
    int len = (*tree).length() + (*text).length() + 3; //The first 3 bits that represent the number 'extra'.
    int need = 8 - len%8;
    if(need == 8) need = 0;
    string *extra = new string(need,'0');
    string *extraLength = new string(Util::digitToBinaryString(need));
    string *tot = new string();
    *tot = *extraLength + *tree + *text + *extra;
    string *compressed = Util::fix(tot);
    delete (extra);
    delete (extraLength);
    delete (tot);
    return compressed;
  }

  void compress(string *fileName){
    encode();
    string *encodedString = getEncodedString();
    string *encodedTree = printEncodedTree();
    string *compressedText = getCompressedText(encodedTree, encodedString);
    this->endSize = compressedText->size();
    Util::print(compressedText, *fileName);
    delete(encodedString);
    delete(encodedTree);
    delete(compressedText);
    delete(input);
  }

  double getCompressionRatio(){
    return startSize*1.0/endSize;
  }

};

string* getFile(string fileName){
  ifstream in(fileName, ios::in | ios::binary);
  std::ostringstream contents;
  contents << in.rdbuf();
  in.close();
  string *ret = new string();
  *ret = contents.str();
  return ret;
}

void startUp(int choice =  0, string inFile = "", string outFile = ""){
  cout<<"I would like to :\n\
  1) Compress a file\n\
  2) Decompress a file\n";
  cin>>choice;
  getchar(), system("clear");
  cout<<"The file you want to change : "<<endl;
  cin>>inFile;
  system("clear");
  cout<<"What should the new file be called : "<<endl;
  cin>>outFile;
  system("clear");
  cout<<"Working...\n";
  IO;
  auto stNormal = chrono::high_resolution_clock::now();
  switch(choice){
  case 1 :{
    HuffmanEncoding huffman(getFile(inFile));
    huffman.compress(new string(outFile));
    system("clear");
    cout<<"Done!"<<endl<<"Compression Ratio = "<<huffman.getCompressionRatio()<<endl;
    break;
    }
  case 2 :{
    HuffmanDecoding huffman(getFile(inFile));
    huffman.decode(new string(outFile));
    system("clear");
  }
  }
    auto enNormal = chrono::high_resolution_clock::now();
    auto durationNormal = chrono::duration_cast<chrono::milliseconds>(enNormal - stNormal);
    cout<<"Time taken : "<<durationNormal.count()<<" milliseconds"<<endl;
}

void generateRandomFile(string fileName, int len, int mod = 256){
  mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
  ofstream out(fileName);
  for(int i = 0, rand; i < len ; i++, rand = rng()){
    if(rand < 0) rand *= -1;
    rand%=mod;
    out<<(unsigned char)rand;
  }
  out.close();
}

int main(){
  startUp();
  //cout<<Util::checkEqualFiles( getFile("test_huffman.txt"), getFile("tst.txt"));
  //generateRandomFile("in.txt", 2e8);
  //checkEqualFiles(getFile("in.txt"), getFile("output2.txt"));
  return 0;
}
