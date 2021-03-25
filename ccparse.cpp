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

//==============解析函数相关================
Json JsonParser::Parse(const string &c, string &errMsg) {
    context = c;
    pos = 0;
    StatusCode = NOTHING;
    if(context.empty()) {
        errMsg = "input empty";
        return Json();
    }
    Skip_Blank();
    Json ret = Parse_Val();
    if(StatusCode!=PARSE_OK){
        fillErrMsg(errMsg);
        return Json();
    }

    Skip_Blank();
    if(pos!=context.size()){
        StatusCode = PARSE_ROOT_NOT_SINGULAR;
        fillErrMsg(errMsg);
        return Json();
    }

    return ret;
}

Json JsonParser::Parse_Obj() {
    return Json();
}

Json JsonParser::Parse_Arr() {
    return Json();
}

typedef enum
{
    DOUBLE_ILLEGAL = 0,
    DOUBLE_START,
    DOUBLE_SIGN,
    DOUBLE_ZERO,
    DOUBLE_DIGIT,
    DOUBLE_POINT,
    DOUBLE_POINT_DIGIT,
    DOUBLE_E,
    DOUBLE_E_SIGN,
    DOUBLE_E_DIGIT
}
doubleStutus;

//数字状态机
static doubleStutus changeStatus(doubleStutus status,char input){
    switch (status){
        case DOUBLE_START:{
            if(input=='-')
                return DOUBLE_SIGN;
            else if(input>='1'&&input<='9')
                return DOUBLE_DIGIT;
            else if (input == '0')
                return DOUBLE_ZERO;
            break;
        }
        case DOUBLE_SIGN :{
            if(input=='0')
                return DOUBLE_ZERO;
            if(input>='1'&&input<='9')
                return DOUBLE_DIGIT;
            break;
        }
        case DOUBLE_ZERO :{
            if(input=='.')
                return DOUBLE_POINT;
            else if(input=='e'||input=='E')
                return DOUBLE_E;
            break;
        }
        case DOUBLE_DIGIT:{
            if(input>='0'&&input<='9')
                return DOUBLE_DIGIT;
            else if(input=='.')
                return DOUBLE_POINT;
            else if(input=='e'||input=='E')
                return DOUBLE_E;
            break;
        }
        case DOUBLE_POINT:{
            if(input>='0'&&input<='9')
                return DOUBLE_POINT_DIGIT;
            break;
        }
        case DOUBLE_POINT_DIGIT:{
            if (input >= '0' && input <= '9')
                return DOUBLE_POINT_DIGIT;
            else if(input=='e'||input=='E')
                return DOUBLE_E;
            break;
        }
        case DOUBLE_E:{
            if(input=='+'||input=='-')
                return DOUBLE_E_SIGN;
            else if(input>='0'&&input<='9')
                return DOUBLE_E_DIGIT;
            break;
        }
        case DOUBLE_E_SIGN:{
            if(input>='0'&&input<='9')
                return DOUBLE_E_DIGIT;
            break;
        }
        case DOUBLE_E_DIGIT:{
            if(input>='0'&&input<='9')
                return DOUBLE_E_DIGIT;
            break;
        }
        default:
            return DOUBLE_ILLEGAL;
    }
    return DOUBLE_ILLEGAL;
}

static bool istrueEnd(doubleStutus status){
    return status == DOUBLE_DIGIT || status == DOUBLE_POINT_DIGIT || status == DOUBLE_E_DIGIT||status==DOUBLE_ZERO;
}

static bool check(const string &src,int i,doubleStutus *status){
    *status = DOUBLE_START;
    for (i = 0; (isdigit(src[i])) || src[i] == '+' || src[i] == '-' || src[i] == 'e' || src[i] == 'E' || src[i] == '.'; ++i)
    {
        *status = changeStatus(*status, src[i]);
        if (*status == DOUBLE_ILLEGAL)
            return 0;
    }
    return istrueEnd(*status);
}


Json JsonParser::Parse_Num() {
    assert(isdigit(context[pos]));
    doubleStutus status = DOUBLE_START;
    if(!check(context,pos,&status)) {
        if(context[pos]=='0'&&pos+1<context.size()&&context[pos+1]=='.'){
            StatusCode = PARSE_ROOT_NOT_SINGULAR;
            return Json();
        }
        StatusCode = PARSE_INVALID_VALUE;
        return Json();
    }

    if(status==DOUBLE_ZERO&&pos+1<context.size()&&!isblank(context[pos+1])){
        StatusCode = PARSE_ROOT_NOT_SINGULAR;
        return Json();
    }
    std::size_t end;
    double num;
    try{
        num = std::stod(context.substr(pos), &end);
    }catch(std::out_of_range){
        StatusCode = PARSE_NUMBER_TOO_BIG;
        return Json();
    }
    catch (std::invalid_argument){
        StatusCode = PARSE_EXPECT_VALUE;
        return Json();
    }
    pos = end;
    StatusCode = PARSE_OK;
    return Json(num);
}

Json JsonParser::Parse_Str() {
    return Json();
}

Json JsonParser::Parse_Null() {
    return Json();
}

Json JsonParser::Parse_Bool() {
    return Json();
}

Json JsonParser::Parse_Val() {
    if(pos==context.size()) {
        StatusCode = PARSE_EXPECT_VALUE;
        return Json();
    }
    switch(context[pos]) {
        case 't': 
        case 'f':  return Parse_Bool();
        case 'n':  return Parse_Null();
        default:   return Parse_Num();
        case '"':  return Parse_Str();
        case '[':  return Parse_Arr();
        case '{':  return Parse_Obj();
    }
}

void JsonParser::Skip_Blank() {
    while(pos<context.size()&&(context[pos]==' '||context[pos]=='\t'||context[pos]=='\n'||context[pos]=='\r'))
        ++pos;
}

bool JsonParser::Is_Blank(char ch) {
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

void JsonParser::fillErrMsg(std::string &err) {

}

void JsonParser::Encoding_UTF8() {

}

string JsonParser::getRawStr() {
    return "";
}

} //namespace;