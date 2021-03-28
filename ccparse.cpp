#include<iostream>
#include<fstream>
#include<ctime>
#include<string>
#include<vector>
#include<unordered_map>
#include<memory>
#include<cassert>
#include<cmath>

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
    assert(context[pos] == '{');
    Json::jsonObj res;
    string key;
    ++pos;
    Skip_Blank();
    if(context[pos]=='}') {
        ++pos;
        StatusCode = PARSE_OK;
        return Json(res);
    }
    while(1) {
        Skip_Blank();
        //std::cout<<pos<<std::endl;
        if(!getRawStr(key)) {
            StatusCode = PARSE_MISS_KEY;
            return Json();
        }
        Skip_Blank();
        if(pos==context.size()||context[pos]!=':') {
            StatusCode = PARSE_MISS_COLON;
            return Json();
        }
        ++pos;
        Skip_Blank();
        res[key] = Parse_Val();
        if(StatusCode!=PARSE_OK) {
            break;
        }
        Skip_Blank();
        if(pos!=context.size()&&context[pos]=='}') {
            ++pos;
            StatusCode = PARSE_OK;
            return Json(res);
        }
        if(pos==context.size()||context[pos]!=',') {
            StatusCode = PARSE_MISS_COMMA_OR_CURLY_BRACKET;
            return Json();
        }
        ++pos;
    }
    StatusCode = PARSE_INVALID_VALUE;
    return Json();
}

Json JsonParser::Parse_Arr() {
    assert(context[pos]=='[');
    Json::jsonArr res;
    ++pos;
    Skip_Blank();
    if(context[pos]==']') {
        ++pos;
        StatusCode = PARSE_OK;
        return Json(res);
    }
    while(1) {
        Skip_Blank();
        Json tmp = Parse_Val();
        if(StatusCode!=PARSE_OK) {
            break;
        }
        res.push_back(tmp);
        Skip_Blank();

        //check , ]
        if(pos!=context.size()&&context[pos]==']') {
            ++pos;
            StatusCode = PARSE_OK;
            return Json(res);
        }

        if(pos==context.size()||context[pos]!=',') {
            StatusCode = PARSE_MISS_COMMA_OR_SQUARE_BRACKET;
            return Json();
        }
        ++pos;
    }
    StatusCode = PARSE_INVALID_VALUE;
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

static bool isNumCh(char ch){
    return (isdigit(ch)) || ch == '+' || ch == '-' || ch == 'e' || ch == 'E' || ch == '.';
}

static bool check(const string &src,int pos,doubleStutus *status){
    *status = DOUBLE_START;
    for (int i = pos; i<src.size()&&isNumCh(src[i]); ++i) {
        *status = changeStatus(*status, src[i]);
        if (*status == DOUBLE_ILLEGAL)
            return 0;
    }
    return istrueEnd(*status);
}


Json JsonParser::Parse_Num() {
    assert(isdigit(context[pos]) || context[pos] == '-');
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
    double num;
    char *end;

    num = strtod(&context[pos], &end);
    if(errno==ERANGE&&num==HUGE_VAL) {
        errno = 0;
        StatusCode = PARSE_NUMBER_TOO_BIG;
        return Json();
    }

    pos += end-&context[pos];
    StatusCode = PARSE_OK;
    return Json(num);
}

static unsigned get4bit(char orgin){
    if(isdigit(orgin))
        return orgin - '0';
    if(orgin>='a'&&orgin<='z')
        return orgin - 'a' + 10;
    return orgin - 'A' + 10;
}

bool JsonParser::getHex(unsigned &u) {
    u = 0;
    for (int i = 0; i < 4;++i) {
        if(!isdigit(context[pos])&&(context[pos]<'a'||context[pos]>'f')&&(context[pos]<'A'||context[pos]>'F')) {
            return false;
        }
        u <<= 4;
        u |= get4bit(context[pos++]);
    }
    return true;
}

Json JsonParser::Parse_Str() {
    assert(context[pos] == '\"');
    string str;
    if(getRawStr(str)) {
        StatusCode = PARSE_OK;
        return Json(str);
    }
    return Json();
}

Json JsonParser::Parse_Null() {
    assert(context[pos] == 'n');
    if(context.size()-pos>=4&&context[pos]=='n'&&context[pos+1]=='u'&&context[pos+2]=='l'&&context[pos+3]=='l') {
        StatusCode = PARSE_OK;
        pos += 4;
        return Json();
    }
    StatusCode = PARSE_INVALID_VALUE;
    return Json();
}

Json JsonParser::Parse_Bool() {
    assert(context[pos] == 'f' || context[pos] == 't');
    if(context.size()-pos>=4&&context[pos]=='t'&&context[pos+1]=='r'&&context[pos+2]=='u'&&context[pos+3]=='e') {
        StatusCode = PARSE_OK;
        pos+=4;
        return Json(true);
    }else if(context.size()-pos>=4&&context[pos]=='f'&&context[pos+1]=='a'&&context[pos+2]=='l'&&context[pos+3]=='s'&&context[pos+4]=='e') {
        StatusCode = PARSE_OK;
        pos += 5;
        return Json(false);
    }
    StatusCode = PARSE_INVALID_VALUE;
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
        case '\"':  return Parse_Str();
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

void JsonParser::Encoding_UTF8(unsigned &u,string &str) {
    if (u >= 0 && u <= 0x007f){
        str.push_back((u & 0x7f));
        return;
    }
    if(u>=0x0080&&u<=0x07ff){
        str.push_back(0xc0 | ((u >> 6) & 0xff));
        str.push_back(0x80 | (u & 0x3f));
        return;
    }

    if(u>=0x0800&&u<=0xffff){
        str.push_back(0xe0 | ((u >> 12) & 0xff));
        str.push_back(0x80 | ((u >> 6) & 0x3F));
        str.push_back(0x80 | (u & 0x3F));
        return;
    }

    if(u>=0x10000&&u<=0x10ffff){
        str.push_back(0xf0 | ((u >> 18) & 0xff));
        str.push_back(0x80 | ((u >> 12) & 0x3f));
        str.push_back(0x80 | ((u >> 6) & 0x3f));
        str.push_back(0x80 | (u & 0x3f));
        return;
    }
}

bool JsonParser::getRawStr(string &str) {
    assert(context[pos] == '\"');
    if(context[pos]!='\"') {
        StatusCode = PARSE_INVALID_VALUE;
        return false;
    }
    str.clear();
    ++pos;
    while(pos<context.size()) {
        char ch = context[pos++];
        switch(ch) {
            case '\"':
                StatusCode = PARSE_OK;
                return true;
            case '\\':
                if(pos>=context.size())
                    break;
                switch(context[pos++]) {
                    //escape
                    case '\"': str.push_back('\"'); break;
                    case '\\': str.push_back('\\'); break;
                    case '/':  str.push_back('/'); break;
                    case 'b':  str.push_back('\b'); break;
                    case 'f':  str.push_back('\f'); break;
                    case 'n':  str.push_back('\n'); break;
                    case 'r':  str.push_back('\r'); break;
                    case 't':  str.push_back('\t'); break;
                    case 'u':
                        unsigned u;
                        if (context.size()-pos<4||!getHex(u)) {
                            StatusCode = PARSE_INVALID_UNICODE_HEX;
                            return false;
                        }
                        if(u>=0xd800&&u<=0xdbff) {
                            //todo get point
                            if(context.size()-pos<6||context[pos]!='\\'||context[pos+1]!='u') {
                                StatusCode = PARSE_INVALID_UNICODE_HEX;
                                return false;
                            }
                            pos += 2;
                            unsigned H = u;
                            if (!getHex(u)||u<0xDC00||u>0xDFFF) {
                                StatusCode = PARSE_INVALID_UNICODE_HEX;
                                return false;
                            }
                            unsigned L = u;
                            u = 0x10000 + ((H - 0xD800) <<10) + (L - 0XDC00);
                        }
                        Encoding_UTF8(u,str);
                        break;
                    default:
                        StatusCode = PARSE_INVALID_STRING_ESCAPE;
                        return false;
                    }
                break;
            default:
                if(ch<0x20) {
                    StatusCode = PARSE_INVALID_STRING_CHAR;
                    return false;
                }
                //normal
                str.push_back(ch);
            }
    }
    StatusCode = PARSE_INVALID_VALUE;
    return false;
}

} //namespace;