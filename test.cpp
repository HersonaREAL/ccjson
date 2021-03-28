#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<ctime>
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
    Json numj = jp.Parse("  2e4000  ");
    cout << (numj.Type() == Json::NUMBER ? "NUMBER" : "ERR") << endl;
    cout << jp.getStatusCode() << endl;
    cout << numj.getNum() << endl<<endl;

    Json t = jp.Parse("   true  ");
    cout << (t.Type() == Json::TRUE ? "TRUE" : "ERR") << endl;
    cout << jp.getStatusCode() << endl;
    cout << t.getBool() << endl<<endl;

    Json f = jp.Parse("   false  ");
    cout << (f.Type() == Json::FALSE ? "FALSE" : "ERR") << endl;
    cout << jp.getStatusCode() << endl;
    cout << f.getBool() << endl<<endl;

    Json nul = jp.Parse("   null  ");
    cout << (nul.Type() == Json::NUL ? "NULL" : "ERR") << endl;
    cout << jp.getStatusCode() << endl<<endl;

    Json str = jp.Parse("\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
    cout << (str.Type()==Json::STRING?"STRING":"ERR")<<endl;
    cout << jp.getStatusCode() << endl;
    cout << str.getStr()<<endl<<endl;

    Json arr = jp.Parse("[ 1.23 ,[   ] ,[[[[[]]]]],5.56 , 50e-2,[[[[1]]]] ,true, false , null,[1.63, \"\\ud834\\udd1e \",[true,false,1.333] ]]");
    cout << (arr.Type() == Json::ARRAY ? "ARRAY" : "ERR") << endl;
    cout << jp.getStatusCode() << endl;
    cout << arr.dump() << endl<<endl;

    Json obj = jp.Parse("{\"arr1\"  : [ 1.23 ,[ {\"BOOL\": true,\"bool2\" : false ,\"null\":null}  ] ,{\"empty\" : {\"empty\" : {\"empty\" : {}}}},[[[[[]]]]],5.56 , 50e-2,[[[[1]]]] ,true, false , null,[1.63, \"\\ud834\\udd1e \",[true,false,1.333] ]] ,\"str1\" : \" i am a str\"}");
    cout << (obj.Type() == Json::OBJECT ? "OBJECT" : "ERR") << endl;
    cout << jp.getStatusCode() << endl;
    cout << obj.dump() << endl<<endl;

    // int count;
    // cout << "loop :";
    // cin >> count;
    // clock_t start = clock();
    // for (int i = 0; i < count;++i)
    //     jp.Parse("{\"arr1\"  : [ 1.23 ,[ {\"BOOL\": true,\"bool2\" : false ,\"null\":null}  ] ,{\"empty\" : {\"empty\" : {\"empty\" : {}}}},[[[[[]]]]],5.56 , 50e-2,[[[[1]]]] ,true, false , null,[1.63, \"\\ud834\\udd1e \",[true,false,1.333] ]] ,\"str1\" : \" i am a str\"}");
    // clock_t end = clock();
    // cout<<"loop "<<count<<" times, " << "time spend : " << ((double)(end - start)/CLOCKS_PER_SEC)*1000 << " ms" << endl;
    ifstream ifs("canada.json");
    if(!ifs){
        cout << "fail";
        return -1;
    }
    stringstream ss;
    ss << ifs.rdbuf();
    string k = ss.str();
    clock_t s = clock();

    //for (int i = 0; i < 1000;++i)
        jp.Parse(k);

    clock_t e = clock();
    double spt = ((double)(e - s) / CLOCKS_PER_SEC);
    cout << "speed: " << (2.14)/spt<<" MB/S" << endl;
}