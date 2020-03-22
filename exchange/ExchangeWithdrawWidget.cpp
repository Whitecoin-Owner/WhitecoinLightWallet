#include "ExchangeWithdrawWidget.h"
#include "ui_ExchangeWithdrawWidget.h"

#include "wallet.h"
#include "ExchangeModePage.h"
#include "control/AssetIconItem.h"
#include "ToolButtonWidget.h"
#include "dialog/TransactionResultDialog.h"
#include <QtCore/qmath.h>

ExchangeWithdrawWidget::ExchangeWithdrawWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExchangeWithdrawWidget)
{
    ui->setupUi(this);

    ui->withdrawTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->withdrawTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->withdrawTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->withdrawTableWidget->setFrameShape(QFrame::NoFrame);
    ui->withdrawTableWidget->setMouseTracking(true);
    ui->withdrawTableWidget->setShowGrid(false);//隐藏表格线
    ui->withdrawTableWidget->horizontalHeader()->setSectionsClickable(true);
    ui->withdrawTableWidget->horizontalHeader()->setVisible(true);
    ui->withdrawTableWidget->verticalHeader()->setVisible(false);
    ui->withdrawTableWidget->setColumnWidth(0,140);
    ui->withdrawTableWidget->setColumnWidth(1,130);
    ui->withdrawTableWidget->setColumnWidth(2,125);
    ui->withdrawTableWidget->setColumnWidth(3,80);

    init();
}

ExchangeWithdrawWidget::~ExchangeWithdrawWidget()
{
    delete ui;
}

void ExchangeWithdrawWidget::init()
{
    TransactionResultDialog transactionResultDialog;
    QStringList accounts = XWCWallet::getInstance()->accountInfoMap.keys();
    ui->accountComboBox->addItems(accounts);
//    connect( ui->accountComboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(onAccountComboBoxCurrentIndexChanged(const QString &)));

    if(accounts.contains(XWCWallet::getInstance()->currentAccount))
    {
        ui->accountComboBox->setCurrentText(XWCWallet::getInstance()->currentAccount);
    }
//    onAccountComboBoxCurrentIndexChanged(ui->accountComboBox->currentText());

    QString contractAddress = EXCHANGE_MODE_CONTRACT_ADDRESS;
    QString func = "getWithdrawAsset";

    QString params = "";
    XWCWallet::getInstance()->postRPC( "id-invoke_contract_offline-getWithdrawAssetRequests", toJsonFormat( "invoke_contract_offline",
                                                                                   QJsonArray() << ui->accountComboBox->currentText()
                                                                                   << contractAddress
																				   << func  << params)); 
																				   
	showWithdrawList();
}

void ExchangeWithdrawWidget::showWithdrawList()
{
    TransactionResultDialog transactionResultDialog;
    //transactionResultDialog.setInfoText(tr("Enter showWithdrawList()"));
    //transactionResultDialog.pop();

    QStringList withdrawlist = XWCWallet::getInstance()->getWithdrawList();
    bool listEmpty = withdrawlist.isEmpty();
    int size = withdrawlist.length();
    int loopindex = 0;
    int account_rowlength = 0;
    if (!listEmpty)
    {
	ui->withdrawTableWidget->setRowCount(0);
    ui->withdrawTableWidget->setRowCount(size);

    for(int i = 0; i < size; i++)
    {
        // transactionResultDialog.setInfoText(tr("Loop Index "));
        // transactionResultDialog.setDetailText(QString::number(i));
        // transactionResultDialog.pop();

        // transactionResultDialog.setInfoText(tr("Value Result String List"));
        // transactionResultDialog.setDetailText(withdrawlist[i]);
        // transactionResultDialog.pop();

        QString withdrawListString = withdrawlist[i];
        QStringList withdrawStrList = withdrawListString.split(":");

		//Assign Index
            QString index ;//= withdrawStrList[0];
            QString address ;//= withdrawStrList[1];
            QStringList withdrawStrList2;// = withdrawStrList[3].split(",");
            QStringList withdrawStrList3;// = withdrawStrList[3].split(",");
            QString asset ;//= withdrawStrList2[0];
            QString amount ;//= withdrawStrList2[0];
            QString status ;//= withdrawStrList[5];
            QString withdrawStrList_ana = withdrawStrList[0];
            withdrawStrList_ana = withdrawStrList_ana.remove("\"");
            withdrawStrList_ana = withdrawStrList_ana.remove("{");
            if (withdrawStrList_ana.startsWith("XWC"))
            {
                index = "Nil";
                address = withdrawStrList[0];
                withdrawStrList2 = withdrawStrList[2].split(",");
                withdrawStrList3 = withdrawStrList[3].split(",");
                status = withdrawStrList[4];
            } else {
                index = withdrawStrList[0];
                address = withdrawStrList[1];
                withdrawStrList2 = withdrawStrList[3].split(",");
                withdrawStrList3 = withdrawStrList[4].split(",");
                status = withdrawStrList[5];
            }
            //Assign Index
            index = index.remove("\"");
            index = index.remove("{");
            //Assign address
            address = address.remove("\"");
            address = address.remove("{");
            //Assign asset
            asset = withdrawStrList2[0];
            asset = asset.remove("\"");
            //Assign amount
            amount = withdrawStrList3[0];
            int amount_int = amount.toInt();
            double amount_double = amount_int / qPow(10,ASSET_PRECISION);;
            amount = QString("%1").arg(amount_double);
            //Assign status
            status = status.remove("\"");
            status = status.remove("}");
            AccountInfo accountInfo = XWCWallet::getInstance()->accountInfoMap.value(ui->accountComboBox->currentText());
            QString currentAccountAddr = accountInfo.address;
            if (currentAccountAddr == address || currentAccountAddr == EXCHANGE_MODE_WITHDRAW_ADMIN_ADDRESS)
            {
                ui->withdrawTableWidget->setRowHeight( loopindex, 42);
                ui->withdrawTableWidget->setItem( loopindex, 0, new QTableWidgetItem(asset));
                ui->withdrawTableWidget->setItem( loopindex, 1, new QTableWidgetItem(amount));
                ui->withdrawTableWidget->setItem( loopindex, 2, new QTableWidgetItem(status));
                ui->withdrawTableWidget->setItem( loopindex, 3, new QTableWidgetItem(tr("approve")));
                ui->withdrawTableWidget->setItem( loopindex, 4, new QTableWidgetItem(address));
        // ui->withdrawTableWidget->setItem( i, 4, new QTableWidgetItem( tr("withdraw")));
        // ui->withdrawTableWidget->setItem( i, 5, new QTableWidgetItem( tr("history")));


                AssetIconItem* assetIconItem = new AssetIconItem();
                assetIconItem->setAsset(ui->withdrawTableWidget->item(loopindex,0)->text());
                ui->withdrawTableWidget->setCellWidget(loopindex, 0, assetIconItem);

                for(int j : {3})
                {
                    if (j == 3)
                    {
                        if(ui->withdrawTableWidget->item(loopindex,j))
                        {
                            ToolButtonWidget *toolButton = new ToolButtonWidget();
                            toolButton->setText(ui->withdrawTableWidget->item(loopindex,j)->text());
                            ui->withdrawTableWidget->setCellWidget(loopindex,j,toolButton);
                            connect(toolButton,&ToolButtonWidget::clicked,std::bind(&ExchangeWithdrawWidget::on_withdrawTableWidget_cellClicked,this,i,j));

                        if (currentAccountAddr != EXCHANGE_MODE_WITHDRAW_ADMIN_ADDRESS) {
                            if(j == 3)  toolButton->setEnabled(false);
                        }

                        }
                    }
                }

                loopindex = loopindex + 1;

            }
        }
    } else {
        ui->withdrawTableWidget->setRowCount(0);
    }
    tableWidgetSetItemZebraColor(ui->withdrawTableWidget);
}

void ExchangeWithdrawWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(239,242,245),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(239,242,245),Qt::SolidPattern));

    painter.drawRect(rect());
}

void ExchangeWithdrawWidget::on_withdrawTableWidget_cellClicked(int row, int column)
{
    if(column == 3)
    {
        if(ui->withdrawTableWidget->item(row,0))
        {
            QString asset_symbol = ui->withdrawTableWidget->item(row,0)->data(Qt::DisplayRole).toString();
            QString asset_amount = ui->withdrawTableWidget->item(row,1)->data(Qt::DisplayRole).toString();
            QString asset_addr = ui->withdrawTableWidget->item(row,4)->data(Qt::DisplayRole).toString();
            QString stepCount = QString::number(10000);
            QString func;
            QString params;
/*
            TransactionResultDialog transactionResultDialog;
            transactionResultDialog.setInfoText(tr("ui->withdrawTableWidget->item(row,0) asset_symbol!"));
            transactionResultDialog.setDetailText(asset_symbol);
            transactionResultDialog.pop();
            transactionResultDialog.setInfoText(tr("ui->withdrawTableWidget->item(row,1) asset_amount!"));
            transactionResultDialog.setDetailText(asset_amount);
            transactionResultDialog.pop();
            transactionResultDialog.setInfoText(tr("ui->withdrawTableWidget->item(row,4) asset_addr!"));
            transactionResultDialog.setDetailText(asset_addr);
            transactionResultDialog.pop();
*/
			QString contractAddress = EXCHANGE_MODE_CONTRACT_ADDRESS;
            //ui->withdrawTableWidget->setItem( i, 0, new QTableWidgetItem(asset));
            //ui->withdrawTableWidget->setItem( i, 1, new QTableWidgetItem(amount));
            //ui->withdrawTableWidget->setItem( i, 2, new QTableWidgetItem(status));
            //ui->withdrawTableWidget->setItem( i, 3, new QTableWidgetItem(tr("approve")));
            //ui->withdrawTableWidget->setItem( i, 4, new QTableWidgetItem(address));

            //具体实现提现请求 XWC / WNTT/XDTT
            // XWC 直接调用on_withdraw_xwc完成2个功能，1. 把XWC从合约转到指定地址 2.在Exchange合约帐目上减掉提现XWC个数 Transfer from Contract EXCHANGE_MODE_CONTRACT_ADDRESS
            if (asset_symbol == "XWC") {
                func = "on_withdraw_xwc";
                params = QString("%1,%2").arg(asset_amount).arg(asset_addr);

                XWCWallet::getInstance()->postRPC( "id-on_withdraw_xwc", toJsonFormat( "invoke_contract",
                                            QJsonArray() << ui->accountComboBox->currentText()
                                            << XWCWallet::getInstance()->currentContractFee() << stepCount
                                            << contractAddress
                                            << func  << params));
/*
                transactionResultDialog.setInfoText(tr("ui->withdrawTableWidget->item(row,4) asset_addr!"));
                transactionResultDialog.setDetailText(toJsonFormat( "invoke_contract",
                                                                    QJsonArray() << ui->accountComboBox->currentText()
                                                                    << XWCWallet::getInstance()->currentContractFee() << stepCount
                                                                    << contractAddress
                                                                    << func  << params));
                transactionResultDialog.pop();
*/
            }
            else
            //XDTT WNTT分2个步骤，1.调用XDTT/WNTT transfer 从Exchange XDTT/WNTT转到指定地址，2.在Exchange合约WNTT/XDTT帐目上减掉对应提现个数
            //这两个是转到合约XDTT WNTT当中去的转完之后还要在XWC Exchange 合约当中调用账目记录 把对应账号的XDTT WNTT给添加进去
            {
                if (asset_symbol == "XDTT")
                {
                    // //1
                    double tmp = asset_amount.toDouble();
                    tmp = tmp * qPow(10,ASSET_PRECISION);
                   // /*transactionResultDialog.setInfoText(tr("tmp * qPow(10,ASSET_PRECISION)"));
                    // transactionResultDialog.setDetailText(QString::number(tmp));
                    // transactionResultDialog.pop();*/

                    QString params = QString("%1,%2,%3").arg(asset_addr).arg(QString::number(tmp,10,8)).arg("memo");
 /*                 transactionResultDialog.setInfoText(tr("QString(%1,%2,%3)"));
                    transactionResultDialog.setDetailText(params);
                    transactionResultDialog.pop();
*/
                    XWCWallet::getInstance()->postRPC( "id-transfer_to_contract_token", toJsonFormat( "invoke_contract",
                                                         QJsonArray() << ui->accountComboBox->currentText()
                                                         << XWCWallet::getInstance()->currentContractFee() << stepCount
                                                         << EXCHANGE_MODE_XDTT_TOKENCONTRACT_ADDRESS
                                                         << "transfer"  << params));
/*
                    transactionResultDialog.setInfoText(tr("ui->withdrawTableWidget->item(row,4) asset_addr!"));
                    transactionResultDialog.setDetailText(toJsonFormat( "invoke_contract",
                                                                        QJsonArray() << ui->accountComboBox->currentText()
                                                                        << XWCWallet::getInstance()->currentContractFee() << stepCount
                                                                        << EXCHANGE_MODE_XDTT_TOKENCONTRACT_ADDRESS
                                                                        << "transfer"  << params));
                    transactionResultDialog.pop();*/
                    // //2
                    func = "on_withdraw_xdt";
                    params = QString("%1,%2").arg(asset_amount).arg(asset_addr);

                    XWCWallet::getInstance()->postRPC( "id-on_withdraw_xdt", toJsonFormat( "invoke_contract",
                                                 QJsonArray() << ui->accountComboBox->currentText()
                                                 << XWCWallet::getInstance()->currentContractFee() << stepCount
                                                 << contractAddress
                                                 << func  << params));
/*
                    transactionResultDialog.setInfoText(tr("ui->withdrawTableWidget->item(row,4) asset_addr!"));
                    transactionResultDialog.setDetailText(toJsonFormat( "invoke_contract",
                                                                        QJsonArray() << ui->accountComboBox->currentText()
                                                                        << XWCWallet::getInstance()->currentContractFee() << stepCount
                                                                        << contractAddress
                                                                        << func  << params));
                    transactionResultDialog.pop();
*/
                }
                else if (asset_symbol == "WNTT")
                {
                // //1
                double tmp = asset_amount.toDouble();
                tmp = tmp * qPow(10,ASSET_PRECISION);
                // /*transactionResultDialog.setInfoText(tr("tmp * qPow(10,ASSET_PRECISION)"));
                // transactionResultDialog.setDetailText(QString::number(tmp));
                // transactionResultDialog.pop();*/

                QString params = QString("%1,%2,%3").arg(asset_addr).arg(QString::number(tmp,10,8)).arg("memo");
/*              transactionResultDialog.setInfoText(tr("QString(%1,%2,%3)"));
                transactionResultDialog.setDetailText(params);
                transactionResultDialog.pop();
*/
                XWCWallet::getInstance()->postRPC( "id-transfer_to_contract_token", toJsonFormat( "invoke_contract",
                                                     QJsonArray() << ui->accountComboBox->currentText()
                                                     << XWCWallet::getInstance()->currentContractFee() << stepCount
                                                     << EXCHANGE_MODE_WNTT_TOKENCONTRACT_ADDRESS
                                                     << "transfer"  << params));
/*
                transactionResultDialog.setInfoText(tr("ui->withdrawTableWidget->item(row,4) asset_addr!"));
                transactionResultDialog.setDetailText(toJsonFormat( "invoke_contract",
                                                                    QJsonArray() << ui->accountComboBox->currentText()
                                                                    << XWCWallet::getInstance()->currentContractFee() << stepCount
                                                                    << EXCHANGE_MODE_WNTT_TOKENCONTRACT_ADDRESS
                                                                    << "transfer"  << params));
                transactionResultDialog.pop();*/
                // //2
                func = "on_withdraw_wnt";
                params = QString("%1,%2").arg(asset_amount).arg(asset_addr);

                XWCWallet::getInstance()->postRPC( "id-on_withdraw_wnt", toJsonFormat( "invoke_contract",
                                             QJsonArray() << ui->accountComboBox->currentText()
                                             << XWCWallet::getInstance()->currentContractFee() << stepCount
                                             << contractAddress
                                             << func  << params));
/*
                transactionResultDialog.setInfoText(tr("ui->withdrawTableWidget->item(row,4) asset_addr!"));
                transactionResultDialog.setDetailText(toJsonFormat( "invoke_contract",
                                                                    QJsonArray() << ui->accountComboBox->currentText()
                                                                    << XWCWallet::getInstance()->currentContractFee() << stepCount
                                                                    << contractAddress
                                                                    << func  << params));
                transactionResultDialog.pop();*/
                }
            }

            // 执行以后把提现强求删掉
            func = "on_removeWithdrawAssetReq";

            params = QString::number(row);
            stepCount = QString::number(10000);

            XWCWallet::getInstance()->postRPC( "id-removewithdrawassetreq", toJsonFormat( "invoke_contract",
                                        QJsonArray() << ui->accountComboBox->currentText()
                                        << XWCWallet::getInstance()->currentContractFee() << stepCount
                                        << contractAddress
                                        << func  << params)); 		
/*
			transactionResultDialog.setInfoText(tr("RemoveWithdrawAssetReq Command!"));
            transactionResultDialog.setDetailText(toJsonFormat("invoke_contract",
                                        QJsonArray() << ui->accountComboBox->currentText()
                                        << XWCWallet::getInstance()->currentContractFee() << stepCount
                                        << contractAddress
                                        << func  << params));
			transactionResultDialog.pop();
*/
            //刷新提现请求列表
            func = "getWithdrawAsset";
			params = "";

			XWCWallet::getInstance()->postRPC( "id-invoke_contract_offline-getWithdrawAssetRequests", toJsonFormat( "invoke_contract_offline",
                                        QJsonArray() << ui->accountComboBox->currentText()
                                        << contractAddress
                                        << func  << params));
/*
			transactionResultDialog.setInfoText(tr("Get Withdraw Asset Request Again!"));
            transactionResultDialog.setDetailText(toJsonFormat( "invoke_contract_offline",
                                        QJsonArray() << ui->accountComboBox->currentText()
                                        << contractAddress
										<< func  << params));
			transactionResultDialog.pop();	
*/
			showWithdrawList();
		
		}

        return;
    }

    if(column == 4)
    {
        if(ui->withdrawTableWidget->item(row,0))
        {
//            WithdrawExchangeContractDialog withdrawExchangeContractDialog(true);
//            withdrawExchangeContractDialog.setCurrentAsset(ui->withdrawTableWidget->item(row,0)->text());
//            withdrawExchangeContractDialog.pop();
        }

    }
}

void ExchangeWithdrawWidget::on_hideZeroCheckBox_stateChanged(int arg1)
{
    int rowCount = ui->withdrawTableWidget->rowCount();
    //if(ui->hideZeroCheckBox->isChecked())
    //{
    //    for(int i = 0; i < rowCount; i++)
    //    {
/*            if( ui->withdrawTableWidget->item( i, 1)->text().toDouble() == 0
                    & ui->withdrawTableWidget->item( i, 2)->text().toDouble() == 0)
            {
                ui->withdrawTableWidget->hideRow(i);
            }
            else
            {
                ui->withdrawTableWidget->showRow(i);
            }*/
    //    }
    //}
    //else
    //{
        for(int i = 0; i < rowCount; i++)
        {
            ui->withdrawTableWidget->showRow(i);
        }
    //}
}

void ExchangeWithdrawWidget::onAccountComboBoxCurrentIndexChanged(const QString &arg1)
{
/*
        TransactionResultDialog transactionResultDialog;
        transactionResultDialog.setInfoText(tr("onAccountComboBoxCurrentIndexChanged !"));
        transactionResultDialog.setDetailText(arg1);
        transactionResultDialog.pop();

    XWCWallet::getInstance()->currentAccount = ui->accountComboBox->currentText();
    XWCWallet::getInstance()->assetExchangeBalanceMap.clear();
*/
    QString contractAddress = EXCHANGE_MODE_CONTRACT_ADDRESS;
    QString func = "getWithdrawAsset";

    QString params = "";
    XWCWallet::getInstance()->postRPC( "id-invoke_contract_offline-getWithdrawAssetRequests", toJsonFormat( "invoke_contract_offline",
                                                                                   QJsonArray() << ui->accountComboBox->currentText()
                                                                                   << contractAddress
                                                                                   << func  << params));
/*
    TransactionResultDialog transactionResultDialog;
    transactionResultDialog.setInfoText(tr("Approve the process!"));
    transactionResultDialog.pop();
*/
    showWithdrawList();
}

void ExchangeWithdrawWidget::on_refreshBtn_clicked()
{
    //TransactionResultDialog transactionResultDialog;
    //transactionResultDialog.setInfoText(tr("on_refreshBtn_clicked !"));
    //transactionResultDialog.pop();

    QString contractAddress = EXCHANGE_MODE_CONTRACT_ADDRESS;
    QString func = "getWithdrawAsset";
    QString params = "";

    XWCWallet::getInstance()->postRPC( "id-invoke_contract_offline-getWithdrawAssetRequests", toJsonFormat( "invoke_contract_offline",
                                QJsonArray() << ui->accountComboBox->currentText()
                                << contractAddress
                                << func  << params));

    /*transactionResultDialog.setInfoText(tr("Get Withdraw Asset Request Again!"));
    transactionResultDialog.setDetailText(toJsonFormat( "invoke_contract_offline",
                                QJsonArray() << ui->accountComboBox->currentText()
                                << contractAddress
                                << func  << params));
    transactionResultDialog.pop();
    */
    showWithdrawList();
}
