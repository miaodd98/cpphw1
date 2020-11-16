#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <stack>
#include <vector>
#include <stdlib.h>

using namespace std;

void trim(string &str); //ɾ���ִ��еĿո�

class CGramCheck
{
public:
    int lines=0;    //����������
    int totalerrs=0;    //�ܴ��������
    int semicolonerrs=0;    //�ֺ�ȱʧ��
    int parathesis=0;   //����ƥ�������
    int quotationerrs=0;    //����ƥ��
    int doublequocnt=0;    //˫����ƥ�����
    int singlequocnt=0;     //������ƥ�����
    int operrs=0;     //������Ŵ�����
    int noteerrs=0;     //ע�ʹ���
    int qflag=0;        //���ű��
    int noteflag=0;     //ע�ͱ��

    stack<char> symbol; //����ջ����������ƥ��
    stack<int> line;    //����ջ�����������λ�ã�����Ƿ���������δƥ��

    CGramCheck();   //���캯��
    void fileCheck(ifstream &is, ofstream &os);    //�����麯��
    void symbolCheck(string str, ofstream &os);   //���з������м��
    void operatorCheck(string str, ofstream &os); //����������
    void writeCheck(ofstream &name);    //���ɼ���ļ�


};
void trim(string &str)      //ɾ������
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

void CGramCheck::fileCheck(ifstream &is, ofstream &os)  //�﷨���
{
    string str;
    int l;
    do  //������������ִ����
    {
        if (is.good())
        {
            lines++;        //ͳ������
            getline(is, str);   //��һ��
            trim(str);      //ɾȥÿ���еĿո�
            symbolCheck(str, os);   //����﷨
            operatorCheck(str, os); //��������ƥ��
        }

    }while(!is.eof());

    if(noteflag==1) //����ע�ͼ��
    {
        noteerrs++;
        cout << "Multi Line Notes Mismatching! " << endl;
        os << "Multi Line Notes Mismatching! " << '\n';
    }
    if(!line.empty())   //δƥ�������ż��
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
        if(!symbol.empty()) //�жϵ�ǰ�Ƿ����δ������ϵĶ���ע��
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
                else        //����ע�ͽ���
                {
                    symbol.pop();
                    noteflag=0;
                    continue;
                }
            }
        }
        if(c == '(' || c == '[' || c== '{')     //��⵽������ţ�ѹջ
        {
            symbol.push(c);
            line.push(lines);
        }
        else if(c == ')' || c== ']' || c=='}')  //��⵽�Ҳ����ţ���ѯջ���ж�
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
            if((c == ')' && temp == '(') || (c == ']' && temp == '[') || (c == '}' && temp == '{')) //��������ƥ��
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
        else if(c == '\'')        //����ƥ��
        {
            singlequocnt++;
        }
        else if(c == '"')
        {
            doublequocnt++;
        }
        else if(c == '/')   //���ע��
        {
            if(str[i+1] != '*' && str[i+1] != '/')     //�Ƕ���ע�ͣ�����
                continue;
            else if(str[i+1] == '/')    //����ע�ͣ���������
                break;
            else if(str[i+1] == '*')    //����ע�����/*��ѹջ
            {
                symbol.push(str[i+1]);
                noteflag=1;
            }
        }
        else if(c == '*' && str[i+1] == '/')    //ƥ��Ķ���ע��
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
    //���ÿһ������Ƿ���ڷֺţ����ǿ������
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
    if(singlequocnt % 2 != 0)   //ÿһ������ⵥ˫����ƥ���������Ϊ���߾��޷����У�ֻ������ִ���
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

void CGramCheck::operatorCheck(string str, ofstream &os)  //�ж�������Ƿ���ȷ
{
    string::size_type posequal, posplus, possub, posmul, posdiv;    //Ѱ���������λ��
    posequal = str.find('=');
    posplus = str.find('+');
    possub = str.find('-');
    posmul = str.find('*');
    posdiv = str.find('/');
    if(posequal != string::npos)    //�и�ֵ��������������ڱ��ʽ
    {
        if(posplus != string::npos)  //�Ӻſ����������
        {
            if(str[posplus+1] != '+' || !isalpha(str[posplus+1]) || !isalpha(str[posplus-1]))
            {
                totalerrs++;
                operrs++;
                cout << "Operator + Mismatch at Line " << lines << endl;
                os << "Operator + Mismatch at Line " << lines << '\n';
            }
        }
        if(possub != string::npos)  //���ſ����Լ����
        {
            if(str[possub+1] != '-' || !isalpha(str[possub+1]) || !isalpha(str[possub-1]))
            {
                totalerrs++;
                operrs++;
                cout << "Operator - Mismatch at Line " << lines << endl;
                os << "Operator - Mismatch at Line " << lines << '\n';
            }
        }
        if(posmul != string::npos)  //�˺�
        {
            if(!isalpha(str[posmul+1]) || !isalpha(str[posmul-1]))
            {
                totalerrs++;
                operrs++;
                cout << "Operator * Mismatch at Line " << lines << endl;
                os << "Operator * Mismatch at Line " << lines << '\n';
            }
        }
        if(posdiv != string::npos)  //���ţ����⿼�ǳ���Ϊ�����
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

void CGramCheck::writeCheck(ofstream &name)     //����checkres.txt
{
    if(!name)
    {
        cout << "Cannot Write File! Exit with exit(1)!" << endl;
        exit(1);
    }
    else    //�ļ������Ϣ
    {
        name << "Total Lines " << lines << '\n';        //����
        name << "Total Errors in File " << totalerrs << '\n';   //�ܴ�����
        name << "Semicolon Loss " << semicolonerrs << '\n';  //�ֺ�ȱʧ��
        name << "Parathesis Mismatching " << parathesis << '\n';    //����ƥ�������
        name << "Quotation Mark Mismatching " << quotationerrs << '\n'; //����ƥ�������
        name << "Multi-Line Note Mismatching " << noteerrs << '\n';     //����ע��ƥ�����
        name << "Operator Mismatching " << operrs << '\n';
    }
}

int main()
{
    string str;
    CGramCheck check;
    ofstream os("checkres.txt");    //���������ļ�
    ifstream is("tobecheck.cpp");    //���Զ�ȡ�ļ�
    if(os.good())           //ɾ������checkres.txt
    {
        remove("checkres.txt");
    }
    if(is.good())       //����ļ��Ƿ����
    {
        check.fileCheck(is, os);
        is.close();
    }
    else //���ļ�ʱֱ���˳�
    {
        cout << "No such file or directory!" <<endl;
        cout << "Stop Running ..." << endl;
        exit(0);
    }
    cout << "Total Errors in File: " << check.totalerrs << endl;    //��������
    check.writeCheck(os);
    os.close();
    cout << "File Checked! " << endl;

    return 0;
}
