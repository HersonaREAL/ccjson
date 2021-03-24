#include<iostream>
#include<string>
#include"ccjson.h"
using namespace std;
using namespace ccjson;

int main(){
    Json a = Json::jsonObj{
        {"abc", "cde"},
        {"acc", false},
        {"ccd","sdkfjsdlfs"},
        {"ggd",1.123},
        {"sss",Json::jsonArr{1.2,2.3,"abc",true}},
        {"empty",Json()},
        {"JsonOBJ",Json::jsonArr{"testOBJBOJ","kcj"}}
    };
    Json b = Json::jsonObj{
        {"abc", "cde"},
        {"acc", false},
        {"ccd","sdkfjsdlfs"},
        {"ggd",1.123},
        {"sss",Json::jsonArr{1.2,2.3,"abc",true,a}},
        {"empty",Json()},
        {"JsonOBJ",Json::jsonObj{
            {"TESTARR",Json::jsonArr{3.33,6.666,Json(),"arrvalue"}}
        }}
    };
    cout << b.dump() << endl;
}