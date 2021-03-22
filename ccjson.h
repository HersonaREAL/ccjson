#ifndef ccjson_ccjson_H_
#define ccjson_ccjson_H_
#include<memory>
#include<vector>
#include<string>
#include<unordered_map>
namespace ccjson{

class JsonVal;

class Json {
public:
    enum jsonType{
        NUL, NUMBER, BOOL, STRING, ARRAY, OBJECT
    };

    typedef std::vector<Json> jsonArr;
    typedef std::unordered_map<std::string, Json> jsonObj;
    Json();                         //NULL
    Json(double value);             //NUM
    Json(const std::string &value); //string
    Json(bool value);               //bool
    Json(const jsonArr &value);     //array
    Json(const jsonObj &value);     //object

    //to string
    virtual std::string dump();

    //judge
    bool isObj();
    bool isNum();
    bool isArr();
    bool isStr();
    bool isNull();
    bool isTrue();
    bool isFalse();

    //access
    jsonType Type() const;
    //for obj
    Json &operator[](const std::string &key);
    Json &insert(const std::string &key, const Json &value);
    Json &erase(const std::string &key);

    //for arr
    Json &operator[](std::size_t i);
    Json &insert(std::size_t, const Json &value);
    Json &erase(std::size_t);

    //for string
    std::string &getStr();

    //for num
    double &getNum();

    //for null

    //for true or false
    bool &getBool();



    Json &operator=(const Json &rhs);
private:
    std::shared_ptr<JsonVal> val;
};

class JsonVal {
protected:
    friend class Json;
    virtual Json::jsonType type() const = 0;
    virtual Json &operator[](const std::string &key);
    virtual Json &operator[](std::size_t i);
    virtual Json &insert(std::size_t, const Json &value);
    virtual Json &erase(std::size_t);
    virtual Json &insert(const std::string &key, const Json &value);
    virtual Json &erase(const std::string &key);
    virtual std::string &str_val();
    virtual double &num_val();
    virtual bool &bool_val();
    virtual void put2arr(const Json &value);
    virtual void put2Obj(const Json &value);
    virtual ~JsonVal();
};

class JsonBool final: public JsonVal{
    
};

class JsonNum final: public JsonVal{

};

class JsonStr final: public JsonVal{

};

class JsonNull final: public JsonVal{

};

class JsonArr final: public JsonVal{

};

class JsonObj final: public JsonVal {

};




}//namespace

#endif