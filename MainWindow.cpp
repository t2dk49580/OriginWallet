#include "MainWindow.h"
#include "ui_MainWindow.h"

QString nodeip = "http://47.75.190.195:3000";

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    req = new NetRequest();
    req->start();
    CONN(this,SIGNAL(toGet(QString)),req,SLOT(onGet(QString)));
    CONN(this,SIGNAL(toPost(QString,QString)),req,SLOT(onPost(QString,QString)));
    CONN(req,SIGNAL(doRsponse(QString)),this,SLOT(onRsponse(QString)));
    basePort = nodeip.split(":").last().toInt();
    contractHead = "_G['os'] = nil\n_G['io'] = nil\njson = require 'json'\n";
    contractBase = contractHead+"gUser = nil\nfunction setUser(pUser)\ngUser = pUser\nend\nfunction init()\nend\n";
    //contractErc20 = contractHead+"gUser = nil\nfunction setUser(pUser)\ngUser = pUser\nend\ngName = 'Zero Erc20 Template'\ngSymbol = 'ZET'\ngOwner = nil\ngTotalSupply = 0\ngBalance = {}\nfunction _toJson(pKey,pVar)\n	return json.encode({f=pKey,v=pVar,u=gUser})\nend\n\nfunction init(pTotal)\n	if gOwner ~= nil then\n		return toJson('init','fail: it was init')\n	end\n	gTotalSupply = pTotal\n	gOwner = gUser\n	gBalance[gOwner] = tonumber(pTotal)\n	return _toJson('init','init finish')\nend\n\nfunction getBalanceOf(pUser)\n	if gBalance[pUser] == nil then\n		return _toJson('balanceOf',0)\n	end\n	return _toJson('balanceOf',gBalance[pUser])\nend\n\nfunction transfer(pTo,pAmount)\n	if gBalance[gUser] == nil then\n		return _toJson('transfer','sender not found')\n	end\n	if gBalance[pTo] == nil then\n		gBalance[pTo] = 0\n	end\n	local curAmount = tonumber(pAmount)\n	if curAmount <= 0 then\n		return _toJson('transfer','curAmount <= 0')\n	end\n	if gBalance[gUser] < curAmount then\n		return _toJson('transfer','sender amount not enough')\n	end\n	gBalance[gUser] = gBalance[gUser] - curAmount\n	gBalance[pTo] = gBalance[pTo] + curAmount\n	return json.encode({sender=gUser,senderBalance=gBalance[gUser],reciver=pTo,reciverBlance=gBalance[pTo]})\nend";
    contractErc20 = ""
"_G['os'] = nil\n\
_G['io'] = nil\n\
json = require 'json'\n\
gUser = nil\n\
gName    = 'Erc20 Template Token'\n\
gSymbol  = 'ETT'\n\
gBalance = {}\n\
gTotal   = 70000000000\n\
gOwner   = nil\n\
function setUser(pUser)\n\
    gUser = pUser\n\
end\n\
function _addResult(pUser,pMethod,pResult,pMsg,pAll)\n\
    local buffer = {}\n\
    local result = pAll\n\
    buffer['method'] = pMethod\n\
    buffer['result'] = pResult\n\
    buffer['msg']    = pMsg\n\
    buffer['owner']  = pUser\n\
    table.insert(result,buffer)\n\
    return result\n\
end\n\
function _getResult(pUser,pMethod,pResult,pMsg)\n\
    local buffer = {}\n\
    local result = _addResult(pUser,pMethod,pResult,pMsg,buffer)\n\
    return json.encode(result)\n\
end\n\
function init()\n\
    if gOwner ~= nil then\n\
        return 'fail'\n\
    end\n\
    gOwner = gUser\n\
    gBalance[gUser] = gTotal\n\
    return _getResult(gUser,'init',true,'ok')\n\
end\n\
function getSymbol()\n\
    return _getResult(gUser,'getSymbol',true,gSymbol)\n\
end\n\
function getTotalSupply()\n\
    return _getResult(gUser,'getTotalSupply',true,gTotal)\n\
end\n\
function getName()\n\
    return _getResult(gUser,'getName',true,gName)\n\
end\n\
function getBalanceOf(pUser)\n\
    if gBalance[pUser] == nil then\n\
        return _getResult(gUser,'getBalanceOf',true,0)\n\
    end\n\
    return _getResult(gUser,'getBalanceOf',true,gBalance[pUser])\n\
end\n\
function transfer(pTo,pAmount)\n\
    local curAmount = tonumber(pAmount)\n\
    if curAmount <= 0 then\n\
        return 'fail'\n\
    end\n\
    if gBalance[gUser] < curAmount then\n\
        return 'fail'\n\
    end\n\
    if gBalance[pTo] == nil then\n\
        gBalance[pTo] = 0\n\
    end\n\
    local curResult = {}\n\
    gBalance[gUser] = gBalance[gUser] - curAmount\n\
    gBalance[pTo] = gBalance[pTo] + curAmount\n\
    _addResult(gUser,'transfer',true,gBalance[gUser],curResult)\n\
    _addResult(pTo,'transfer',true,gBalance[pTo],curResult)\n\
    return json.encode(curResult)\n\
end";

    rc = new Reciver(nodeip.split(":").first()+":"+QString::number(basePort+1));
    QObject::connect(rc,SIGNAL(toWindow(QJsonDocument)),this,SLOT(onMessage(QJsonDocument)),Qt::QueuedConnection);
    //ui->lbAccount->setStyleSheet("background-color:red");
    if(!passwd.hasAppkey()){
        setAppkey();
    }else{
        if(!checkAppkey())
            exit(-1);
        QJsonArray jsonArr;// = HttpRequest::doMethodGet(passwd,nodeip,ui->le_contract->text(),"getBalanceOf",GETADDR(passwd.pubkey).toHex());
        if(jsonArr.count()>0){
            ui->lcd_onn->display(SETXF(jsonArr.at(0).toObject()["msg"].toDouble(),4));
        }
        else
            BUG << "QJsonArray == null";
    }
    //ui->lb_pubkey->setText(GETADDR(QByteArray::fromHex(passwd.pubkey)));
    //ui->le_addr->setText(GETADDR(QByteArray::fromHex(passwd.pubkey)));
    ui->lb_pubkey->setText(GETADDR(passwd.pubkey));
    ui->le_addr->setText(GETADDR(passwd.pubkey));
    ui->te_code->setText(contractBase);

    ui->le_geturl_req->setText(doLoad("geturl0"));

    ui->le_posturl_req->setText(doLoad("posturl0"));
    ui->le_posturl_req_2->setText(doLoad("posturl1"));
    ui->le_posturl_req_3->setText(doLoad("posturl2"));
    ui->le_posturl_req_4->setText(doLoad("posturl3"));

    ui->le_posturi_req->setText(doLoad("posturi0"));
    ui->le_posturi_req_2->setText(doLoad("posturi1"));
    ui->le_posturi_req_3->setText(doLoad("posturi2"));
    ui->le_posturi_req_4->setText(doLoad("posturi3"));
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

void MainWindow::onMessage(QJsonDocument pJson){
    QJsonDocument pDoc(pJson);
    QJsonArray pArr = pDoc.array();
    ui->tb_network->append(QString(pDoc.toJson()).remove("\\"));
    for(int i=0;i<pArr.count();i++){
        QJsonObject obj = pArr.at(i).toObject();
        if(obj["method"] == "transfer"){
            if(obj["owner"] == ui->lb_pubkey->text()){
                ui->lcd_onn->display(SETXF(obj["msg"].toDouble(),4));
            }
        }else if(obj["method"] == "getBalanceOf"){
            if(obj["owner"] == ui->lb_pubkey->text()){
                ui->lcd_onn->display(SETXF(obj["msg"].toDouble(),4));
            }
        }else if(obj["method"] == "init"){
            if(obj["owner"] == ui->lb_pubkey->text()){
                if(obj["symbol"] == "ONN")
                    ui->lcd_onn->display(SETXF(obj["msg"].toDouble(),4));
            }
        }
    }
}

static onnBlock createBlock(QByteArray pData){
    onnBlock curBlock;
    QList<QByteArray> data = pData.split('@');
    if(data.count()<7)
        return curBlock;
    curBlock.blockIndex = data.at(0);
    curBlock.blockTimestamp = data.at(1);
    curBlock.blockHashPrev = data.at(2);
    curBlock.blockHash = data.at(3);
    curBlock.blockData = data.at(4);
    curBlock.blockMaker = data.at(5);
    curBlock.blockMakerSign = data.at(6);
    return curBlock;
}

void MainWindow::on_ui_query_clicked(){
    QByteArray result = HttpRequest::qtGet(QByteArray(QString(nodeip.split(":").first()+":"+QString::number(basePort)+"/block").toLatin1())+ui->le_querykey_contract->text().toLatin1()+"-"+ui->le_querykey_index->text().toLatin1());
    onnBlock curBlock = createBlock(result);
    QByteArray curData = curBlock.blockData;
    QList<QByteArray> data = result.split('@');
    ui->ui_databrowser->clear();
    if(data.count()>=5){
        ui->ui_index->setText(data.at(0));
        ui->ui_prehash->setText(data.at(2));
        ui->ui_curhash->setText(data.at(3));
        ui->ui_timestamp->setText(data.at(1));
        ui->ui_databrowser->setText(data.at(4));

        QList<QByteArray> listCmd = curData.split('&');
        if(listCmd.count()<2){
            BUG << "bad data: listCmd.count()<2";
            return;
        }
        QByteArray sign = listCmd.at(0);
        QByteArray data1 = listCmd.at(1);
        QList<QByteArray> listData = data1.split('$');

        QByteArray type = listData.at(0);
        QByteArray pubkey = GETADDR(listData.at(1));
        QByteArray name = listData.at(2);
        QByteArray code = listData.at(3);
        QByteArray arg = QByteArray::fromHex(listData.at(4));
        QByteArray maker = GETADDR(curBlock.blockMaker);
        QByteArray makerSign = curBlock.blockMakerSign;

        ui->lb_arg->setText(arg);
        ui->lb_contract->setText(name);
        ui->lb_function->setText(code);
        ui->lb_user->setText(pubkey);
        ui->lb_maker->setText(maker);
        ui->lb_makersign->setText(makerSign);
        ui->lb_type->setText(type);
        ui->lb_sign->setText(sign);

    }else{
        ui->ui_databrowser->setText(result);
    }
}

void MainWindow::on_pushButton_clicked(){
    QString arg;
    arg.append(ui->le_addr->text());
    arg.append("?");
    arg.append(ui->le_number->text());
    HttpRequest::doMethodSet(passwd,nodeip.split(":").first()+":"+nodeip.split(":").last(),ui->le_contract->text(),"transfer",arg.toLatin1().toHex());
}

void MainWindow::on_pb_deploy_clicked(){
    QString arg;
    arg.append(ui->le_argment->text());
    QString code = ui->te_code->toPlainText();
    while((int)code.at(code.count()-1).toLatin1() <= 10){
        code.remove(code.count()-1,1);
    }
    if(code.contains("_G")){
        BUG << "do not use _G in custom program";
        return;
    }
    BUG << code;
    HttpRequest::doDeploy(passwd,nodeip+"/deploy",ui->le_name->text(),code.toLatin1().toHex(),arg.toLatin1().toHex());
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
    ui->le_querykey_index->setText(QString::number(ui->le_querykey_index->text().toInt()+1));
    on_ui_query_clicked();
}

void MainWindow::on_pb_prev_clicked(){
    if(ui->le_querykey_index->text()=="0"){
        return;
    }
    ui->le_querykey_index->setText(QString::number(ui->le_querykey_index->text().toInt()-1));
    on_ui_query_clicked();
}

void MainWindow::on_pushButton_2_clicked(){
    QString code = ui->te_arg->toPlainText();
    if(code.isEmpty()){
        code = "null";
    }else{
        while((int)code.at(code.count()-1).toLatin1() <= 10){
            code.remove(code.count()-1,1);
        }
    }
    QString arg = code.toLatin1().toHex();
    if(ui->le_method->text().left(3)=="get"){
        auto pDoc = HttpRequest::doMethodGet1(passwd,nodeip,ui->le_method_contract->text(),ui->le_method->text(),arg);
        ui->tb_network->append(QString(pDoc.toJson()).remove("\\"));
    }else{
        HttpRequest::doMethodSet(passwd,nodeip,ui->le_method_contract->text(),ui->le_method->text(),arg);
    }
}

void MainWindow::on_pb_image_clicked(){
    if(ui->le_image->text().isEmpty()){
        return;
    }
    ui->tb_image->clear();
    if(!QFile::exists(ui->le_image->text())){
        ui->tb_image->append("image not found");
        return;
    }
    QImage img(ui->le_image->text());
    QByteArray imgData;
    QBuffer imgbuffer;

    imgbuffer.open(QIODevice::WriteOnly);
    img.save(&imgbuffer,"PNG");
    imgData = imgbuffer.data().toHex();
    ui->tb_image->append(imgData);
}

void MainWindow::on_pb_tohex_clicked(){
    ui->te_hex->clear();
    ui->te_hex->append(ui->te_string->toPlainText().toLatin1().toHex());
}

void MainWindow::on_pb_sign_clicked(){
    QString curResult;
    if(ui->sign_cb_type->currentIndex()==0){
        curResult = HttpRequest::doSignMethod(passwd,ui->sign_le_contract->text(),ui->sign_le_method->text(),ui->sign_te_arg->toPlainText().toLatin1().toHex());
    }else if(ui->sign_cb_type->currentIndex()==1){
        curResult = HttpRequest::doSignDeploy(passwd,ui->sign_le_contract->text(),ui->sign_le_method->text(),ui->sign_te_arg->toPlainText().toLatin1().toHex());
    }
    ui->sign_tb_result->clear();
    ui->sign_tb_result->append(curResult);
}

void MainWindow::on_pb_make_clicked(){
    QString curResult;
    curResult = HttpRequest::doMake(passwd,ui->make_le_index->text(),ui->make_le_hashprv->text(),ui->make_le_data->toPlainText().toLatin1());
    ui->make_tb_result->clear();
    ui->make_tb_result->append(curResult);
}

void MainWindow::on_pb_tohash_clicked(){
    ui->tb_tohash->clear();
    if(ui->cb_tohash->currentIndex()==0){
        ui->tb_tohash->append(GETSHA256(ui->te_tohash->toPlainText().toLatin1()));
    }else if(ui->cb_tohash->currentIndex()==1){
        ui->tb_tohash->append(GETADDR(ui->te_tohash->toPlainText().toLatin1()));
    }else if(ui->cb_tohash->currentIndex()==2){
        uint8_t pp[65] = {0};
        uECC_compute_public_key((uint8_t *)QByteArray::fromHex(ui->te_tohash->toPlainText().toLatin1()).data(),pp,uECC_secp256k1());
        QByteArray final = (char *)pp;
        ui->tb_tohash->append(final.left(64).toHex());
    }
}

void MainWindow::on_pb_import_clicked(){
    passwd.computePubkey(ui->te_import->toPlainText().toLatin1());
    ui->lb_pubkey->setText(GETADDR(passwd.pubkey));
    ui->te_import->clear();
    ui->te_import->append(GETADDR(passwd.pubkey));
}

void MainWindow::on_pb_export_clicked(){
    ui->te_export->clear();
    ui->te_export->append(passwd.prikey);
}

void MainWindow::on_pb_toasc_clicked(){
    ui->tb_toasc->clear();
    ui->tb_toasc->append(QByteArray::fromHex(ui->te_toasc->toPlainText().toLatin1()));
}

void MainWindow::doSave(QString pKey,QString pVar){
    QSettings curSet("request.ini",QSettings::IniFormat);
    curSet.setValue(pKey,pVar);
}
QString MainWindow::doLoad(QString pKey){
    QSettings curSet("request.ini",QSettings::IniFormat);
    return curSet.value(pKey).toString();
}

void MainWindow::on_pb_get_req_clicked(){
    doSave("geturl0",ui->le_geturl_req->text());
    emit toGet(ui->le_geturl_req->text());
}

void MainWindow::on_pb_post_req_clicked(){
    doSave("posturl0",ui->le_posturl_req->text());
    doSave("posturi0",ui->le_posturi_req->text());
    doPost(ui->le_posturl_req->text(),ui->le_posturi_req->text());
}

void MainWindow::on_pb_post_req_2_clicked(){
    doSave("posturl1",ui->le_posturl_req_2->text());
    doSave("posturi1",ui->le_posturi_req_2->text());
    doPost(ui->le_posturl_req_2->text(),ui->le_posturi_req_2->text());
}

void MainWindow::on_pb_post_req_3_clicked(){
    doSave("posturl2",ui->le_posturl_req_3->text());
    doSave("posturi2",ui->le_posturi_req_3->text());
    doPost(ui->le_posturl_req_3->text(),ui->le_posturi_req_3->text());
}

void MainWindow::on_pb_post_req_4_clicked(){
    doSave("posturl3",ui->le_posturl_req_4->text());
    doSave("posturi3",ui->le_posturi_req_4->text());
    doPost(ui->le_posturl_req_4->text(),ui->le_posturi_req_4->text());
}

void MainWindow::doPost(QString pUrl,QString pUri){
    emit toPost(pUrl,pUri);
}

void MainWindow::onRsponse(QString pData){
    ui->tb_req->clear();
    ui->tb_req->append(pData);
}





