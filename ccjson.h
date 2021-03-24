#ifndef ccjson_ccjson_H_
#define ccjson_ccjson_H_
#include<memory>
#include<vector>
#include<string>
#include<unordered_map>
namespace ccjson{

class JsonVal;

class Json final{
public:
    enum jsonType{
        NUL, NUMBER, TRUE,FALSE, STRING, ARRAY, OBJECT
    };

    typedef std::vector<Json> jsonArr;
    typedef std::unordered_map<std::string, Json> jsonObj;
    Json();                         //NULL
    Json(double value);             //NUM
    Json(const std::string &value); //string
    Json(const char *value);        //string
    Json(bool value);               //bool
    Json(const jsonArr &value);     //array
    Json(const jsonObj &value);     //object
    //Json(void *) = delete;//防止 Json a = "cdslfjsl",转换成bool类型
    //to string
    std::string dump() const;

    //judge
    bool isObj() { return Type() == OBJECT; }
    bool isNum() { return Type() == NUMBER; }
    bool isArr() { return Type() == ARRAY; }
    bool isStr() { return Type() == STRING; }
    bool isNull() { return Type() == NUL; }
    bool isTrue() { return Type() == TRUE; }
    bool isFalse() { return Type() == FALSE; }

    //access
    jsonType Type() const;
    //for obj
    Json &operator[](const std::string &key);
    Json &insert(const std::string &key, const Json &value);
    Json &erase(const std::string &key);

    //for arr
    Json &operator[](std::size_t i);
    Json &insert(std::size_t i, const Json &value);
    Json &erase(std::size_t i);

    //for string
    std::string &getStr();

    //for num
    double &getNum();

    //for null

    //for true or false
    bool &getBool();



    //Json &operator=(const Json &rhs);
private:
    //pimpl idiom
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
    virtual void dump(std::string &res) const = 0;
    virtual ~JsonVal() = default;
};

}//namespace

#endif