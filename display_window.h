#ifndef DISPLAY_WINDOW_H
#define DISPLAY_WINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QTabWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QString>
#include <QVector>
#include <QStringList>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QGroupBox>
#include <QRadioButton>
#include <QSpinBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

#include "test.h"
#include "DataStructures/SparseMatrix.h"
#include "DataStructures/SparseVector.h"
#include "DataStructures/BTree.h"
#include "DataStructures/HashTable.h"
#include "test_btree.h"
#include "graphicsview_zoom.h"

class StructureInteractionWindow : public QWidget {
    Q_OBJECT

public:
    explicit StructureInteractionWindow(QWidget *parent = nullptr);
    ~StructureInteractionWindow();

private slots:
    void structureSelected();
    void dictionarySelected();
    void performOperation();
    void updateDisplay();

private:
    void setupUi();
    void setupVectorUi();
    void setupMatrixUi();

    QVBoxLayout *mainLayout;

    QGroupBox *structureGroupBox;
    QRadioButton *vectorRadioButton;
    QRadioButton *matrixRadioButton;

    QGroupBox *dictionaryGroupBox;
    QRadioButton *hashTableRadioButton;
    QRadioButton *bTreeRadioButton;

    QWidget *interactionWidget;
    QVBoxLayout *interactionLayout;

    QSpinBox *vectorIndexSpinBox;
    QLineEdit *vectorValueLineEdit;
    QPushButton *vectorSetButton;
    QPushButton *vectorGetButton;
    QTextEdit *vectorDisplay;

    QSpinBox *matrixRowSpinBox;
    QSpinBox *matrixColSpinBox;
    QLineEdit *matrixValueLineEdit;
    QPushButton *matrixSetButton;
    QPushButton *matrixGetButton;
    QTextEdit *matrixDisplay;

    SparseVector<double> *sparseVector;
    SparseMatrix<double> *sparseMatrix;
};

class DisplayWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit DisplayWindow(QWidget *parent = nullptr);
    ~DisplayWindow();

private slots:
    void runTests();
    void openStructureInteraction();

    void onGraphStructureChanged();               
    void updateGraph(const QString &graphType);   

    void addBTreeElement();
    void removeBTreeElement();
    void fillBTree();
    void visualizeBTree();

private:
    void setupUi();
    void setupTestingTab();
    void setupGraphTab();
    void setupBTreeTab();

    void loadCsvData();

    QTabWidget *tabWidget;

    QWidget *testingTab;
    QTextEdit *testOutput;
    QPushButton *runTestsButton;
    QPushButton *structureInteractionButton;

    StructureInteractionWindow *structureInteractionWindow;

    QWidget *graphTab;
    QRadioButton *radioVector;
    QRadioButton *radioMatrix;
    QComboBox *metricSelector;
    QtCharts::QChartView *chartView;

    QWidget *btreeTab;
    QVBoxLayout *btreeLayout;

    QHBoxLayout *btreeControlsLayout;
    QLabel *btreeKeyLabel;
    QLineEdit *btreeKeyLineEdit;
    QLabel *btreeValueLabel;
    QLineEdit *btreeValueLineEdit;
    QPushButton *btreeAddButton;
    QPushButton *btreeRemoveButton;

    QHBoxLayout *btreeFillLayout;
    QLabel *btreeFillNumLabel;
    QSpinBox *btreeFillNumSpinBox;
    QLabel *btreeFillMinLabel;
    QSpinBox *btreeFillMinSpinBox;
    QLabel *btreeFillMaxLabel;
    QSpinBox *btreeFillMaxSpinBox;
    QPushButton *btreeFillButton;

    QPushButton *btreeVisualizeButton;
    QLabel *btreeImageLabel;
    QGraphicsView *btreeGraphicsView;
    QGraphicsScene *btreeScene;
    QGraphicsPixmapItem *btreePixmapItem;

    BTreeTest *btreeTest;

    QVector<QStringList> csvData;
    QStringList csvHeaders;
};

#endif // DISPLAY_WINDOW_H
