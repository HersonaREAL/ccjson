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
    cout << b["ccd"].getStr() << endl;
    cout << b["JsonOBJ"]["TESTARR"][1].getNum() << endl;
    cout << b.insert("testInsert", "insertSuccess")["testInsert"].getStr() << endl;

    cout << (b["acc"].Type()==Json::FALSE) << endl;
    cout << b["testInsert"].getStr() << endl;
    cout << b.insert("acc", "insertacc")["acc"].getStr() << endl;
    cout << (b["acc"].Type() == Json::STRING) << endl;

    cout << "b[\"sss\"].erase(4);" << endl;
    b["sss"].erase(4);
    cout << b.dump() << endl
         << endl;

    cout << "b.erase(\"sss\");" << endl;
    b.erase("sss");
    cout << b.dump() << endl
         << endl;

    cout << "b.erase(\"JsonOBJ\");" << endl;
    b.erase("JsonOBJ");
    cout << b.dump() << endl<<endl;

    JsonParser jp;
    Json numj = jp.Parse("3.1415");
    cout << (numj.Type() == Json::NUMBER ? "true" : "false") << endl;
    cout << numj.Type() << endl;
    cout << jp.getStatusCode() << endl;
    cout << numj.getNum() << endl;
}