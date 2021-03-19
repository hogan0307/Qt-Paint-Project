#ifndef FRMMAINWND_H
#define FRMMAINWND_H

#include "document.h"
#include "commands.h"

#include <QMainWindow>
#include <QFontComboBox>
#include <QtWidgets>
#include <QUndoGroup>

QT_BEGIN_NAMESPACE
namespace Ui { class frmMainWnd; }
QT_END_NAMESPACE

class frmMainWnd : public QMainWindow
{
    Q_OBJECT

public:
    frmMainWnd(QWidget *parent = nullptr);
    ~frmMainWnd();

    void addDocument(Document *doc);
    void setCurrentDocument(Document *doc);
    Document *currentDocument() const;
    void removeDocument(Document *doc);

private:
    Ui::frmMainWnd *ui;
    void SlideFormat(int row);
    QLabel* fontlabel;
    QLabel* textlabel;
    QLineEdit* text;
    QFontComboBox *fontcombo;
    QComboBox *fontsizecombo;
    QLabel* weightlabel;
    QSpinBox *spinweight;
    QLabel* typelabel;
    QComboBox* typecombo;

    QUndoGroup *m_undoGroup;
    QColor shapeColor;
    const QString parsingcode = "SlideEnd";


private slots:
    void NewSlide();
    void newDocument();
    void updateActions();
    void closeDocument();
    void saveDocument();
    void openDocument();
    void exportImage();
    //when changed controls as color, penstyle, penweight, fontfamily, fontsize as so on.
    void addShape();
    void removeShape();
    void setShapeColor();
    void setShapeWidth(int);
    void setShapeType(int);
    void addText(QString);
    void setFontfamily(QString);
    void setFontsize(QString);
    //when selected the shape
    void currentShapeText(QString shapeText);
    void currentShapeColorChanged(const QColor &color);
    void currentShapePenstyleChanged(int penstype);
    void currentShapePenweightChanged(int penweight);
    void currentShapeFontfamilyChanged(const QString &fontfamily);
    void currentShapeFontsizeChanged(int fontsize);

    void on_NewSlideAction_triggered();
    void on_RemoveSlideAction_triggered();
    void on_actionClose_triggered();
    void on_tbl_Slide_cellClicked(int row, int column);
    void on_actionAddCircle_triggered();
    void on_actionAddRectangle_triggered();
    void on_actionAddTriangle_triggered();
    void on_actionAddLine_triggered();
    void on_actionRemoveShape_triggered();
    void on_actionSave_triggered();
    void on_actionOpen_triggered();
    void on_SlideViewAction_triggered(bool checked);
    void on_actionColor_triggered();
    void on_actionExportImage_triggered();
    void on_actionText_triggered();
};
#endif // FRMMAINWND_H
