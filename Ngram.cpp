#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <ranges>

template <typename T>
class CircularBuffer {
public:
    std::vector<T> buffer;
    int size;
    int pos;

public:
    CircularBuffer(int size) : buffer(size), size(size), pos(0){}
    void push(T elem){
        buffer[pos] = elem;
        pos = (pos + 1) % size;
    }

    std::vector<T> enumerate(){
        std::vector<T> out(size);
        for(int start = 0 ; start < size ; start++)
            out[start] = buffer[(pos+start)%size];
        return out;
    }
};

class Tokenizer{
    std::ifstream in_file;
    std::istringstream in_buffer;

public:
    Tokenizer(std::string filename){
        in_file.open(filename);
    }

    std::string get_word(){
        in_buffer>>std::ws;
        while(in_buffer.eof()){
            std::string temp;
            getline(in_file,temp); //gets a line
            in_file>>std::ws;
            in_buffer.str(temp);
            in_buffer.clear(); //not eof anymore
            //it's possible that this line is empty, do we need to check it?
            //Or can we be sure that our corpus will not have such cases?
        }
        std::string out;
        in_buffer>>out;
        return out;
    }

    bool has_more() {
        if (in_file.eof() && in_buffer.eof()) return false;
        return true;
    }
};

class NGramBuffer : public CircularBuffer<std::string> {
    std:: string str_pad = "<s>";

public:
    NGramBuffer(int size) : CircularBuffer(size){
        for(int i =0;i<size;i++)
            push(str_pad);
    }

};
int main(){
    const int ngram_size = 3;
    NGramBuffer ngram_buffer(ngram_size);
    Tokenizer tokens("sample.txt");
    std::unordered_map<std::string, std::unordered_map<std::string, int>> final_str;

    /*for(int i = 0 ; i < 3 ; i++)
        ngram_buffer.push(tokens.get_word());*/
    while(tokens.has_more()) {
        std::string fourth = tokens.get_word();
        auto words = ngram_buffer.enumerate();
      //  std::cout<<words[0];
        std::string ngram = words[0];
        for(auto& elem : words | std::ranges::views::drop(1))
                ngram+=" " + elem;
        final_str[ngram][fourth]++;
        ngram_buffer.push(fourth);
    }
    std:: string end_pad = "</s>";
    for(int pos = ngram_size;pos>0;pos--) {
        auto words = ngram_buffer.enumerate();
        //  std::cout<<words[0];
        std::string ngram = words[0];
        for(auto& elem : words | std::ranges::views::drop(1))
            ngram+=" " + elem;
        final_str[ngram][end_pad]++;
        ngram_buffer.push(end_pad);
    }

    for(auto itr = final_str.begin(); itr!= final_str.end();itr++) {
        for (auto ptr = itr->second.begin(); ptr != itr->second.end(); ptr++) {
            std::cout << itr->first << "\t" << ptr->first << " " << ptr->second << std::endl;
        }
    }
    for(auto& [key,value]:final_str){
        int count=0;
        for(auto itr = value.begin(); itr!= value.end();itr++) {
            count += itr->second;
        }
        for(auto ptr= value.begin();ptr!= value.end();ptr++)
            std::cout<<key<<"\n"<<ptr->first<<" "<<float(ptr->second)/count<<std::endl;

    }
return 0;
}