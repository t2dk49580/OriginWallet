#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    contractHead = "_G['os'] = nil\n_G['io'] = nil\njson = require 'json'\n";
    contractBase = contractHead+"gUser = nil\nfunction setUser(pUser)\ngUser = pUser\nend\nfunction init()\nend\n";
    contractErc20 = contractHead+"gUser = nil\nfunction setUser(pUser)\ngUser = pUser\nend\ngName = 'Zero Erc20 Template'\ngSymbol = 'ZET'\ngOwner = nil\ngTotalSupply = 0\ngBalance = {}\nfunction _toJson(pKey,pVar)\n	return json.encode({f=pKey,v=pVar,u=gUser})\nend\n\nfunction init(pTotal)\n	if gOwner ~= nil then\n		return toJson('init','fail: it was init')\n	end\n	gTotalSupply = pTotal\n	gOwner = gUser\n	gBalance[gOwner] = tonumber(pTotal)\n	return _toJson('init','init finish')\nend\n\nfunction getBalanceOf(pUser)\n	if gBalance[pUser] == nil then\n		return _toJson('balanceOf',0)\n	end\n	return _toJson('balanceOf',gBalance[pUser])\nend\n\nfunction transfer(pTo,pAmount)\n	if gBalance[gUser] == nil then\n		return _toJson('transfer','sender not found')\n	end\n	if gBalance[pTo] == nil then\n		gBalance[pTo] = 0\n	end\n	local curAmount = tonumber(pAmount)\n	if curAmount <= 0 then\n		return _toJson('transfer','curAmount <= 0')\n	end\n	if gBalance[gUser] < curAmount then\n		return _toJson('transfer','sender amount not enough')\n	end\n	gBalance[gUser] = gBalance[gUser] - curAmount\n	gBalance[pTo] = gBalance[pTo] + curAmount\n	return json.encode({sender=gUser,senderBalance=gBalance[gUser],reciver=pTo,reciverBlance=gBalance[pTo]})\nend";
    rc = new Reciver(ui->le_url->text().split(":").first()+":4003");
    QObject::connect(rc,SIGNAL(toWindow(QJsonObject)),this,SLOT(onMessage(QJsonObject)),Qt::QueuedConnection);
    //ui->lbAccount->setStyleSheet("background-color:red");
    if(!passwd.hasAppkey()){
        setAppkey();
    }else{
        if(!checkAppkey())
            exit(-1);
        QStringList arg;
        arg.append(GETADDR(QByteArray::fromHex(passwd.pubkey)));
        QJsonObject jsonObj = HttpRequest::doMethodGet(passwd,ui->le_url->text().split(":").first()+":4001",ui->le_contract->text(),"getBalanceOf",arg);
        ui->lb_zero->display(QString::number(jsonObj["v"].toDouble()));
    }
    ui->lb_pubkey->setText(GETADDR(QByteArray::fromHex(passwd.pubkey)));
    ui->le_addr->setText(GETADDR(QByteArray::fromHex(passwd.pubkey)));
    ui->te_code->setText(contractBase);
}

MainWindow::~MainWindow(){
    delete ui;
}

QString MainWindow::getInput(QString pMsg){
    bool ok = false;
    QString text;
    while(1){
        text = QInputDialog::getText(
                    NULL,
                    QString( "password check" ),
                    QString( pMsg ),
                    QLineEdit::Normal, QString(""), &ok);
        if ( ok && !text.isEmpty() ){
            break;
        }
    }
    return text;
}

void MainWindow::setAppkey(){
    passwd.updAppkey(GETMD5(getInput("create a new password").toLatin1()));
}

bool MainWindow::checkAppkey(){
    QByteArray input = getInput("please enter your password").toLatin1();
    if(GETMD5(input) == passwd.appkey){
        return true;
    }
    return false;
}

void MainWindow::onMessage(QJsonObject pObj){
    if(pObj.contains("senderBalance")){
        if(pObj.contains("sender")){
            if(pObj["sender"].toString() == GETADDR(QByteArray::fromHex(passwd.pubkey))){
                ui->lb_zero->display(QString::number(pObj["senderBalance"].toDouble()));
            }
        }
    }
    if(pObj.contains("reciverBalance")){
        if(pObj.contains("reciver")){
            if(pObj["reciver"].toString() == GETADDR(QByteArray::fromHex(passwd.pubkey))){
                ui->lb_zero->display(QString::number(pObj["reciverBalance"].toDouble()));
            }
        }
    }
}

void MainWindow::on_ui_query_clicked(){
    QByteArray result = HttpRequest::qtGet(QByteArray(QString(ui->le_url->text().split(":").first()+":4001/block").toLatin1())+ui->le_querykey->text().toLatin1());
    QList<QByteArray> data = result.split('@');
    ui->ui_databrowser->clear();
    if(data.count()>=5){
        ui->ui_index->setText(data.at(0));
        ui->ui_prehash->setText(data.at(2));
        ui->ui_curhash->setText(data.at(3));
        ui->ui_timestamp->setText(data.at(1));
        ui->ui_databrowser->setText(data.at(4));
    }else{
        ui->ui_databrowser->setText(result);
    }
}

void MainWindow::on_pushButton_clicked(){
    QStringList arg;
    arg.append(ui->le_addr->text());
    arg.append(ui->le_number->text());
    HttpRequest::doMethodSet(passwd,ui->le_url->text(),ui->le_contract->text(),"transfer",arg);
}

void MainWindow::on_pb_deploy_clicked(){
    QStringList arg;
    arg.append(ui->le_argment->text());
    QString code = ui->te_code->toPlainText();
    while((int)code.at(code.count()-1).toLatin1() <= 10){
        code.remove(code.count()-1,1);
    }
    if(code.left(contractHead.count()) != contractHead){
        BUG << "do not edit front 3 line";
        return;
    }
    code.remove(contractHead);
    if(code.contains("_G")){
        BUG << "do not use _G in custom program";
        return;
    }
    if(code.contains("require")){
        BUG << "do not use require in custom program";
        return;
    }
    HttpRequest::doDeploy(passwd,ui->le_port->text()+"/deploy",ui->le_name->text(),code.toLatin1().toHex(),arg);
}

void MainWindow::on_te_code_textChanged(){
    int cost = ui->te_code->toPlainText().count()*2-250;
    QString margin = QString::number(cost>0?cost:0);
    ui->le_margin->display(margin);
}

void MainWindow::on_radioButton_toggled(bool checked){
    if(checked){
        ui->te_code->setText(contractBase);
    }
}

void MainWindow::on_radioButton_3_toggled(bool checked){
    if(checked){
        ui->te_code->setText(contractErc20);
    }
}

void MainWindow::on_pb_next_clicked(){
    ui->le_querykey->setText(QString::number(ui->le_querykey->text().toInt()+1));
    on_ui_query_clicked();
}

void MainWindow::on_pb_prev_clicked(){
    if(ui->le_querykey->text()=="0"){
        return;
    }
    ui->le_querykey->setText(QString::number(ui->le_querykey->text().toInt()-1));
    on_ui_query_clicked();
}

void MainWindow::on_pushButton_2_clicked(){
    QString code = ui->te_arg->toPlainText();
    while((int)code.at(code.count()-1).toLatin1() <= 10){
        code.remove(code.count()-1,1);
    }
    QStringList arg;
    if(ui->rb_normal->isChecked()){
        if(code.contains("?")){
            arg = code.split("?");
        }else{
            arg.append(code);
        }
    }
    if(ui->rb_hex->isChecked()){
        if(code.contains("?")){
            QStringList curCode = code.split("?");
            for(auto cur:curCode){
                arg.append(cur.toLatin1().toHex());
            }
        }else{
            arg.append(code.toLatin1().toHex());
        }
    }
    HttpRequest::doMethodSet(passwd,ui->le_method_url->text(),ui->le_method_contract->text(),ui->le_method->text(),arg);
}









