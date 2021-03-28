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
    //考虑右值
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
    //obj,考虑右值
    virtual Json &operator[](const std::string &key);
    virtual void insert(const std::string &key, const Json &value);
    virtual void erase(const std::string &key);
    //arr,考虑右值
    virtual Json &operator[](std::size_t i);
    virtual void insert(std::size_t i, const Json &value);
    virtual void erase(std::size_t i);
    //normal
    virtual std::string &str_val();
    virtual double &num_val();
    virtual bool &bool_val();
    virtual void dump(std::string &res) const = 0;
    virtual ~JsonVal() = default;
    virtual std::size_t size() const;
};

class JsonParser final {
public:
    enum err{
        NOTHING,
        PARSE_OK,
        PARSE_EXPECT_VALUE,
        PARSE_INVALID_VALUE,
        PARSE_ROOT_NOT_SINGULAR,
        PARSE_NUMBER_TOO_BIG,
        PARSE_MISS_QUOTATION_MARK,
        PARSE_INVALID_STRING_ESCAPE,
        PARSE_INVALID_STRING_CHAR,
        PARSE_INVALID_UNICODE_HEX,
        PARSE_INVALID_UNICODE_SURROGATE,
        PARSE_MISS_COMMA_OR_SQUARE_BRACKET,
        PARSE_MISS_KEY,
        PARSE_MISS_COLON,
        PARSE_MISS_COMMA_OR_CURLY_BRACKET
    };

    Json Parse(const std::string &c) { std::string empty; return Parse(c,empty); }
    Json Parse(const std::string &c, std::string &errMsg);
    err getStatusCode() { return StatusCode; }
    Json operator()(const std::string &c) { return Parse(c); }
    bool ParseOK() { return StatusCode == PARSE_OK; }
    std::size_t getPos() { return pos; }
private:
    Json Parse_Obj();
    Json Parse_Arr();
    Json Parse_Num();
    Json Parse_Str();
    Json Parse_Null();
    Json Parse_Bool();
    Json Parse_Val();
    void Skip_Blank();
    bool Is_Blank(char ch);
    void fillErrMsg(std::string &err);
    void Encoding_UTF8(unsigned &u,std::string &str);
    bool getRawStr(std::string &str);
    bool getHex(unsigned &u);
    std::string context;
    std::size_t pos;
    err StatusCode = NOTHING;
};

}//namespace

#endif