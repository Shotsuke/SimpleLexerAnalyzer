#ifndef LEXERTREEANALYZE_H
#define LEXERTREEANALYZE_H

#endif // LEXERTREEANALYZE_H

#include <iostream>
#include <string.h>
#include <map>
#include <vector>
#include <algorithm>
#include <QVector>
#include <stack>
#include <QDebug>
#include <QTextStream>
#include <QString>



using namespace std;

extern QString lexerAnalyzeResult;


class Token
{
public:
    enum TokenType
    {
        Identifier,
        Integer,
        Plus,
        Minus,
        Multiple,
        Divide,
        Equal,
        DoubleEqual,
        Lparen,
        Rparen,
        Bigger,
        Smaller,
        Question,
        Colon,
        Semicolon,
        Output,
        Error,
        InvalidToken,
        EndOfInput
    };
    TokenType   type;
    string      name;
    int         val;

    Token(TokenType type)
        :
        type(type), name(""), val(0) {}

    Token(TokenType type, string name)
        :
        type(type), name(name), val(0) {}

    Token(TokenType type, int val)
        :
        type(type), name(""), val(val) {}

    void showMySelf()
    {
        cout << type << "\t";

        if (type == Identifier)
            cout << name;

        if (type == Integer)
            cout << val;

        cout << endl;
        return;
    }

};


class Expression
{
public:
    enum ExpType
    {
        IdentifierExp,
        IntegerExp,
        UnaryExp,
        BinaryExp,
        EqualExp,
        TernaryExp,
        StopExp
    };
    ExpType     type;
    string      name    = " ";
    int         val     = 0;
    Token*      op      = nullptr;
    Token*      op2     = nullptr;
    Expression* left    = nullptr;
    Expression* mid     = nullptr;
    Expression* right   = nullptr;

    static map<string, int> varieties;

    Expression(ExpType type)
        :
        type(type) {}

    Expression(ExpType type, string name)
        :
        type(type), name(name) {}

    Expression(ExpType type, int val)
        :
        type(type), val(val) {}

    Expression(ExpType type, Token* op, Expression* right)
        :
        type(type), op(op), right(right) {}

    Expression(ExpType type, Token* op, Expression* left, Expression* right)
        :
        type(type), op(op), left(left), right(right) {}

    Expression(ExpType type, Expression* left, Token* op, Expression* mid,
               Token* op2, Expression* right)
        :
        type(type), op(op), op2(op2), left(left), mid(mid), right(right) {}

    virtual ~Expression()
    {
        delete op;
        delete left;
        delete right;
    }

    virtual int calculate() = 0;
};


class IdentifierExp : public Expression
{
public:
    IdentifierExp(ExpType type, string name)
        :
        Expression(type, name)
    {
        if (Expression::varieties.count(name) == 0) Expression::varieties[name] = 0;
    }

    int calculate()
    {
        return Expression::varieties[name];
    }
};


class IntegerExp : public Expression
{
public:
    IntegerExp(ExpType type, int val)
        :
        Expression(type, val)
    {}

    int calculate()
    {
        return val;
    }
};


class UnaryExp : public Expression
{
public:
    UnaryExp(ExpType type, Token* op, Expression* right)
        :
        Expression(type, op, right)
    {}

    int calculate()
    {
        switch (op->type)
        {

        case    Token::TokenType::Output:
        {
            int cal = right->calculate();
            QString res;
            if (cal == 0) res += '0';
            while(cal > 0)
            {
                res = ((cal % 10) + 48) + res;
                cal /= 10;
            }
            lexerAnalyzeResult += res + '\r';
            return 1;
        }
        default:
            qDebug() << "wrong in unaryexp!" << endl;
            return -1;
        }
    }
};


class BinaryExp : public Expression
{
public:
    BinaryExp(ExpType type, Token* op, Expression* left, Expression* right)
        :
        Expression(type, op, left, right)
    {}

    int calculate()
    {
        switch (op->type)
        {
        case    Token::TokenType::Plus:
            return left->calculate() + right->calculate();
            break;

        case    Token::TokenType::Minus:
            return left->calculate() - right->calculate();
            break;

        case    Token::TokenType::Multiple:
            return left->calculate() * right->calculate();
            break;

        case    Token::TokenType::Divide:
            return left->calculate() / right->calculate();
            break;

        case    Token::DoubleEqual:
            if(left->calculate() == right->calculate()) return 1;
            else return 0;
            break;

        case    Token::Bigger:
            if(left->calculate() > right->calculate()) return 1;
            else return 0;
            break;

        case    Token::Smaller:
            if(left->calculate() < right->calculate()) return 1;
            else return 0;
            break;

        default:
            cout << "wrong in binaryexp!" << endl;
            return -1;
            break;
        }
    }
};


class TernaryExp : public Expression
{
public:
    TernaryExp(ExpType type, Expression* left, Token* op, Expression* mid, Token* op2, Expression* right)
        :
        Expression(type, left, op, mid, op2, right)
    {}

    int calculate()
    {
        switch (op->type)
        {
        case    Token::TokenType::Question:
        {
            if(op2->type == Token::TokenType::Colon)
            {
                if(left->calculate() == 1)
                {
                    return mid->calculate();
                }
                else return right->calculate();
            }
        }
        default:
            cout << "wrong in TernaryExp!" << endl;
            return -1;
            break;
        }
    }
};


class EqualExp : public Expression
{
public:
    EqualExp(ExpType type, Token* op, Expression* left, Expression* right)
        :
        Expression(type, op, left, right)
    {}

    int calculate()
    {
        if (op->type == Token::TokenType::Equal)
        {
            Expression::varieties[left->name] = right->calculate();
            return val;
        }
        else
        {
            cout << "wrong in equalexp" << endl;
            return -1;
        }
    }
};


class Lexer
{
public:
    vector<Token*>          myTokens;
    vector<Expression*>     roots;
    int                     nowLine = 0;
    QVector<int>            isInvalidTokenLines;
    QVector<bool>           isInvalidGrammarSemis;
    Lexer()
    {
        Expression::varieties.clear();
    }


    void    showMySelf      ();
    bool    isLatin         (QChar x);
    bool    validNextChar   (QChar x);
    bool    skipBlank       (QString input, int* now);
    bool    getNextToken    (QString input, int* now);
    bool    dealTokens      (QString input);
    bool    gainRoots       ();
    int     analyzeTree     (Expression* root);
    QString resStr          ();


    Expression* treeBuild(int starting, int ending);
};
