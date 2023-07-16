#ifndef MYMODEL_H
#define MYMODEL_H

#include <QMainWindow>
#include <QTextEdit>
#include <QString>
#include <QDebug>
#include <QTableWidget>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include <QScrollBar>
#include <QPushButton>
#include <QAbstractItemView>
#include <set>
#include "lexerTreeAnalyze.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MyModel; }
QT_END_NAMESPACE

class MyModel : public QMainWindow
{
    Q_OBJECT

public:
    MyModel(QWidget *parent = nullptr);
    ~MyModel();

private:
    Ui::MyModel *ui;

// UI
    QMenuBar        *bar;
    QMenu           *menuBtn;
    QAction         *editBtn;
    QAction         *debugBtn;
    QPushButton     *goBtn;
    QPushButton     *undoBtn;


// View
    QTextEdit       *textEdit_inputView;
    QTextEdit       *textEdit_outputView;
    QTableWidget    *table_monitorView;
    QTextEdit       *textEdit_orderView;

// Data
    QString         inputStr;
    QTextDocument   *inputDocu;
    QString         outputOrderStr;
    QString         outputStr;
    QTextCursor     inputCursor;
    Lexer           anaLexer;
    QVector<int>    highlightRedLines;
    QVector<int>    semicolonLines;
    QVector<map<string, int>> varietiesMonitor;
    map<string,int> nowStatus;
    QVector<QString>outputStrMonitor;
    QString         nowOutputStr;
    bool            isValidIden = false;
    bool            isValidGram = false;
    int             nextSemicolon;
    int             haveDoneSemicolon;

// Function
    void            initLayout();
    void            getDataInEditting();
    void            getDataInDebugging_goBtn();
    void            getDataInDebugging_undoBtn();
    void            getDataInTableChanged();
    void            gainRedLines();
    QString         outputLineNumber(QTextDocument* inputDocu);
    void            updateInEditting();
    void            updateInDebugging();
    void            highlightLines(QVector<int> Lines);
    void            highlightLines(int line);
    void            editting();
    void            debugging();
};
#endif // MYMODEL_H
