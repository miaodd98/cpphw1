#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <stack>
#include <vector>
#include <stdlib.h>

using namespace std;

void trim(string &str); //删除字串中的空格

class CGramCheck
{
public:
    int lines=0;    //行数计数器
    int totalerrs=0;    //总错误计数器
    int semicolonerrs=0;    //分号缺失数
    int parathesis=0;   //括号匹配计数器
    int quotationerrs=0;    //引号匹配
    int doublequocnt=0;    //双引号匹配错误
    int singlequocnt=0;     //单引号匹配错误
    int operrs=0;     //运算符号错误数
    int noteerrs=0;     //注释错误
    int qflag=0;        //引号标记
    int noteflag=0;     //注释标记

    stack<char> symbol; //符号栈，用于括号匹配
    stack<int> line;    //数字栈，存放左括号位置，检查是否有左括号未匹配

    CGramCheck();   //构造函数
    void fileCheck(ifstream &is, ofstream &os);    //整体检查函数
    void symbolCheck(string str, ofstream &os);   //所有符号逐行检查
    void operatorCheck(string str, ofstream &os); //检查运算符号
    void writeCheck(ofstream &name);    //生成检查文件


};
void trim(string &str)      //删除括号
{
    int i = 0;
    if (!str.empty())
    {
        while ((i = str.find(' ', i)) != string::npos)
        {
            str.erase(i, 1);
        }
    }
}

CGramCheck::CGramCheck()
{
    cout << "Initialized! " << endl;
}

void CGramCheck::fileCheck(ifstream &is, ofstream &os)  //语法检查
{
    string str;
    int l;
    do  //逐行输入语句字串检查
    {
        if (is.good())
        {
            lines++;        //统计行数
            getline(is, str);   //读一行
            trim(str);      //删去每行中的空格
            symbolCheck(str, os);   //检查语法
            operatorCheck(str, os); //检查运算符匹配
        }

    }while(!is.eof());

    if(noteflag==1) //跨行注释检查
    {
        noteerrs++;
        cout << "Multi Line Notes Mismatching! " << endl;
        os << "Multi Line Notes Mismatching! " << '\n';
    }
    if(!line.empty())   //未匹配左括号检查
    {
        while(!line.empty())
        {
            parathesis++;
            totalerrs++;
            l=line.top();
            cout << "Wrong Parathesis Matching in Line " << l << endl;
            line.pop();
        }
    }

}


void CGramCheck::symbolCheck(string str, ofstream &os)
{

    int len=str.size();
    char c,temp;

    for (int i=0; i<len;i++)
    {
        c = str[i];
        if(c == '\0')
            break;
        if(!symbol.empty()) //判断当前是否存在未处理完毕的多行注释
        {
            if(symbol.top() == '*' && c != '*')
            {
                continue;
            }
            else if(symbol.top() == '*' && c == '*')
            {
                if(str[i+1] != '/')
                {
                    continue;
                }
                else        //多行注释结束
                {
                    symbol.pop();
                    noteflag=0;
                    continue;
                }
            }
        }
        if(c == '(' || c == '[' || c== '{')     //检测到左侧括号，压栈
        {
            symbol.push(c);
            line.push(lines);
        }
        else if(c == ')' || c== ']' || c=='}')  //检测到右侧括号，查询栈顶判断
        {
            if(symbol.empty())
            {
                parathesis++;
                totalerrs++;
                cout << "Wrong Parathesis Matching in Line " << lines << endl;
                os << "Wrong Parathesis Matching in Line " << lines << '\n';
                continue;
            }
            temp = symbol.top();
            if((c == ')' && temp == '(') || (c == ']' && temp == '[') || (c == '}' && temp == '{')) //左右括号匹配
            {
                symbol.pop();
                line.pop();
            }
            else
            {
                parathesis++;
                totalerrs++;
                cout << "Wrong Parathesis Matching in Line " << lines << endl;
                os << "Wrong Parathesis Matching in Line " << lines << '\n';
            }
        }
        else if(c == '\'')        //引号匹配
        {
            singlequocnt++;
        }
        else if(c == '"')
        {
            doublequocnt++;
        }
        else if(c == '/')   //检测注释
        {
            if(str[i+1] != '*' && str[i+1] != '/')     //非多行注释，继续
                continue;
            else if(str[i+1] == '/')    //单行注释，跳过该行
                break;
            else if(str[i+1] == '*')    //多行注释情况/*，压栈
            {
                symbol.push(str[i+1]);
                noteflag=1;
            }
        }
        else if(c == '*' && str[i+1] == '/')    //匹配的多行注释
        {
            if(symbol.empty())
            {
                noteerrs++;
                totalerrs++;
                cout << "Multi-Line Notes Mismatch at Line " << lines << endl;
                os << "Multi-Line Notes Mismatch at Line " << lines << '\n' ;
            }
            else
            {
                temp=symbol.top();
                if(temp!= '*')
                {
                    noteerrs++;
                    totalerrs++;
                    cout << "Multi-Line Notes Mismatch at Line " << lines << endl;
                    os << "Multi-Line Notes Mismatch at Line " << lines << '\n';
                }
            }

        }

    }
    //检测每一行最后是否存在分号，考虑空行情况
    if(str[len-1] != ';' && len > 0 && str[len-1] != '}' && str[len-1] != ')' && str[0] != '#' && str[len-1] != '>' && str[len-1] != '{' && str[len-1] != '/' && str[len-1] != ':')
    {
        if(noteflag != 1)
        {
            totalerrs++;
            semicolonerrs++;
            cout << "No semicolon at the end of line " << lines << endl;
            os << "No semicolon at the end of line " << lines << '\n';
        }
    }
    if(singlequocnt % 2 != 0)   //每一行最后检测单双引号匹配情况，因为二者均无法跨行，只需检测出现次数
    {
        cout << "Single Quotation Mask Mismatch at line " << lines << endl;
        os << "Single Quotation Mask Mismatch at line " << lines << '\n';
        quotationerrs++;
        totalerrs++;
    }
    if(doublequocnt % 2 !=0)
    {
        cout << "Double Quotation Mask Mismatch at line " << lines << endl;
        os << "Double Quotation Mask Mismatch at line " << lines << '\n';
        quotationerrs++;
        totalerrs++;
    }
    singlequocnt=0;
    doublequocnt=0;
}

void CGramCheck::operatorCheck(string str, ofstream &os)  //判断运算符是否正确
{
    string::size_type posequal, posplus, possub, posmul, posdiv;    //寻找运算符号位置
    posequal = str.find('=');
    posplus = str.find('+');
    possub = str.find('-');
    posmul = str.find('*');
    posdiv = str.find('/');
    if(posequal != string::npos)    //有赋值运算符，表明存在表达式
    {
        if(posplus != string::npos)  //加号考虑自增情况
        {
            if(str[posplus+1] != '+' || !isalpha(str[posplus+1]) || !isalpha(str[posplus-1]))
            {
                totalerrs++;
                operrs++;
                cout << "Operator + Mismatch at Line " << lines << endl;
                os << "Operator + Mismatch at Line " << lines << '\n';
            }
        }
        if(possub != string::npos)  //减号考虑自减情况
        {
            if(str[possub+1] != '-' || !isalpha(str[possub+1]) || !isalpha(str[possub-1]))
            {
                totalerrs++;
                operrs++;
                cout << "Operator - Mismatch at Line " << lines << endl;
                os << "Operator - Mismatch at Line " << lines << '\n';
            }
        }
        if(posmul != string::npos)  //乘号
        {
            if(!isalpha(str[posmul+1]) || !isalpha(str[posmul-1]))
            {
                totalerrs++;
                operrs++;
                cout << "Operator * Mismatch at Line " << lines << endl;
                os << "Operator * Mismatch at Line " << lines << '\n';
            }
        }
        if(posdiv != string::npos)  //除号，额外考虑除数为零情况
        {
            if(!isalpha(str[posdiv+1]) || !isalpha(str[posdiv-1]))
            {
                totalerrs++;
                operrs++;
                cout << "Operator / Mismatch at Line " << lines << endl;
                os << "Operator / Mismatch at Line " << lines << '\n';
            }
            if(str[posdiv+1] == '0')
            {
                totalerrs++;
                operrs++;
                cout << "Divisor is 0 at Line " << lines << endl;
                os << "Divisor is 0 at Line " << lines << '\n';
            }
        }
    }
}

void CGramCheck::writeCheck(ofstream &name)     //保存checkres.txt
{
    if(!name)
    {
        cout << "Cannot Write File! Exit with exit(1)!" << endl;
        exit(1);
    }
    else    //文件检测信息
    {
        name << "Total Lines " << lines << '\n';        //行数
        name << "Total Errors in File " << totalerrs << '\n';   //总错误数
        name << "Semicolon Loss " << semicolonerrs << '\n';  //分号缺失数
        name << "Parathesis Mismatching " << parathesis << '\n';    //括号匹配错误数
        name << "Quotation Mark Mismatching " << quotationerrs << '\n'; //引号匹配错误数
        name << "Multi-Line Note Mismatching " << noteerrs << '\n';     //多行注释匹配错误
        name << "Operator Mismatching " << operrs << '\n';
    }
}

int main()
{
    string str;
    CGramCheck check;
    ofstream os("checkres.txt");    //保存检测结果文件
    ifstream is("tobecheck.cpp");    //尝试读取文件
    if(os.good())           //删除现有checkres.txt
    {
        remove("checkres.txt");
    }
    if(is.good())       //检查文件是否存在
    {
        check.fileCheck(is, os);
        is.close();
    }
    else //无文件时直接退出
    {
        cout << "No such file or directory!" <<endl;
        cout << "Stop Running ..." << endl;
        exit(0);
    }
    cout << "Total Errors in File: " << check.totalerrs << endl;    //错误总数
    check.writeCheck(os);
    os.close();
    cout << "File Checked! " << endl;

    return 0;
}
