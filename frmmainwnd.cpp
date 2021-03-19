#include "frmmainwnd.h"
#include "ui_frmmainwnd.h"
#include <QHeaderView>
#include <QMessageBox>

frmMainWnd::frmMainWnd(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::frmMainWnd)
{
    ui->setupUi(this);

    setWindowTitle("PowerPoint");
    setWindowState(Qt::WindowMaximized);
    ui->undoView->setVisible(false);

    ui->tbl_Slide->horizontalHeader()->setVisible(false);
    ui->tbl_Slide->verticalHeader()->setVisible(false);
    ui->tbl_Slide->setRowCount(1);
    ui->tbl_Slide->setColumnCount(1);
    ui->tbl_Slide->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    SlideFormat(0);
    ui->DocumentArea->removeWidget(ui->page);
    ui->SlideViewAction->setChecked(true);

    m_undoGroup = new QUndoGroup(this);
    ui->undoView->setGroup(m_undoGroup);
    ui->undoView->setCleanIcon(QIcon(":/Resource/ok.png"));

    QAction *undoAction = m_undoGroup->createUndoAction(this);
    QAction *redoAction = m_undoGroup->createRedoAction(this);
    undoAction->setIcon(QIcon(":/Resource/undo.png"));
    redoAction->setIcon(QIcon(":/Resource/redo.png"));
    redoAction->setShortcut(QString("Ctrl+Z"));
    undoAction->setShortcut(QString("Ctrl+Y"));
    ui->menuShape->insertAction(ui->menuShape->actions().at(0), undoAction);
    ui->menuShape->insertAction(undoAction, redoAction);

    ui->toolBar->addAction(undoAction);
    ui->toolBar->addAction(redoAction);

    shapeColor = QColor("blue");
    //Text Input
    textlabel = new QLabel;
    textlabel->setText("        Text :  ");
    textlabel->setFont(QFont("Arial Rounded MT Bold", 10, 60));

    text = new QLineEdit;
    text->setFixedWidth(400);
    text->setFont(QFont("Arial Rounded MT Bold", 10, 60));
    text->setClearButtonEnabled(true);
    connect(text,SIGNAL(textChanged(QString)),this,SLOT(addText(QString)));

    fontlabel = new QLabel;
    fontlabel->setText("             Font :  ");
    fontlabel->setFont(QFont("Arial Rounded MT Bold", 10, 60));

    fontcombo = new QFontComboBox;
    fontcombo->setFont(QFont("MS Gothic", 8, 60));
    fontcombo->setFixedWidth(200);
    fontcombo->setCurrentText("Arial Rounded MT Bold");
    connect(fontcombo,SIGNAL(activated(QString)),this,SLOT(setFontfamily(QString)));
    fontsizecombo = new QComboBox;
    fontsizecombo->setFont(QFont("MS Gothic", 8, 60));
    fontsizecombo->setFixedWidth(50);
    fontsizecombo->setEditable(true);
    for(int i = 8; i <= 12; i++)
        fontsizecombo->addItem(QString::number(i));

    for(int i = 14; i <= 36; i++)
    {
        if(i % 2 == 0)
            fontsizecombo->addItem(QString::number(i));
    }
    fontsizecombo->setCurrentIndex(0);
    connect(fontsizecombo,SIGNAL(activated(QString)),this,SLOT(setFontsize(QString)));

    //Shape Format
    weightlabel = new QLabel;
    weightlabel->setText("    Weight : ");
    weightlabel->setFont(QFont("Arial Rounded MT Bold", 10, 60));
    ui->toolBar->addWidget(weightlabel);
    spinweight = new QSpinBox;
    spinweight->setMinimum(1);
    spinweight->setMaximum(20);
    spinweight->setFont(QFont("Arial Rounded MT Bold", 10, 60));
    connect(spinweight,SIGNAL(valueChanged(int)),this,SLOT(setShapeWidth(int)));

    ui->toolBar->addWidget(spinweight);
    typelabel = new QLabel;
    typelabel->setText("     Type : ");
    typelabel->setFont(QFont("Arial Rounded MT Bold", 10, 60));
    ui->toolBar->addWidget(typelabel);
    typecombo = new QComboBox;
    typecombo->setFont(QFont("Arial Rounded MT Bold", 10, 60));
    typecombo->addItem("SolidLine");
    typecombo->addItem("DashLine");
    typecombo->addItem("DotLine");
    ui->toolBar->addWidget(typecombo);
    connect(typecombo,SIGNAL(currentIndexChanged(int)),this,SLOT(setShapeType(int)));

    newDocument();
    updateActions();
    ui->tbl_Slide->setCurrentCell(0,0);
}

frmMainWnd::~frmMainWnd()
{
    delete ui;
}


void frmMainWnd::NewSlide()
{
    ui->tbl_Slide->insertRow(ui->tbl_Slide->rowCount());
    SlideFormat(ui->tbl_Slide->rowCount() - 1);
}

void frmMainWnd::SlideFormat(int row)
{
    ui->tbl_Slide->setItem(row,0, new QTableWidgetItem("Slide" + QString::number(row + 1)));
    ui->tbl_Slide->item(row,0)->setFont(QFont("Arial Black",24,60));
    ui->tbl_Slide->item(row,0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbl_Slide->item(row,0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui->tbl_Slide->setRowHeight(row,120);
}

void frmMainWnd::on_NewSlideAction_triggered()
{
    NewSlide();
    ui->tbl_Slide->setCurrentCell(ui->tbl_Slide->rowCount() - 1, 0);
    newDocument();
    updateActions();
}

void frmMainWnd::on_RemoveSlideAction_triggered()
{
      if(ui->tbl_Slide->rowCount() > 1)
      {
          ui->tbl_Slide->removeRow(ui->tbl_Slide->currentRow());
          //ui->DocumentArea->removeWidget();
          for(int i = 0; i < ui->tbl_Slide->rowCount(); i++)
              ui->tbl_Slide->item(i,0)->setText("Slide" + QString::number(i + 1));

          removeDocument(currentDocument());
      }
}

void frmMainWnd::on_actionClose_triggered()
{
    closeDocument();
}

void frmMainWnd::newDocument()
{
    addDocument(new Document());
}

void frmMainWnd::addDocument(Document *doc)
{
    if(ui->DocumentArea->indexOf(doc) != -1)
        return;

    m_undoGroup->addStack(doc->undoStack());
    ui->DocumentArea->addWidget(doc);
    connect(doc, SIGNAL(currentShapeChanged(QString)), this, SLOT(updateActions()));
    connect(doc,SIGNAL(currentShapetextChanged(QString)),this,SLOT(currentShapeText(QString)));
    connect(doc,SIGNAL(currentShapeColorChanged(QColor)),this,SLOT(currentShapeColorChanged(QColor)));
    connect(doc,SIGNAL(currentShapePenstyleChanged(int)),this,SLOT(currentShapePenstyleChanged(int)));
    connect(doc,SIGNAL(currentShapePenweightChanged(int)),this,SLOT(currentShapePenweightChanged(int)));
    connect(doc,SIGNAL(currentShapeFontsizeChanged(int)),this,SLOT(currentShapeFontsizeChanged(int)));
    connect(doc,SIGNAL(currentShapeFontfamilyChanged(QString)),this,SLOT(currentShapeFontfamilyChanged(QString)));
    connect(doc->undoStack(), SIGNAL(indexChanged(int)), this, SLOT(updateActions()));
    connect(doc->undoStack(), SIGNAL(cleanChanged(bool)), this, SLOT(updateActions()));

    setCurrentDocument(doc);
}

void frmMainWnd::setCurrentDocument(Document *doc)
{
    ui->DocumentArea->setCurrentWidget(doc);
}

Document *frmMainWnd::currentDocument() const
{
    return qobject_cast<Document*>(ui->DocumentArea->currentWidget());
}

void frmMainWnd::updateActions()
{
    Document *doc = currentDocument();
    m_undoGroup->setActiveStack(doc == 0 ? 0 : doc->undoStack());
    QString shapeName = doc == 0 ? QString() : doc->currentShapeName();

    ui->actionAddCircle->setEnabled(doc != 0);
    ui->actionAddRectangle->setEnabled(doc != 0);
    ui->actionAddTriangle->setEnabled(doc != 0);
    ui->actionAddLine->setEnabled(doc != 0);
    ui->actionSave->setEnabled(doc != 0 && !doc->undoStack()->isClean());
    ui->actionExportImage->setEnabled(doc != 0 && !doc->undoStack()->isClean());

    if (shapeName.isEmpty()) {
        ui->actionRemoveShape->setEnabled(false);
        ui->actionSave->setEnabled(false);
        ui->actionExportImage->setEnabled(false);
    } else {
        ui->actionRemoveShape->setEnabled(true);
        ui->actionSave->setEnabled(true);
        ui->actionExportImage->setEnabled(true);
    }

    if (doc != 0) {
        int index = ui->DocumentArea->indexOf(doc);
        Q_ASSERT(index != -1);

        if (doc->undoStack()->count() == 0)
            doc->undoStack()->setUndoLimit(10);
    }
}

void frmMainWnd::on_tbl_Slide_cellClicked(int row, int column)
{
    if(column == 0)
        ui->DocumentArea->setCurrentIndex(row);

    updateActions();
}

void frmMainWnd::removeDocument(Document *doc)
{
    int index = ui->DocumentArea->indexOf(doc);
    if (index == -1)
        return;

    ui->DocumentArea->removeWidget(doc);
    m_undoGroup->removeStack(doc->undoStack());
    disconnect(doc, SIGNAL(currentShapeChanged(QString)), this, SLOT(updateActions()));
    disconnect(doc->undoStack(), SIGNAL(indexChanged(int)), this, SLOT(updateActions()));
    disconnect(doc->undoStack(), SIGNAL(cleanChanged(bool)), this, SLOT(updateActions()));

    if (ui->DocumentArea->count() == 0) {
        newDocument();
        updateActions();
    }

    if(index != 0)
    {
        on_tbl_Slide_cellClicked(index-1,0);
        ui->tbl_Slide->setCurrentCell(index-1,0);
    }else
    {
        on_tbl_Slide_cellClicked(0,0);
        ui->tbl_Slide->setCurrentCell(0,0);
    }
}

void frmMainWnd::openDocument()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    QApplication::applicationDirPath(),
                                                    tr("Projects (*.rs)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this,
                            tr("File error"),
                            tr("Failed to open\n%1").arg(fileName));
        return;
    }

    Document *doc = currentDocument();
    for(int i = 0; i < ui->tbl_Slide->rowCount(); i++)
    {
        ui->DocumentArea->setCurrentIndex(0);
        ui->DocumentArea->removeWidget(doc);
    }
    ui->tbl_Slide->setRowCount(0);

    QTextStream stream(&file);
    QString data = stream.readAll();
    data = data.mid(0,data.length() - 8);
    for(int i = 0; i < data.split(parsingcode).count(); i++)
    {
        newDocument();
        ui->DocumentArea->setCurrentIndex(i);
        doc = currentDocument();
        QString content = data.split(parsingcode).at(i);
        QTextStream partstream(&content);

        if (!doc->load(partstream)) {
            QMessageBox::warning(this,
                                tr("Parse error"),
                                tr("Failed to parse\n%1").arg(fileName));
            delete doc;
            return;
        }

        ui->tbl_Slide->insertRow(i);
        SlideFormat(i);
        addDocument(doc);
    }

    ui->tbl_Slide->setCurrentCell(0,0);
    ui->DocumentArea->setCurrentIndex(0);
}

void frmMainWnd::saveDocument()
{
    ui->DocumentArea->setCurrentIndex(0);
    Document *doc = currentDocument();
    if (doc == 0)
        return;

    for (;;) {
        QString fileName = doc->fileName();

        if (fileName.isEmpty())
            fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                    QApplication::applicationDirPath() + "/untitled.rs",
                                                    tr("Projects (*.rs)"));
        if (fileName.isEmpty())
            break;

        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this,
                                tr("File error"),
                                tr("Failed to open\n%1").arg(fileName));
            doc->setFileName(QString());
        } else {
            for(int i = 0; i < ui->DocumentArea->count(); i++)
            {
                ui->DocumentArea->setCurrentIndex(i);
                doc = currentDocument();
                QTextStream stream(&file);
                doc->save(stream);
            }
            doc->setFileName(fileName);

            int index = ui->DocumentArea->indexOf(doc);
            Q_ASSERT(index != -1);

            break;
        }
    }
}

void frmMainWnd::exportImage()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                      "/home",
                                                      QFileDialog::ShowDirsOnly
                                                      | QFileDialog::DontResolveSymlinks);
    if (! dir.isNull())
    {
        for(int i = 0; i < ui->tbl_Slide->rowCount(); i++)
        {
            ui->DocumentArea->setCurrentIndex(i);
            Document *doc = currentDocument();
            QPixmap pixmap = doc->grab(doc->rect());
            pixmap.save(dir + "/Slide" + QString::number(i+1) + ".jpg");
        }
        //fileDialog
    }
}

void frmMainWnd::closeDocument()
{
    Document *doc = currentDocument();
    if (doc == 0)
        return;

    if (!doc->undoStack()->isClean()) {
        int button
            = QMessageBox::warning(this,
                            tr("Unsaved changes"),
                            tr("Would you like to save this document?"),
                            QMessageBox::Yes, QMessageBox::No);
        if (button == QMessageBox::Yes)
        {
            saveDocument();
            close();
        }else{
            close();
        }
    }else{
        close();
    }

    removeDocument(doc);
    delete doc;
}

static QRect randomRect(const QSize &s)
{
    QSize min = Shape::minSize;

    int left = (int) ((0.0 + s.width() - min.width())*(rand()/(RAND_MAX + 1.0)));
    int top = (int) ((0.0 + s.height() - min.height())*(rand()/(RAND_MAX + 1.0)));
    int width = 200;
    int height = 200;

    return QRect(left, top, width, height);
}

void frmMainWnd::addShape()
{
    Document *doc = currentDocument();
    if (doc == 0)
        return;

    Shape::Type type;

    if (sender() == ui->actionAddCircle)
        type = Shape::Circle;
    else if (sender() == ui->actionAddRectangle)
        type = Shape::Rectangle;
    else if (sender() == ui->actionAddTriangle)
        type = Shape::Triangle;
    else if(sender() == ui->actionAddLine)
        type = Shape::Line;
    else return;

    Shape newShape(type, shapeColor, randomRect(doc->size()),spinweight->value(),typecombo->currentIndex()+1, "",
                   QFont(fontcombo->currentText(),fontsizecombo->currentText().toInt(),60));
    doc->undoStack()->push(new AddShapeCommand(doc, newShape));
}

void frmMainWnd::on_actionAddCircle_triggered()
{
    addShape();
    updateActions();
}

void frmMainWnd::on_actionAddRectangle_triggered()
{
    addShape();
    updateActions();
}

void frmMainWnd::on_actionAddTriangle_triggered()
{
    addShape();
    updateActions();
}

void frmMainWnd::on_actionAddLine_triggered()
{
    addShape();
    updateActions();
}

void frmMainWnd::removeShape()
{
    Document *doc = currentDocument();
    if (doc == 0)
        return;

    QString shapeName = doc->currentShapeName();
    if (shapeName.isEmpty())
        return;

    doc->undoStack()->push(new RemoveShapeCommand(doc, shapeName));
}

void frmMainWnd::setShapeColor()
{
    Document *doc = currentDocument();
    if (doc == 0)
        return;

    QString shapeName = doc->currentShapeName();
    if (shapeName.isEmpty())
        return;

    if (shapeColor == doc->shape(shapeName).color())
        return;

    doc->undoStack()->push(new SetShapeColorCommand(doc, shapeName, shapeColor, spinweight->value(), typecombo->currentIndex() + 1,
                          text->text(), QFont(fontcombo->currentText(),fontsizecombo->currentText().toInt(),60)));
}

void frmMainWnd::on_actionRemoveShape_triggered()
{
    removeShape();
}

void frmMainWnd::on_actionSave_triggered()
{
    saveDocument();
}

void frmMainWnd::on_actionOpen_triggered()
{
    openDocument();
}

void frmMainWnd::on_SlideViewAction_triggered(bool checked)
{
   ui->SlideDock->setVisible(checked);
}

void frmMainWnd::on_actionColor_triggered()
{
    shapeColor = QColorDialog::getColor(QColor("red"),this,"Select Color",QColorDialog::ShowAlphaChannel);
    Document *doc = currentDocument();
    if (doc == 0)
        return;

    QString shapeName = doc->currentShapeName();
    if (shapeName.isEmpty())
        return;

    if (shapeColor == doc->shape(shapeName).color())
        return;

    fontcombo->setCurrentText(doc->shape(shapeName).shapefont().family());
    fontsizecombo->setCurrentText(QString::number(doc->shape(shapeName).shapefont().pointSize()));
    doc->undoStack()->push(new SetShapeColorCommand(doc, shapeName, shapeColor,spinweight->value(),typecombo->currentIndex() + 1,
                                                    text->text(),QFont(fontcombo->currentText(),fontsizecombo->currentText().toInt(),60)));
}

void frmMainWnd::setShapeWidth(int w)
{
    Document *doc = currentDocument();
    if (doc == 0)
        return;

    QString shapeName = doc->currentShapeName();
    if (shapeName.isEmpty())
        return;

    shapeColor = doc->shape(shapeName).color();
    fontcombo->setCurrentText(doc->shape(shapeName).shapefont().family());
    fontsizecombo->setCurrentText(QString::number(doc->shape(shapeName).shapefont().pointSize()));
    doc->undoStack()->push(new SetShapeColorCommand(doc, shapeName, shapeColor, w, typecombo->currentIndex() + 1, text->text(),
                                                    QFont(fontcombo->currentText(),fontsizecombo->currentText().toInt(),60)));
}

void frmMainWnd::setShapeType(int type)
{
    Document *doc = currentDocument();
    if (doc == 0)
        return;

    QString shapeName = doc->currentShapeName();
    if (shapeName.isEmpty())
        return;

    shapeColor = doc->shape(shapeName).color();
    fontcombo->setCurrentText(doc->shape(shapeName).shapefont().family());
    fontsizecombo->setCurrentText(QString::number(doc->shape(shapeName).shapefont().pointSize()));
    doc->undoStack()->push(new SetShapeColorCommand(doc, shapeName, shapeColor, spinweight->value(), type + 1, text->text(),
                                                    QFont(fontcombo->currentText(),fontsizecombo->currentText().toInt(),60)));
}

void frmMainWnd::on_actionExportImage_triggered()
{
    exportImage();
}

void frmMainWnd::on_actionText_triggered()
{
    Document *doc = currentDocument();
    if (doc == 0)
        return;

    QString shapeName = doc->currentShapeName();
    if (shapeName.isEmpty())
        return;

    ui->toolBar->addWidget(textlabel);
    ui->toolBar->addWidget(text);
    ui->toolBar->addWidget(fontlabel);
    ui->toolBar->addWidget(fontcombo);
    ui->toolBar->addWidget(fontsizecombo);
}

void frmMainWnd::addText(QString text)
{
    Document *doc = currentDocument();

    if (doc == 0)
        return;

    QString shapeName = doc->currentShapeName();
    if (shapeName.isEmpty())
        return;

    fontcombo->setCurrentText(doc->shape(shapeName).shapefont().family());
    fontsizecombo->setCurrentText(QString::number(doc->shape(shapeName).shapefont().pointSize()));
    doc->undoStack()->push(new SetShapeColorCommand(doc, shapeName, doc->shape(shapeName).color(),spinweight->value(),
                                                    typecombo->currentIndex() + 1, text,
                                                    QFont(fontcombo->currentText(),fontsizecombo->currentText().toInt(),60)));
}

void frmMainWnd::setFontfamily(QString family)
{
    Document *doc = currentDocument();
    if (doc == 0)
        return;

    QString shapeName = doc->currentShapeName();
    if (shapeName.isEmpty())
        return;

    fontcombo->setCurrentText(doc->shape(shapeName).shapefont().family());
    fontsizecombo->setCurrentText(QString::number(doc->shape(shapeName).shapefont().pointSize()));
    doc->undoStack()->push(new SetShapeColorCommand(doc, shapeName, doc->shape(shapeName).color(),spinweight->value(),
                                                    typecombo->currentIndex() + 1, text->text(),
                                                    QFont(family,fontsizecombo->currentText().toInt(),60)));
}

void frmMainWnd::setFontsize(QString size)
{
    Document *doc = currentDocument();
    if (doc == 0)
        return;

    QString shapeName = doc->currentShapeName();
    if (shapeName.isEmpty())
        return;

    fontcombo->setCurrentText(doc->shape(shapeName).shapefont().family());
    fontsizecombo->setCurrentText(QString::number(doc->shape(shapeName).shapefont().pointSize()));
    doc->undoStack()->push(new SetShapeColorCommand(doc, shapeName, doc->shape(shapeName).color(),spinweight->value(),
                                                    typecombo->currentIndex() + 1, text->text(),
                                                    QFont(fontcombo->currentText(),size.toInt(),60)));
}

void frmMainWnd::currentShapeText(QString shapeText)
{
    text->setText(shapeText);
}

void frmMainWnd::currentShapeColorChanged(const QColor &color)
{
    shapeColor = color;
}

void frmMainWnd::currentShapePenstyleChanged(int penstype)
{
    typecombo->setCurrentIndex(penstype);
}

void frmMainWnd::currentShapePenweightChanged(int penweight)
{
    spinweight->setValue(penweight);
}

void frmMainWnd::currentShapeFontfamilyChanged(const QString &fontfamily)
{
    fontcombo->setCurrentText(fontfamily);
}

void frmMainWnd::currentShapeFontsizeChanged(int fontsize)
{
    fontsizecombo->setCurrentText(QString::number(fontsize));
}
