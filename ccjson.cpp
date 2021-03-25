#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<unordered_map>
#include<memory>
#include<cassert>

#include"ccjson.h"

namespace ccjson {
using std::shared_ptr;
using std::string;
using std::unordered_map;
using std::vector;
using std::fstream;
using std::ostream;


//黑洞类，吸收错误,单例模式
class blackHole;
static blackHole &BlackHole();

class blackHole final{
public:
    void clear() {
        emptyNum = 0.0;
        emptyBool = false;
        emptyStr.clear();
        emptyArr.clear();
        emptyObj.clear();
    }
    double &getNum() { return emptyNum; }
    bool &getBool() { return emptyBool; }
    string &getStr() { return emptyStr; }
    vector<Json> &getArr() { return emptyArr; }
    unordered_map<string, Json> &getObj() { return emptyObj; }
    Json &getJson() { return emptyJson; }

private:
    friend blackHole &BlackHole();
    ~blackHole() = delete;
    blackHole():emptyNum(0.0),emptyBool(false){}
    double emptyNum;
    bool emptyBool;
    string emptyStr;
    vector<Json> emptyArr;
    unordered_map<string,Json> emptyObj;
    Json emptyJson;
};

static blackHole &BlackHole(){
    static blackHole *bh;
    if(bh==nullptr)
        bh = new blackHole();
    return *bh;
}

//====================接口JsonVal函数定义=========================
Json & JsonVal::operator[](const std::string &key) {
    BlackHole().clear();
    return BlackHole().getJson();
}
Json & JsonVal::operator[](std::size_t i) {
    BlackHole().clear();
    return BlackHole().getJson();
}
void JsonVal::insert(std::size_t, const Json &value) {

}
void JsonVal::erase(std::size_t) {

}
void JsonVal::insert(const std::string &key, const Json &value) {

}
void JsonVal::erase(const std::string &key) {

}
std::string & JsonVal::str_val() {
    BlackHole().clear();
    return BlackHole().getStr();
}
double & JsonVal::num_val() {
    BlackHole().clear();
    return BlackHole().getNum();
}
bool & JsonVal::bool_val() {
    BlackHole().clear();
    return BlackHole().getBool();
}



//=======================继承JsonVal接口子类的定义======================
class JsonBool final: public JsonVal{
public:
    explicit JsonBool(bool value) : val(value){}
    Json::jsonType type() const override{ return val ? Json::TRUE : Json::FALSE; }
    void dump(std::string &res) const override;
    bool &bool_val() override { return val; }
private:
    bool val;
};


class JsonNum final: public JsonVal{
public:
    explicit JsonNum(double value):val(value){}
    Json::jsonType type() const override { return Json::NUMBER; }
    void dump(std::string &res) const override;
    double &num_val() override { return val; }
private:
    double val;
};


class JsonStr final: public JsonVal{
public:
    explicit JsonStr(const string &value):val(value){}
    Json::jsonType type() const override { return Json::STRING; }
    void dump(std::string &res) const override;
    string &str_val() override { return val; }
private:
    string val;
};


class JsonNull final: public JsonVal{
public:
    explicit JsonNull() = default;
    Json::jsonType type() const override { return Json::NUL; }
    void dump(std::string &res) const override;
private:
};


class JsonArr final: public JsonVal{
public:
    explicit JsonArr(const Json::jsonArr &value):val(value){}
    Json::jsonType type() const override { return Json::ARRAY; }
    void dump(std::string &res) const override;
    Json &operator[](std::size_t i) override;
    void insert(std::size_t i, const Json &value) override;
    void erase(std::size_t i) override;
private:
    Json::jsonArr val;
};


class JsonObj final: public JsonVal {
public:
    explicit JsonObj(const Json::jsonObj &value) : val(value) {}
    Json::jsonType type() const override { return Json::OBJECT; }
    void dump(std::string &res) const override;
    Json &operator[](const std::string &key) override;
    void insert(const std::string &key, const Json &value) override;
    void erase(const std::string &key) override;
private:
    Json::jsonObj val;
};


//========================将Json解析成Json原文======================
static void dumpStr(std::string &res,const std::string target){
    res += "\"" + target + "\"";
}

void JsonObj::dump(std::string &res) const {
    res +='{';
    for (auto it = val.cbegin(); it != val.cend();++it){
        dumpStr(res, it->first);
        res += ':';
        res += it->second.dump();
        res += ',';
    }
    res.pop_back();
    res += '}';
}
void JsonArr::dump(std::string &res) const {
    res += '[';
    for (std::size_t i = 0; i < val.size();++i){
        res += val[i].dump();
        res += ',';
    }
    res.pop_back();
    res += ']';
}
void JsonNum::dump(std::string &res) const {
    res += std::to_string(val);
}
void JsonBool::dump(std::string &res) const {
    res += val ? "true" : "false";
}
void JsonNull::dump(std::string &res) const {
    res += "null";
}
void JsonStr::dump(std::string &res) const {
    //需添加转义
    dumpStr(res, val);
}

//============================访问成员函数==========================
//arr
Json &JsonArr::operator[](std::size_t i) {
    if(i>=val.size()){
        BlackHole().clear();
        return BlackHole().getJson();
    }
    return val[i];
}

void JsonArr::insert(std::size_t i, const Json &value) {
    if(i>=val.size())
        val.push_back(value);
    else
        val.insert(val.begin()+i, value);
}

void JsonArr::erase(std::size_t i) {
    if(i>=val.size())
        val.pop_back();
    else
        val.erase(val.begin() + i);
}

//obj
Json &JsonObj::operator[](const std::string &key) {
    if(val.find(key)==val.end()){
        BlackHole().clear();
        return BlackHole().getJson();
    }
    return val[key];
}

void JsonObj::insert(const std::string &key, const Json &value) {
    val[key] = value;
}

void JsonObj::erase(const std::string &key) {
    val.erase(key);
}

//=============================JSON某些函数定义======================
//构造函数
Json::Json()                        :val(new JsonNull()) {}
Json::Json(double value)            :val(new JsonNum(value)) {}
Json::Json(const std::string &value): val(new JsonStr(value)) { }
Json::Json(const char *value)       :val(new JsonStr(string(value))){}
Json::Json(bool value)              :val(new JsonBool(value)) {}
Json::Json(const jsonArr &value)    :val(new JsonArr(value)) {}
Json::Json(const jsonObj &value)    :val(new JsonObj(value)) {}


Json::jsonType Json::Type() const { 
    return val->type(); 
}
std::string Json::dump() const{
    string res;
    val->dump(res);
    return res;
}

//for obj,考虑下containkey?
Json &Json::operator[](const std::string &key) {
    if(!isObj())
        return *this;
    return (*val)[key];
}

Json &Json::insert(const std::string &key, const Json &value) {
    val->insert(key, value);
    return *this;
}

Json &Json::erase(const std::string &key) {
    val->erase(key);
    return *this;
}

//for arr
Json &Json::operator[](std::size_t i) {
    if(!isArr())
        return *this;
    return (*val)[i];
}

Json &Json::insert(std::size_t i , const Json &value) {
    val->insert(i, value);
    return *this;
}

Json &Json::erase(std::size_t i) {
    val->erase(i);
    return *this;
}

//for string
std::string &Json::getStr() {
    return val->str_val();
}

//for num
double &Json::getNum() {
    return val->num_val();
}

//for true or false
bool &Json::getBool() {
    return val->bool_val();
}



}//namespace