#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

////////////////////////////////////////////////////////////////
// Obsługa VTK

    // utworzenie renderera
    ren = vtkRenderer::New();
    // podłączenie renderera do wigetu:
    ui->qvtkWidget->GetRenderWindow()->AddRenderer(ren);
    renWin = ui->qvtkWidget->GetRenderWindow();
    renWin->StereoCapableWindowOn();

    // podmiana interaktora
//    iren = ui->qvtkWidget->GetInteractor();
//    vtkInteractorStyleUnicam *unicamStyl = vtkInteractorStyleUnicam::New();
//    iren->SetInteractorStyle(unicamStyl);

    // przykładowy strumień VTK
    newMesh();

////////////////////////////////////////////////////////////////
// Zestawienie połączeń SIGNAL->SLOT interfejsu Qt
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)),
            this, SLOT(showOnStatusBar(int)) );
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)),
            this, SLOT(setResolution(int)) );
    connect(this, SIGNAL(setStatusText(QString)),
            ui->statusBar, SLOT(showMessage(QString)) );

//    connect(ui->qvtkWidget, SIGNAL(mouseEvent(QMouseEvent*)),   //sygnał z widgetu, nie z VTK!
//            this, SLOT(updateCoords()));


    // Zestawienie połączeń VTK->Gt:
    vtkSmartPointer<vtkEventQtSlotConnect> connections =
            vtkSmartPointer<vtkEventQtSlotConnect>::New();

    // przechwycenie zdarzeń vtk:
    connections->Connect(ui->qvtkWidget->GetInteractor(),
                            vtkCommand::LeftButtonPressEvent,
                            this,
                            SLOT(updateCoords()));
    //przykład podpięcia się pod postępy w obliczeniach, np. pod wczytywanie pliku:
//    connections->Connect( XXXReader, vtkCommand::ProgressEvent,
//                          this, SLOT( updateProgressValue( vtkObject*, unsigned long, void*, void*)));

    // debug poprawności połączeń na konsolę:
    connections->PrintSelf(cout, vtkIndent());

}

void MainWindow::showOnStatusBar(int i)
{
    // wystawienie wartości INT na StatusBar
    ui->statusBar->showMessage(QString::fromUtf8("Wartość: %1").arg(i));
}

// wyświetlenie wsp. kamery
void MainWindow::updateCoords()
{
    double camPosition[3];
    ren->GetActiveCamera()->GetPosition(camPosition);

    QString str = QString("x=%1 : y=%2 : z=%3")
                  .arg(camPosition[0]).arg( camPosition[1]).arg(camPosition[2]);

    qDebug() << str;

    ui->statusBar->showMessage(str);
}

// komunikacja VTK -> QT
void MainWindow::updateProgressValue( vtkObject *caller,
                                      unsigned long vtkEvent,
                                      void* client_data,
                                      void* call_data)
{

        if( vtkEvent == vtkCommand::ProgressEvent )     // weryfikacja zdarzenia
        {
                double v = *((double*) call_data);
                int value = 100*v;
                emit newProgress( value );  // powiadomienie o postępach w %
        }
        QCoreApplication::processEvents();  // "odblokowanie" kolejki zdarzeń w GUI
}

// zmiana parametrów obiektu VTK
void MainWindow::setResolution(int res)
{
    source->SetResolution(res);     // zmiana parametru źródła
    ui->qvtkWidget->repaint();      // wymuszenie narysowania widgetu
}

// przygotwanie strumienia VTK
void MainWindow::newMesh()
{
    // Geometry
    source = vtkConeSource::New();
    source->SetRadius(3);
    source->SetHeight(5);
    source->SetResolution(5);

    // Mapper
    mapper = vtkPolyDataMapper::New();
    mapper->ImmediateModeRenderingOn();
    mapper->SetInputConnection(source->GetOutputPort());

    // Actor in scene
    actor = vtkActor::New();
    actor->SetMapper(mapper);

    // Add Actor to renderer
    ren->AddActor(actor);

    // Reset camera
    ren->ResetCamera();

    ren->GetRenderWindow()->Render();

    // Cleaning
    actor->Delete();
    mapper->Delete();
    source->Delete();
}

MainWindow::~MainWindow()
{
    ren->Delete();

    delete ui;
}

