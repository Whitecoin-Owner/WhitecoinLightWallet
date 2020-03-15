#include "ExchangeWithdrawWidget.h"
#include "ui_ExchangeWithdrawWidget.h"

#include "wallet.h"
#include "ExchangeModePage.h"
#include "control/AssetIconItem.h"
#include "ToolButtonWidget.h"
#include "dialog/TransactionResultDialog.h"


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
    // transactionResultDialog.setInfoText(tr("showWithdrawList !"));
    // transactionResultDialog.pop();

    QStringList withdrawlist = XWCWallet::getInstance()->getWithdrawList();
    int size = withdrawlist.length();
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
 		QString index = withdrawStrList[0];
		index = index.remove("\"");
		index = index.remove("{");
		//Assign address
		QString address = withdrawStrList[1];
		address = address.remove("\"");
		address = address.remove("{");
		//Assign asset
		QStringList withdrawStrList2 = withdrawStrList[3].split(",");		
		QString asset = withdrawStrList2[0];
		asset = asset.remove("\"");
		//Assign amount
		withdrawStrList2 = withdrawStrList[4].split(",");			
		QString amount = withdrawStrList2[0];
		//Assign status
		QString status = withdrawStrList[5];
		status = status.remove("\"");
		status = status.remove("}");
		
        ui->withdrawTableWidget->setRowHeight( i, 42);
		
        ui->withdrawTableWidget->setItem( i, 0, new QTableWidgetItem(asset));
        ui->withdrawTableWidget->setItem( i, 1, new QTableWidgetItem(amount));
        ui->withdrawTableWidget->setItem( i, 2, new QTableWidgetItem(status));
        ui->withdrawTableWidget->setItem( i, 3, new QTableWidgetItem(tr("approve")));
		
        // ui->withdrawTableWidget->setItem( i, 3, new QTableWidgetItem( tr("deposit")));
        // ui->withdrawTableWidget->setItem( i, 4, new QTableWidgetItem( tr("withdraw")));
        // ui->withdrawTableWidget->setItem( i, 5, new QTableWidgetItem( tr("history")));


        AssetIconItem* assetIconItem = new AssetIconItem();
        assetIconItem->setAsset(ui->withdrawTableWidget->item(i,0)->text());
        ui->withdrawTableWidget->setCellWidget(i, 0, assetIconItem);

        for(int j : {3})
        {
			if (j == 3)
            {
				if(ui->withdrawTableWidget->item(i,j))
				{
					ToolButtonWidget *toolButton = new ToolButtonWidget();
					toolButton->setText(ui->withdrawTableWidget->item(i,j)->text());
					ui->withdrawTableWidget->setCellWidget(i,j,toolButton);
					connect(toolButton,&ToolButtonWidget::clicked,std::bind(&ExchangeWithdrawWidget::on_withdrawTableWidget_cellClicked,this,i,j));

                    //if(j == 3)  toolButton->setEnabled(false);
				}
			}	
        }
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
			TransactionResultDialog transactionResultDialog;
        
			QString contractAddress = EXCHANGE_MODE_CONTRACT_ADDRESS;
			QString func = "on_removeWithdrawAssetReq";

			QString params = QString::number(row);
            QString stepCount = QString::number(10000);
                 
            XWCWallet::getInstance()->postRPC( "id-removewithdrawassetreq", toJsonFormat( "invoke_contract",
                                        QJsonArray() << ui->accountComboBox->currentText()
                                        << XWCWallet::getInstance()->currentContractFee() << stepCount
                                        << EXCHANGE_MODE_XDTT_TOKENCONTRACT_ADDRESS
                                        << func  << params)); 		
			
			transactionResultDialog.setInfoText(tr("RemoveWithdrawAssetReq Command!"));
            transactionResultDialog.setDetailText(toJsonFormat("invoke_contract",
                                        QJsonArray() << ui->accountComboBox->currentText()
                                        << XWCWallet::getInstance()->currentContractFee() << stepCount
                                        << EXCHANGE_MODE_XDTT_TOKENCONTRACT_ADDRESS
                                        << func  << params));
			transactionResultDialog.pop();
			
		
			params = "";
			XWCWallet::getInstance()->postRPC( "id-invoke_contract_offline-getWithdrawAssetRequests", toJsonFormat( "invoke_contract_offline",
                                        QJsonArray() << ui->accountComboBox->currentText()
                                        << EXCHANGE_MODE_XDTT_TOKENCONTRACT_ADDRESS
										<< func  << params)); 

			transactionResultDialog.setInfoText(tr("Get Withdraw Asset Request Again!"));
            transactionResultDialog.setDetailText(toJsonFormat( "invoke_contract_offline",
                                        QJsonArray() << ui->accountComboBox->currentText()
                                        << EXCHANGE_MODE_XDTT_TOKENCONTRACT_ADDRESS
										<< func  << params));
			transactionResultDialog.pop();	
		
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
    TransactionResultDialog transactionResultDialog;
    transactionResultDialog.setInfoText(tr("Approve the process!"));
    transactionResultDialog.pop();

    showWithdrawList();
}

void ExchangeWithdrawWidget::on_refreshBtn_clicked()
{
    //TransactionResultDialog transactionResultDialog;
    //transactionResultDialog.setInfoText(tr("on_refreshBtn_clicked !"));
    //transactionResultDialog.pop();

    showWithdrawList();
}
