#include "mymodel.h"
#include "ui_mymodel.h"

void MyModel::initLayout()
{
    this->setFixedSize(1440,840);

    bar = menuBar();
    setMenuBar(bar);
    menuBtn     = bar->addMenu("程序");
    editBtn     = menuBtn->addAction("编译");
    debugBtn    = menuBtn->addAction("调试");

    textEdit_inputView  = new QTextEdit(this);
    textEdit_outputView = new QTextEdit(this);
    table_monitorView   = new QTableWidget(this);
    textEdit_orderView  = new QTextEdit(this);
    goBtn               = new QPushButton(this);
    undoBtn             = new QPushButton(this);

    textEdit_inputView->resize(1000,810);
    textEdit_inputView->move(100,30);
    textEdit_inputView->setFontFamily("Consolas");
    textEdit_inputView->setFontPointSize(14);
    textEdit_inputView->setWordWrapMode(QTextOption::ManualWrap);

    textEdit_outputView->resize(340,380);
    textEdit_outputView->move(1100,460);
    textEdit_outputView->setFontFamily("Consolas");
    textEdit_outputView->setFontPointSize(14);

    table_monitorView->resize(340,380);
    table_monitorView->move(1100,80);
    table_monitorView->setFont(QFont("Consolas",14));
    table_monitorView->setEditTriggers(QAbstractItemView::NoEditTriggers);


    textEdit_orderView->resize(100,810);
    textEdit_orderView->move(0,30);
    textEdit_orderView->setFontFamily("Consolas");
    textEdit_orderView->setFontPointSize(14);
    textEdit_orderView->verticalScrollBar()->setVisible(false);
    textEdit_orderView->setReadOnly(true);


    goBtn->resize(170,50);
    goBtn->move(1100,30);
    goBtn->setFont(QFont("Consolas",14));
    goBtn->setText("Go");

    undoBtn->resize(170,50);
    undoBtn->move(1270,30);
    undoBtn->setFont(QFont("Consolas",14));
    undoBtn->setText("Undo");

    textEdit_inputView->setReadOnly(false);

    table_monitorView->setColumnCount(2);
    table_monitorView->setHorizontalHeaderLabels({"Varieties", "Value"});


// Init
    // 同步垂直滚动条
    connect(textEdit_inputView->verticalScrollBar(), &QScrollBar::valueChanged,
            textEdit_orderView->verticalScrollBar(), &QScrollBar::setValue);
    // 同步输入文本框的改变
    connect(textEdit_inputView, &QTextEdit::textChanged,
            [this](){
        // table_monitorView
            while (table_monitorView->rowCount() > 0) {
                table_monitorView->removeRow(0);
            }
        // textEdit_inputView
            textEdit_outputView->clear();
        getDataInEditting();
        updateInEditting();
    });
    // debugBtn（QAction）点击
    connect(debugBtn , &QAction::triggered ,
            this , &MyModel::debugging);
    // goBtn点击
    connect(goBtn , &QPushButton::clicked , [&](bool)
    {
        getDataInDebugging_goBtn();
        updateInDebugging();
    });
    // undoBtn点击
    connect(undoBtn , &QPushButton::clicked , [&](bool)
    {
        getDataInDebugging_undoBtn();
        updateInDebugging();
    });



// 初始状态Edit
    editting();
}


void MyModel::getDataInEditting()
{
// Input
    inputDocu       = textEdit_inputView->document();
    inputStr        = textEdit_inputView->toPlainText();

// inputCursor
    inputCursor     = textEdit_inputView->textCursor();
    inputCursor.movePosition(QTextCursor::Start);

// anaLexer
    anaLexer.dealTokens(inputStr);
    anaLexer.gainRoots();

// highlightRedLines
    gainRedLines();

// outputOrderStr
    outputOrderStr  = outputLineNumber(inputDocu);

// isValid && anaLexer.myTokens
    isValidIden     = anaLexer.dealTokens(inputStr);
    if(!inputStr.isEmpty() && isValidIden)
    {
        isValidGram = anaLexer.gainRoots();
    }
}


void MyModel::gainRedLines()
{
    highlightRedLines.clear();
    if(inputStr.isEmpty()) return;

// InvalidTokens
    for(auto i : anaLexer.isInvalidTokenLines)
    {
        highlightRedLines.push_back(i);
    }

// InvalidGrammar
    semicolonLines.clear();
    QStringList lines = inputStr.split("\n");
    for(int i=0 ; i<lines.size() ; ++i)
    {
        for(int j=0 ; j<lines[i].count(';') ; ++j)
        {
            semicolonLines.push_back(i);
        }
    }
    for(int i=lines.size()-1 ; i>=0 ; --i)
    {
        if(!lines[i].isEmpty())
        {
            for(int j=lines[i].size()-1 ; j>=0 ; --j)
            {
                if(lines[i].at(j) == ' ' || lines[i].at(j) == '\t') continue;
                else if(lines[i].at(j) == ';') break;
                else
                {
                    semicolonLines.push_back(i);
                }
            }
            break;
        }
    }
    int now = -1;
    for(auto i : anaLexer.isInvalidGrammarSemis)
    {
        ++now;
        if(i == false)
        {
            highlightRedLines.push_back(semicolonLines[now]);
        }
    }

}



void MyModel::updateInEditting()
{
// textEdit_orderView
    textEdit_orderView  ->setPlainText(outputOrderStr);
    int lineValue       = textEdit_inputView->verticalScrollBar()->value();
    textEdit_orderView  ->verticalScrollBar()->setValue(lineValue);
    textEdit_inputView->setFontFamily("Consolas");
    textEdit_inputView->setFontPointSize(14);

// textEdit_inputView
    textEdit_inputView->setReadOnly(false);
    highlightLines(highlightRedLines);

// editBtn && debugBtn
    if(isValidGram + isValidIden == 2) debugBtn->setEnabled(true);
    else debugBtn->setEnabled(false);


}


QString MyModel::outputLineNumber(QTextDocument* inputDocu)
{
    int line = 0;
    auto now = inputDocu->begin();
    while(now.isValid() && now.layout())
    {
        line += now.layout()->lineCount();
        now = now.next();
    }
    QString res;
    for(int i = 1 ; i <= line + 1 ; ++i)
    {
        res += QString::number(i) + '\r';
    }
    return  res;
}


void MyModel::highlightLines(QVector<int> Lines)
{
    // 清除之前的高亮
    textEdit_inputView->setExtraSelections(QList<QTextEdit::ExtraSelection>());
    inputCursor.movePosition(QTextCursor::Start);
    QList<QTextEdit::ExtraSelection> listSelections;
    for(auto iter : Lines)
    {
        for(int i=0 ; i<iter ; ++i)
            inputCursor.movePosition(QTextCursor::NextBlock);

        QTextEdit::ExtraSelection selection;
        selection.format.setBackground(QColor(255,20,0,100));
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = inputCursor;
        listSelections.append(selection);
        inputCursor.movePosition(QTextCursor::Start);
    }
    textEdit_inputView->setExtraSelections(listSelections);
}


void MyModel::highlightLines(int line)
{
    // 清除之前的高亮
    textEdit_inputView->setExtraSelections(QList<QTextEdit::ExtraSelection>());
    inputCursor.movePosition(QTextCursor::Start);
    QList<QTextEdit::ExtraSelection> listSelections;

    for(int i=0 ; i<line ; ++i)
        inputCursor.movePosition(QTextCursor::NextBlock);

    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(QColor(255,20,0,100));
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = inputCursor;
    listSelections.append(selection);
    inputCursor.movePosition(QTextCursor::Start);
    textEdit_inputView->setExtraSelections(listSelections);
}


void MyModel::editting()
{
    goBtn->setEnabled(false);
    undoBtn->setEnabled(false);
    table_monitorView->setEnabled(false);

    getDataInEditting();
    updateInEditting();

}


void MyModel::debugging()
{
    goBtn->setEnabled(true);
    undoBtn->setEnabled(true);
    table_monitorView->setEnabled(true);
    textEdit_inputView->setReadOnly(true);

    varietiesMonitor.clear();
    outputStrMonitor.clear();

    nextSemicolon = 0;
    haveDoneSemicolon = -1;

    updateInDebugging();
}


void MyModel::getDataInDebugging_goBtn()
{
// 即将处理最后一条语句并且结束程序
    if(nextSemicolon == semicolonLines.size() - 1)
    {
        anaLexer.analyzeTree(anaLexer.roots[nextSemicolon]);
        haveDoneSemicolon = nextSemicolon;
        nextSemicolon++;
        varietiesMonitor.push_back(Expression::varieties);
        outputStrMonitor.push_back(lexerAnalyzeResult);
    }
// 执行了新语句
    else if(nextSemicolon > haveDoneSemicolon)
    {
        anaLexer.analyzeTree(anaLexer.roots[nextSemicolon]);
        varietiesMonitor.push_back(Expression::varieties);
        outputStrMonitor.push_back(lexerAnalyzeResult);
        haveDoneSemicolon = nextSemicolon;
        nextSemicolon++;
    }
// 其余
    else
    {
        nextSemicolon++;
    }
}


void MyModel::getDataInDebugging_undoBtn()
{
    nextSemicolon--;
    updateInDebugging();
}


void MyModel::getDataInTableChanged()
{
    int cutStatus = varietiesMonitor.size() - nextSemicolon;
    for(int i=0 ; i<=cutStatus ; ++i)
        varietiesMonitor.pop_back();

    for(int i=0 ; i<cutStatus ; ++i)
        outputStrMonitor.pop_back();

    haveDoneSemicolon = nextSemicolon - 1;

    nowStatus.clear();
    for(int i=0 ; i<table_monitorView->rowCount() ; ++i)
    {
        QTableWidgetItem *itemVariety = table_monitorView->item(i, 0);
        QTableWidgetItem *itemValue = table_monitorView->item(i, 1);
        string variety = itemVariety->text().toStdString();
        int value = itemValue->text().toInt();
        nowStatus.insert(make_pair(variety,value));
    }
    Expression::varieties = nowStatus;
    varietiesMonitor.push_back(nowStatus);

    lexerAnalyzeResult = outputStrMonitor[nextSemicolon-1];

//    if(anaLexer.roots[nextSemicolon-1]->op->type == Token::TokenType::Output)
//    {
//        if(!outputStrMonitor.empty())
//        {
//            lexerAnalyzeResult = outputStrMonitor[nextSemicolon-2];
//            anaLexer.analyzeTree(anaLexer.roots[nextSemicolon-1]);
//            outputStrMonitor.pop_back();
//            outputStrMonitor.push_back(lexerAnalyzeResult);
//        }
//    }
}


void MyModel::updateInDebugging()
{
    disconnect(table_monitorView , &QTableWidget::cellChanged , 0 , 0 );

    if(nextSemicolon == 0)  undoBtn->setEnabled(false);
    else                    undoBtn->setEnabled(true);

    if(haveDoneSemicolon == semicolonLines.size() - 1)
        // 最后一句运行
        textEdit_inputView->setExtraSelections(QList<QTextEdit::ExtraSelection>());
    else
        highlightLines(semicolonLines[nextSemicolon]);

    nowStatus.clear();
    nowOutputStr.clear();
    if (!varietiesMonitor.empty() && nextSemicolon != 0)
    {
        nowStatus = varietiesMonitor[nextSemicolon-1];
        nowOutputStr = outputStrMonitor[nextSemicolon-1];
    }

    table_monitorView->setRowCount(nowStatus.size());
    table_monitorView->setEditTriggers(QAbstractItemView::CurrentChanged);
    int row = -1;
    for(auto i : nowStatus)
    {
        row++;

        QTableWidgetItem *itemVariety = new QTableWidgetItem(i.first.c_str());
        itemVariety->setFlags(itemVariety->flags() & ~ Qt::ItemIsEnabled);
        table_monitorView->setItem(row, 0, itemVariety);

        QTableWidgetItem *itemValue = new QTableWidgetItem(QString::number(i.second));
        itemValue->setFlags(itemValue->flags() | Qt::ItemIsEditable);
        table_monitorView->setItem(row, 1, itemValue);
    }

    textEdit_outputView->setPlainText(nowOutputStr);


    if(haveDoneSemicolon == semicolonLines.size() - 1)
    {
        editting();
        return;
    }

    connect(table_monitorView , &QTableWidget::cellChanged , [&]()
    {
        getDataInTableChanged();
        updateInDebugging();
    });
}


MyModel::MyModel(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MyModel)
{
    ui->setupUi(this);
    initLayout();
}


MyModel::~MyModel()
{
    delete ui;
}

