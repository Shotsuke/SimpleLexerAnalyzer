#include "lexerTreeAnalyze.h"

// 在.cpp文件中，添加对Expression类静态成员变量的定义。
// 这样，.cpp中的定义将成为唯一的定义。
map<string, int> Expression::varieties;
QString lexerAnalyzeResult = "";


void    Lexer::showMySelf()
{
    for (auto iter : myTokens)
    {
        iter->showMySelf();
    }
    return;
}


bool    Lexer::isLatin(QChar x)
{
    if(x >= 'a' && x <= 'z')
        return true;
    if(x >= 'A' && x <= 'Z')
        return true;
    return false;
}


bool    Lexer::validNextChar(QChar x)
{
    if(     x == ' '    ||
            x == '\r'   ||
            x == '\n'   ||
            x == '\t'   ||
            x == ';'    ||
            x == '+'    ||
            x == '-'    ||
            x == '*'    ||
            x == '/'    ||
            x == '='    ||
            x == '>'    ||
            x == '<'    ||
            x == '('    ||
            x == ')'    ||
            x == '?'    ||
            x == ':'    )
    {
        return true;
    }
    return false;
}


bool    Lexer::skipBlank(QString input, int* now)
{   // Skip: ' ', '\t', '\r', '\n'
    // If the input is valid, return true.
    // If it meets the end, return false.
    // Return with *now poingting to the first valid char
    QChar c;
    while ((*now) < input.length() )
    {
        c = input[(*now)];
        if(c == ' '  ||
           c == '\t' ||
           c == '\n' ||
           c == '\r')
        {
            if(c == '\r' || c == '\n') nowLine++;
            (*now)++;
        }
        else break;
    }

    if ((*now) >= input.length() || c == QChar(26) || c.isNull())
        return false;
    else
    {
        return true;
    }
}


bool    Lexer::getNextToken(QString input, int* now)
{   // Get the next Token, and put it into myTokens.
    // If it is an invalid token, i,e, has wrong in grammar,
    // then a token called "InvalidToken" will be put into,
    // and return with False.
    // Else if it meets the end, then a token called "EOF" will be put into,
    // and return with False.
    // Else, it will put the right token into the vector and return True.
    if (!skipBlank(input, now))
    { // Deal with blanks.
        myTokens.push_back(new Token(Token::TokenType::EndOfInput));
        return false;
    }


    QChar c = input[*now] ;
    (*now)++;

    if (c.isDigit())
    {
        // Deal with Integer
        int _num = c.digitValue();
        while ((*now) < input.length() && input[*now].isDigit())
        {
            c = input[(*now)];
            (*now)++;
            _num = _num * 10 + c.digitValue();
        }

        if ((*now) >= input.length() || input[*now] == QChar(26))
        {
            myTokens.push_back(new Token(Token::TokenType::Integer, _num));
            myTokens.push_back(new Token(Token::TokenType::EndOfInput));
            return false;
        }

        if(validNextChar(input[*now]))
        {
            myTokens.push_back(new Token(Token::TokenType::Integer, _num));
        }
        else
        {
            myTokens.push_back(new Token(Token::TokenType::InvalidToken));
        }

    }


    else if (isLatin(c))
    {
        // Deal with Identifier
        QString temp;
        temp += c;
        while ((*now) < input.length() && (input[*now].isNumber()   ||
                                           input[*now] == '_'       ||
                                           isLatin(input[*now])))
        {
            c = input[(*now)];
            (*now)++;
            temp += c;
        }

        if ((*now) >= input.length() || input[*now] == QChar(26))
        {
            if (temp == "output")
                myTokens.push_back(new Token(Token::TokenType::Output));
            else
                myTokens.push_back(new Token(Token::TokenType::Identifier, temp.toStdString()));

            myTokens.push_back(new Token(Token::TokenType::EndOfInput));
            return false;
        }

        if(validNextChar(input[*now]))
        {
            if (temp == "output")
                myTokens.push_back(new Token(Token::TokenType::Output));
            else
                myTokens.push_back(new Token(Token::TokenType::Identifier, temp.toStdString()));
        }
        else
        {
            myTokens.push_back(new Token(Token::TokenType::InvalidToken));
        }
    }
    else if (validNextChar(c))
    {
        switch (c.toLatin1())
        {
        case '+':
            myTokens.push_back(new Token(Token::TokenType::Plus));
            break;
        case '-':
            myTokens.push_back(new Token(Token::TokenType::Minus));
            break;
        case '*':
            myTokens.push_back(new Token(Token::TokenType::Multiple));
            break;
        case '/':
            myTokens.push_back(new Token(Token::TokenType::Divide));
            break;
        case '>':
            myTokens.push_back(new Token(Token::TokenType::Bigger));
            break;
        case '<':
            myTokens.push_back(new Token(Token::TokenType::Smaller));
            break;
        case '(':
            myTokens.push_back(new Token(Token::TokenType::Lparen));
            break;
        case ')':
            myTokens.push_back(new Token(Token::TokenType::Rparen));
            break;
        case ';':
            myTokens.push_back(new Token(Token::TokenType::Semicolon));
            break;
        case '?':
            myTokens.push_back(new Token(Token::TokenType::Question));
            break;
        case ':':
            myTokens.push_back(new Token(Token::TokenType::Colon));
            break;
        case '=':
            if ((*now) < input.length() && input[*now] == '=')
            {
                (*now)++;
                myTokens.push_back(new Token(Token::TokenType::DoubleEqual));
                break;
            }
            else
            {
                myTokens.push_back(new Token(Token::TokenType::Equal));
                break;
            }
        }
    }
    else
    {
        myTokens.push_back(new Token(Token::TokenType::InvalidToken));
    }

    return true;
}


bool    Lexer::dealTokens(QString input)
{
    isInvalidTokenLines.clear();
    myTokens.clear();
    if(input.isEmpty())
    {
        myTokens.push_back(new Token(Token::TokenType::EndOfInput));
        return false;
    }

    nowLine             = 0;
    bool    flag        = true;
    int     ending      = -1;
    int*    input_now   = new int;
    *input_now = 0;
    while (!input.isEmpty() &&
           getNextToken(input , input_now) &&
           myTokens[++ending]->type != Token::TokenType::EndOfInput)
    {   // each time a token was put into the vector
        if(!myTokens.empty() &&
           myTokens[ending]->type == Token::TokenType::InvalidToken)
        {
            flag = false;
            if(isInvalidTokenLines.count(nowLine) == 0)
                isInvalidTokenLines.push_back(nowLine);
        }
    }
    return flag;
}


Expression* Lexer::treeBuild(int starting, int ending)
{
    // treeBuild is used to build a lexer tree
    // We suppose the end token is a semicolon
    // NOT an end of input

    if(starting == ending) return nullptr;

// Deal with the base case
// Length 1
    if (starting == ending - 1)
    {
        switch (myTokens[starting]->type)
        {
        case    Token::TokenType::Integer:
        {
            Expression* res = new IntegerExp
                        (
                        Expression::ExpType::IntegerExp,
                        myTokens[starting]->val
                        );

            return res;
            break;
        };
        case    Token::TokenType::Identifier:
        {
            Expression* res = new IdentifierExp
                        (
                        Expression::ExpType::IdentifierExp,
                        myTokens[starting]->name
                        );

            return res;
            break;
        };
        default:
            return nullptr;
            break;
        }
    }


// Judge the l/rparen error
// & cancel l/rparen
    int     lparen = 0;
    int     firstClosure = -2;
    int     firstLparen = -1;
            // firstClosure:
            // -3 means their exists a l/rparen error
            // -2 means their is no any l/rparen
            // -1 means it remains to receive a closure rparen
            // a positive integer means a first closure's ending
    for(int i = starting ; i <= ending - 1 ; ++i)
    {

        if(myTokens[i]->type == Token::TokenType::Lparen)
        {
            lparen++;
            if(firstLparen == -1) firstLparen = i;
        }
        else if(myTokens[i]->type == Token::TokenType::Rparen)
            lparen--;

        if(lparen < 0)
        {
            firstClosure = -3;
            break;
        }
        if(lparen != 0 && firstClosure == -2)
            firstClosure = -1;
        if(lparen == 0 && firstClosure == -1)
            firstClosure = i;
    }
        if (firstClosure == ending - 1 && firstLparen == starting)
        {
            // Calcel the pairing l/rparen
            return treeBuild(starting + 1, ending - 1);
        }
        else if(firstClosure == -3 || lparen != 0)
        {
            return nullptr;
        }

//Deal with the ? :
//Prior 1
    bool haveQuestioned = false;
    for (int i = starting; i < ending; ++i)
    {
        if (myTokens[i]->type == Token::TokenType::Lparen)
        {
            lparen++;
        }
        else if (myTokens[i]->type == Token::TokenType::Rparen)
        {
            lparen--;
        }

        if (lparen == 0 && myTokens[i]->type == Token::TokenType::Question && !haveQuestioned)
        {
            Expression* left = treeBuild(starting, i);
            haveQuestioned = true;

            for (int j = i + 1 ; j < ending; ++j)
            {
                if (myTokens[j]->type == Token::TokenType::Lparen)
                {
                    lparen++;
                }
                else if (myTokens[j]->type == Token::TokenType::Rparen)
                {
                    lparen--;
                }

                if (lparen == 0 && myTokens[j]->type == Token::TokenType::Colon && haveQuestioned)
                {
                    Expression* mid = treeBuild(i + 1, j);
                    Expression* right = treeBuild(j + 1, ending);
                    if(left == nullptr || mid == nullptr || right == nullptr) return nullptr;
                    Expression* res = new TernaryExp
                                (
                                Expression::ExpType::TernaryExp,
                                left,
                                myTokens[i],
                                mid,
                                myTokens[j],
                                right
                                );
                    return res;
                }
            }
            return nullptr;
        }
    }

//Deal with the >, <, ==
//Prior
    for (int i = starting; i < ending; ++i)
    {
        if (myTokens[i]->type == Token::TokenType::Lparen)
        {
            lparen++;
        }
        else if (myTokens[i]->type == Token::TokenType::Rparen)
        {
            lparen--;
        }

        if (lparen == 0 && (
                    myTokens[i]->type == Token::TokenType::Bigger ||
                    myTokens[i]->type == Token::TokenType::Smaller||
                    myTokens[i]->type == Token::TokenType::DoubleEqual))
        {
            Expression* left = treeBuild(starting, i);
            Expression* right = treeBuild(i + 1, ending);
            if(left == nullptr || right == nullptr) return nullptr;
            Expression* res = new BinaryExp
                        (
                        Expression::ExpType::BinaryExp,
                        myTokens[i],
                        left,
                        right
                        );

            return res;
        }
    }


//Deal with the output, also sum up the number of lparen
//Prior 2
    for (int i = starting; i < ending; ++i)
    {
        if (myTokens[i]->type == Token::TokenType::Lparen)
            lparen++;
        else if (myTokens[i]->type == Token::TokenType::Rparen)
            lparen--;

        if (lparen == 0 && myTokens[i]->type == Token::TokenType::Output)
        {
            Expression* right =
                    treeBuild(i + 1, ending);

            if(right == nullptr) return nullptr;

            Expression* res = new UnaryExp
                        (
                        Expression::ExpType::UnaryExp,
                        myTokens[i],
                        right
                        );

            return res;
        }
    }




//Deal with the =
//Prior 3
    for (int i = starting; i < ending; ++i)
    {
        if (myTokens[i]->type == Token::TokenType::Lparen)
        {
            lparen++;
        }
        else if (myTokens[i]->type == Token::TokenType::Rparen)
        {
            lparen--;
        }

        if (lparen == 0 && myTokens[i]->type == Token::TokenType::Equal)
        {
            Expression* left = treeBuild(starting, i);
            Expression* right = treeBuild(i + 1, ending);
            if(left == nullptr || right == nullptr) return nullptr;
            Expression* res = new EqualExp
                        (
                        Expression::ExpType::EqualExp,
                        myTokens[i],
                        left,
                        right
                        );

            return res;
        }
    }



//Deal with the +, -
//Prior 4
    for (int i = starting; i < ending; ++i)
    {
        if (myTokens[i]->type == Token::TokenType::Lparen)
        {
            lparen++;
        }
        else if (myTokens[i]->type == Token::TokenType::Rparen)
        {
            lparen--;
        }

        if (lparen == 0 && (
                    myTokens[i]->type == Token::TokenType::Plus ||
                    myTokens[i]->type == Token::TokenType::Minus))
        {
            Expression* left = treeBuild(starting, i);
            Expression* right = treeBuild(i + 1, ending);
            if(left == nullptr || right == nullptr) return nullptr;
            Expression* res = new BinaryExp
                        (
                        Expression::ExpType::BinaryExp,
                        myTokens[i],
                        left,
                        right
                        );

            return res;
        }
    }

//Deal with the *, /
//Prior 5
    lparen = 0;
    for (int i = starting; i < ending; ++i)
    {
        if (myTokens[i]->type == Token::TokenType::Lparen)
        {
            lparen++;
        }
        else if (myTokens[i]->type == Token::TokenType::Rparen)
        {
            lparen--;
        }

        if (lparen == 0 && (
                    myTokens[i]->type == Token::TokenType::Multiple ||
                    myTokens[i]->type == Token::TokenType::Divide))
        {
            Expression* left = treeBuild(starting, i);
            Expression* right = treeBuild(i + 1, ending);
            if(left == nullptr || right == nullptr) return nullptr;
            Expression* res = new BinaryExp
                        (
                        Expression::ExpType::BinaryExp,
                        myTokens[i],
                        left,
                        right
                        );

            return res;
        }
    }

//Now the left is an invalid Expression
//Include: 2 or more IdentifierExp and IntegerExp
    return nullptr;
}


bool    Lexer::gainRoots()
{
    roots.clear();
    isInvalidGrammarSemis.clear();
    lexerAnalyzeResult.clear();
    Expression::varieties.clear();

    if(myTokens[0]->type == Token::TokenType::EndOfInput)
    {
        isInvalidGrammarSemis.push_back(true);
        return true;
    }
    int     starting    = 0;
    int     ending      = -1;
    int*    input_now   = new int;
    *input_now = 0;
    bool    flag        = true;
    for(auto i : myTokens)
    {
        ending++;
        if(i->type == Token::TokenType::Semicolon)
        {
            if(treeBuild(starting ,ending) == nullptr)
            {
                flag = false;
                isInvalidGrammarSemis.push_back(false);
            }
            else
                isInvalidGrammarSemis.push_back(true);
            roots.push_back(treeBuild(starting ,ending));
            starting = ending + 1;
        }
    }
    if(myTokens[myTokens.size()-2]->type != Token::TokenType::Semicolon)
    {
        flag = false;
        isInvalidGrammarSemis.push_back(false);
    }
    return flag;
}


int     Lexer::analyzeTree(Expression* root)
{
    // Analyze the root of the tree
    if(root == nullptr) return -1;
    return root->calculate();
}


QString Lexer::resStr()
{
    QString res;
    Expression::varieties.clear();
    lexerAnalyzeResult.clear();
    for(auto i : roots)
    {
        analyzeTree(i);
    }
    return lexerAnalyzeResult;
}


// Example
/*
signed main()
{
    Lexer   mylex;
    int     starting = 0;
    int     ending = -1;
    while (mylex.getNextToken() &&
        mylex.myTokens[++ending]->type != Token::TokenType::EndOfInput)
    {
        if (mylex.myTokens[ending]->type == Token::TokenType::Semicolon)
        {
            Expression* root = mylex.treeBuild(starting, ending);
            mylex.analyzeTree(root);
            starting = ending + 1;
        }
    }

    return 0;
}
*/
