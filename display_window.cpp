#include "display_window.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
#include <fstream>
#include <QGroupBox>
#include <QRadioButton>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include "graphicsview_zoom.h"


StructureInteractionWindow::StructureInteractionWindow(QWidget *parent)
        : QWidget(parent), sparseVector(nullptr), sparseMatrix(nullptr) {
    setupUi();
}

StructureInteractionWindow::~StructureInteractionWindow() {
    delete sparseVector;
    delete sparseMatrix;
}

void StructureInteractionWindow::setupUi() {
    mainLayout = new QVBoxLayout(this);

    structureGroupBox = new QGroupBox("Select Structure", this);
    QVBoxLayout *structureLayout = new QVBoxLayout(structureGroupBox);
    vectorRadioButton = new QRadioButton("SparseVector", structureGroupBox);
    matrixRadioButton = new QRadioButton("SparseMatrix", structureGroupBox);
    structureLayout->addWidget(vectorRadioButton);
    structureLayout->addWidget(matrixRadioButton);
    vectorRadioButton->setChecked(true);
    connect(vectorRadioButton, &QRadioButton::toggled, this, &StructureInteractionWindow::structureSelected);

    dictionaryGroupBox = new QGroupBox("Select Dictionary", this);
    QVBoxLayout *dictionaryLayout = new QVBoxLayout(dictionaryGroupBox);
    hashTableRadioButton = new QRadioButton("HashTable", dictionaryGroupBox);
    bTreeRadioButton = new QRadioButton("BTree", dictionaryGroupBox);
    dictionaryLayout->addWidget(hashTableRadioButton);
    dictionaryLayout->addWidget(bTreeRadioButton);
    hashTableRadioButton->setChecked(true);
    connect(hashTableRadioButton, &QRadioButton::toggled, this, &StructureInteractionWindow::dictionarySelected);

    interactionWidget = new QWidget(this);
    interactionLayout = new QVBoxLayout(interactionWidget);

    setupVectorUi();

    mainLayout->addWidget(structureGroupBox);
    mainLayout->addWidget(dictionaryGroupBox);
    mainLayout->addWidget(interactionWidget);
    setLayout(mainLayout);
}

void StructureInteractionWindow::structureSelected() {
    delete sparseVector;
    sparseVector = nullptr;
    delete sparseMatrix;
    sparseMatrix = nullptr;

    QLayoutItem *child;
    while ((child = interactionLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    if (vectorRadioButton->isChecked()) {
        setupVectorUi();
    } else {
        setupMatrixUi();
    }
}

void StructureInteractionWindow::dictionarySelected() {
    structureSelected();
}

void StructureInteractionWindow::setupVectorUi() {
    int length = 30;
    if (hashTableRadioButton->isChecked()) {
        UnqPtr<IDictionary<int, double>> dict(new HashTable<int, double>());
        sparseVector = new SparseVector<double>(length, std::move(dict));
    } else {
        UnqPtr<IDictionary<int, double>> dict(new BTree<int, double>());
        sparseVector = new SparseVector<double>(length, std::move(dict));
    }

    QLabel *indexLabel = new QLabel("Index (0-29):");
    vectorIndexSpinBox = new QSpinBox();
    vectorIndexSpinBox->setRange(0, 29);

    QLabel *valueLabel = new QLabel("Value:");
    vectorValueLineEdit = new QLineEdit();

    vectorSetButton = new QPushButton("Set Element");
    vectorGetButton = new QPushButton("Get Element");

    vectorDisplay = new QTextEdit();
    vectorDisplay->setReadOnly(true);

    connect(vectorSetButton, &QPushButton::clicked, this, &StructureInteractionWindow::performOperation);
    connect(vectorGetButton, &QPushButton::clicked, this, &StructureInteractionWindow::performOperation);

    interactionLayout->addWidget(indexLabel);
    interactionLayout->addWidget(vectorIndexSpinBox);
    interactionLayout->addWidget(valueLabel);
    interactionLayout->addWidget(vectorValueLineEdit);
    interactionLayout->addWidget(vectorSetButton);
    interactionLayout->addWidget(vectorGetButton);
    interactionLayout->addWidget(new QLabel("Vector Elements:"));
    interactionLayout->addWidget(vectorDisplay);

    updateDisplay();
}

void StructureInteractionWindow::setupMatrixUi() {
    int rows = 5;
    int cols = 5;
    if (hashTableRadioButton->isChecked()) {
        UnqPtr<IDictionary<IndexPair, double>> dict(new HashTable<IndexPair, double>());
        sparseMatrix = new SparseMatrix<double>(rows, cols, std::move(dict));
    } else {
        UnqPtr<IDictionary<IndexPair, double>> dict(new BTree<IndexPair, double>());
        sparseMatrix = new SparseMatrix<double>(rows, cols, std::move(dict));
    }

    QLabel *rowLabel = new QLabel("Row (0-4):");
    matrixRowSpinBox = new QSpinBox();
    matrixRowSpinBox->setRange(0, 4);

    QLabel *colLabel = new QLabel("Column (0-4):");
    matrixColSpinBox = new QSpinBox();
    matrixColSpinBox->setRange(0, 4);

    QLabel *valueLabel = new QLabel("Value:");
    matrixValueLineEdit = new QLineEdit();

    matrixSetButton = new QPushButton("Set Element");
    matrixGetButton = new QPushButton("Get Element");

    matrixDisplay = new QTextEdit();
    matrixDisplay->setReadOnly(true);

    connect(matrixSetButton, &QPushButton::clicked, this, &StructureInteractionWindow::performOperation);
    connect(matrixGetButton, &QPushButton::clicked, this, &StructureInteractionWindow::performOperation);

    interactionLayout->addWidget(rowLabel);
    interactionLayout->addWidget(matrixRowSpinBox);
    interactionLayout->addWidget(colLabel);
    interactionLayout->addWidget(matrixColSpinBox);
    interactionLayout->addWidget(valueLabel);
    interactionLayout->addWidget(matrixValueLineEdit);
    interactionLayout->addWidget(matrixSetButton);
    interactionLayout->addWidget(matrixGetButton);
    interactionLayout->addWidget(new QLabel("Matrix Elements:"));
    interactionLayout->addWidget(matrixDisplay);

    updateDisplay();
}

void StructureInteractionWindow::performOperation() {
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (!button)
        return;

    if (sparseVector) {
        int index = vectorIndexSpinBox->value();
        if (button == vectorSetButton) {
            bool ok;
            double value = vectorValueLineEdit->text().toDouble(&ok);
            if (!ok) {
                QMessageBox::warning(this, "Invalid Input", "Please enter a valid number.", QMessageBox::Ok);
                return;
            }
            sparseVector->SetElement(index, value);
            updateDisplay();
        } else if (button == vectorGetButton) {
            try {
                double value = sparseVector->GetElement(index);
                QMessageBox::information(this, "Element Value", 
                    QString("Value at index %1: %2").arg(index).arg(value), QMessageBox::Ok);
            } catch (const std::exception &e) {
                QMessageBox::warning(this, "Error", e.what(), QMessageBox::Ok);
            }
        }
    } else if (sparseMatrix) {
        int row = matrixRowSpinBox->value();
        int col = matrixColSpinBox->value();
        if (button == matrixSetButton) {
            bool ok;
            double value = matrixValueLineEdit->text().toDouble(&ok);
            if (!ok) {
                QMessageBox::warning(this, "Invalid Input", "Please enter a valid number.", QMessageBox::Ok);
                return;
            }
            sparseMatrix->SetElement(row, col, value);
            updateDisplay();
        } else if (button == matrixGetButton) {
            try {
                double value = sparseMatrix->GetElement(row, col);
                QMessageBox::information(this, "Element Value", 
                    QString("Value at (%1,%2): %3").arg(row).arg(col).arg(value), QMessageBox::Ok);
            } catch (const std::exception &e) {
                QMessageBox::warning(this, "Error", e.what(), QMessageBox::Ok);
            }
        }
    }
}

void StructureInteractionWindow::updateDisplay() {
    if (sparseVector) {
        QString displayText;
        for (int i = 0; i < sparseVector->GetLength(); ++i) {
            double value = sparseVector->GetElement(i);
            displayText += QString("Index %1: %2\n").arg(i).arg(value);
        }
        vectorDisplay->setText(displayText);
    } else if (sparseMatrix) {
        QString displayText;
        for (int i = 0; i < sparseMatrix->GetRows(); ++i) {
            QString rowText;
            for (int j = 0; j < sparseMatrix->GetColumns(); ++j) {
                double value = sparseMatrix->GetElement(i, j);
                rowText += QString("%1\t").arg(value);
            }
            displayText += rowText + "\n";
        }
        matrixDisplay->setText(displayText);
    }
}

DisplayWindow::DisplayWindow(QWidget *parent)
        : QMainWindow(parent), structureInteractionWindow(nullptr), btreeTab(nullptr), btreeTest(nullptr) {
    setupUi();
    loadCsvData();
}

DisplayWindow::~DisplayWindow() {
    delete structureInteractionWindow;
    delete btreeTest;
}

void DisplayWindow::setupUi() {
    tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);

    setupTestingTab();
    setupGraphTab();
    setupBTreeTab();
}

void DisplayWindow::setupTestingTab() {
    testingTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(testingTab);

    runTestsButton = new QPushButton("Run Tests", testingTab);
    connect(runTestsButton, &QPushButton::clicked, this, &DisplayWindow::runTests);

    structureInteractionButton = new QPushButton("Structure Interaction", testingTab);
    connect(structureInteractionButton, &QPushButton::clicked, this, &DisplayWindow::openStructureInteraction);

    testOutput = new QTextEdit(testingTab);
    testOutput->setReadOnly(true);

    layout->addWidget(runTestsButton);
    layout->addWidget(structureInteractionButton);
    layout->addWidget(new QLabel("Test Output:"));
    layout->addWidget(testOutput);

    testingTab->setLayout(layout);
    tabWidget->addTab(testingTab, "Testing");
}

void DisplayWindow::setupGraphTab() {
    graphTab = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(graphTab);

    QGroupBox *graphGroupBox = new QGroupBox("Performance Graph", graphTab);
    QVBoxLayout *graphLayout = new QVBoxLayout(graphGroupBox);

    QHBoxLayout *radioLayout = new QHBoxLayout();
    radioVector = new QRadioButton("Vector", graphGroupBox);
    radioMatrix = new QRadioButton("Matrix", graphGroupBox);
    radioVector->setChecked(true);
    radioLayout->addWidget(radioVector);
    radioLayout->addWidget(radioMatrix);

    connect(radioVector, &QRadioButton::toggled, this, &DisplayWindow::onGraphStructureChanged);

    metricSelector = new QComboBox(graphGroupBox);
    chartView = new QtCharts::QChartView(graphGroupBox);

    connect(metricSelector, &QComboBox::currentTextChanged, this, &DisplayWindow::updateGraph);

    graphLayout->addLayout(radioLayout);
    graphLayout->addWidget(new QLabel("Select Metric:"));
    graphLayout->addWidget(metricSelector);
    graphLayout->addWidget(chartView);

    graphGroupBox->setLayout(graphLayout);
    mainLayout->addWidget(graphGroupBox);

    graphTab->setLayout(mainLayout);
    tabWidget->addTab(graphTab, "Graphs");
}

void DisplayWindow::onGraphStructureChanged() {
    metricSelector->clear();

    QStringList metrics;
    for (int i = 4; i < csvHeaders.size(); ++i) {
        metrics.append(csvHeaders.at(i));
    }
    metricSelector->addItems(metrics);

    if (!metricSelector->currentText().isEmpty()) {
        updateGraph(metricSelector->currentText());
    }
}

void DisplayWindow::updateGraph(const QString &graphType) {
    if (graphType.isEmpty()) return;

    QString structureType = radioVector->isChecked() ? "Vector" : "Matrix";

    QtCharts::QChart *chart = new QtCharts::QChart();

    int xAxisIndex = 2;
    int yAxisIndex = csvHeaders.indexOf(graphType);
    if (yAxisIndex == -1) {
        qDebug() << "Graph type not found in CSV headers:" << graphType;
        return;
    }

    QMap<QString, QtCharts::QLineSeries*> seriesMap;

    for (const QStringList &row : csvData) {
        if (row.size() <= yAxisIndex) continue;

        QString dictionary = row.at(0);
        QString structure = row.at(1);
        if (structure != structureType) {
            continue; 
        }
        double xValue = row.at(xAxisIndex).toDouble();
        double yValue = row.at(yAxisIndex).toDouble();

        if (!seriesMap.contains(dictionary)) {
            seriesMap[dictionary] = new QtCharts::QLineSeries();
            seriesMap[dictionary]->setName(dictionary);
        }
        seriesMap[dictionary]->append(xValue, yValue);
    }

    for (auto s : seriesMap) {
        chart->addSeries(s);
    }

    chart->setTitle(QString("%1 (%2)").arg(graphType).arg(structureType));

    chart->createDefaultAxes();

    chartView->setChart(chart);
}

void DisplayWindow::setupBTreeTab() {
    btreeTab = new QWidget();
    btreeLayout = new QVBoxLayout(btreeTab);

    btreeTest = new BTreeTest(3);

    btreeControlsLayout = new QHBoxLayout();

    btreeKeyLabel = new QLabel("Key:");
    btreeKeyLineEdit = new QLineEdit();
    btreeValueLabel = new QLabel("Value:");
    btreeValueLineEdit = new QLineEdit();
    btreeAddButton = new QPushButton("Add");
    btreeRemoveButton = new QPushButton("Remove");

    connect(btreeAddButton, &QPushButton::clicked, this, &DisplayWindow::addBTreeElement);
    connect(btreeRemoveButton, &QPushButton::clicked, this, &DisplayWindow::removeBTreeElement);

    btreeControlsLayout->addWidget(btreeKeyLabel);
    btreeControlsLayout->addWidget(btreeKeyLineEdit);
    btreeControlsLayout->addWidget(btreeValueLabel);
    btreeControlsLayout->addWidget(btreeValueLineEdit);
    btreeControlsLayout->addWidget(btreeAddButton);
    btreeControlsLayout->addWidget(btreeRemoveButton);

    btreeFillLayout = new QHBoxLayout();
    btreeFillNumLabel = new QLabel("Number of Elements:");
    btreeFillNumSpinBox = new QSpinBox();
    btreeFillNumSpinBox->setRange(1, 1000);
    btreeFillNumSpinBox->setValue(10);
    btreeFillMinLabel = new QLabel("Key Min:");
    btreeFillMinSpinBox = new QSpinBox();
    btreeFillMinSpinBox->setRange(-100000, 100000);
    btreeFillMinSpinBox->setValue(1);
    btreeFillMaxLabel = new QLabel("Key Max:");
    btreeFillMaxSpinBox = new QSpinBox();
    btreeFillMaxSpinBox->setRange(-100000, 100000);
    btreeFillMaxSpinBox->setValue(100);
    btreeFillButton = new QPushButton("Fill Random");

    connect(btreeFillButton, &QPushButton::clicked, this, &DisplayWindow::fillBTree);

    btreeFillLayout->addWidget(btreeFillNumLabel);
    btreeFillLayout->addWidget(btreeFillNumSpinBox);
    btreeFillLayout->addWidget(btreeFillMinLabel);
    btreeFillLayout->addWidget(btreeFillMinSpinBox);
    btreeFillLayout->addWidget(btreeFillMaxLabel);
    btreeFillLayout->addWidget(btreeFillMaxSpinBox);
    btreeFillLayout->addWidget(btreeFillButton);

    btreeVisualizeButton = new QPushButton("Visualize");
    connect(btreeVisualizeButton, &QPushButton::clicked, this, &DisplayWindow::visualizeBTree);

    btreeGraphicsView = new GraphicsViewZoom();
    btreeScene = new QGraphicsScene(this);
    btreeGraphicsView->setScene(btreeScene);
    btreeGraphicsView->setRenderHint(QPainter::Antialiasing);
    btreeGraphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    btreeGraphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    btreeLayout->addLayout(btreeControlsLayout);
    btreeLayout->addLayout(btreeFillLayout);
    btreeLayout->addWidget(btreeVisualizeButton);
    btreeLayout->addWidget(btreeGraphicsView);

    tabWidget->addTab(btreeTab, "BTree");
}

void DisplayWindow::runTests() {
    testOutput->clear();

    std::ofstream file("test_output.txt");
    if (!file.is_open()) {
        testOutput->append("Failed to open test output file.");
        return;
    }
    auto oldCoutBuffer = std::cout.rdbuf(file.rdbuf());

    run_tests();

    std::cout.rdbuf(oldCoutBuffer);
    file.close();

    QFile qfile("test_output.txt");
    if (!qfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        testOutput->append("Failed to read test output file.");
        return;
    }
    QTextStream in(&qfile);
    testOutput->append(in.readAll());
    qfile.close();

    loadCsvData();
}

void DisplayWindow::openStructureInteraction() {
    if (!structureInteractionWindow) {
        structureInteractionWindow = new StructureInteractionWindow();
        structureInteractionWindow->setWindowTitle("Structure Interaction");
        structureInteractionWindow->resize(400, 600);
    }
    structureInteractionWindow->show();
    structureInteractionWindow->raise();
    structureInteractionWindow->activateWindow();
}

void DisplayWindow::loadCsvData() {
    csvData.clear();
    csvHeaders.clear();

    QFile file("performance_results.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open CSV file.";
        return;
    }

    QTextStream in(&file);
    bool firstLine = true;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(",");
        if (firstLine) {
            csvHeaders = fields;
            firstLine = false;
        } else {
            csvData.append(fields);
        }
    }
    file.close();

    onGraphStructureChanged();
}

void DisplayWindow::addBTreeElement() {
    QString keyStr = btreeKeyLineEdit->text();
    QString value = btreeValueLineEdit->text();

    bool ok;
    int key = keyStr.toInt(&ok);
    if (!ok || value.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a valid key and non-empty value.", QMessageBox::Ok);
        return;
    }

    btreeTest->AddElement(key, value.toStdString());

    btreeTest->Visualize("btree.png");
    QString imagePath = "btree.png";
    QPixmap pixmap(imagePath);
    if (pixmap.isNull()) {
        QMessageBox::information(this, "Image Not Found", "The tree image is not available yet.", QMessageBox::Ok);
    } else {
        btreeScene->clear();
        btreePixmapItem = btreeScene->addPixmap(pixmap);
        btreeGraphicsView->fitInView(btreePixmapItem, Qt::KeepAspectRatio);
    }
}

void DisplayWindow::removeBTreeElement() {
    QString keyStr = btreeKeyLineEdit->text();

    bool ok;
    int key = keyStr.toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a valid key.", QMessageBox::Ok);
        return;
    }

    btreeTest->RemoveElement(key);

    btreeTest->Visualize("btree.png");
    QString imagePath = "btree.png";
    QPixmap pixmap(imagePath);
    if (pixmap.isNull()) {
        QMessageBox::information(this, "Image Not Found", "The tree image is not available yet.", QMessageBox::Ok);
    } else {
        btreeScene->clear();
        btreePixmapItem = btreeScene->addPixmap(pixmap);
        btreeGraphicsView->fitInView(btreePixmapItem, Qt::KeepAspectRatio);
    }
}

void DisplayWindow::fillBTree() {
    int numElements = btreeFillNumSpinBox->value();
    int keyMin = btreeFillMinSpinBox->value();
    int keyMax = btreeFillMaxSpinBox->value();

    if (keyMin > keyMax) {
        QMessageBox::warning(this, "Invalid Range", "Key Min should be less than or equal to Key Max.", QMessageBox::Ok);
        return;
    }

    delete btreeTest;
    btreeTest = new BTreeTest(3);

    btreeTest->FillRandom(numElements, keyMin, keyMax);

    btreeTest->Visualize("btree.png");
    QString imagePath = "btree.png";
    QPixmap pixmap(imagePath);
    if (pixmap.isNull()) {
        QMessageBox::information(this, "Image Not Found", "The tree image is not available yet.", QMessageBox::Ok);
    } else {
        btreeScene->clear();
        btreePixmapItem = btreeScene->addPixmap(pixmap);
        btreeGraphicsView->fitInView(btreePixmapItem, Qt::KeepAspectRatio);
    }
}

void DisplayWindow::visualizeBTree() {
    btreeTest->Visualize("btree.png");
    QString imagePath = "btree.png";
    QPixmap pixmap(imagePath);
    if (pixmap.isNull()) {
        QMessageBox::information(this, "Image Not Found", "The tree image is not available yet.", QMessageBox::Ok);
    } else {
        btreeScene->clear();
        btreePixmapItem = btreeScene->addPixmap(pixmap);
        btreeGraphicsView->fitInView(btreePixmapItem, Qt::KeepAspectRatio);
    }
}
