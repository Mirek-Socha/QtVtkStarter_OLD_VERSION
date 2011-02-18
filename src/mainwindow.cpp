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

    // Zestawienie połączeń VTK->Gt:
    vtkSmartPointer<vtkEventQtSlotConnect> connections =
            vtkSmartPointer<vtkEventQtSlotConnect>::New();

    // przechwycenie zdarzeń vtk:
    connections->Connect(ui->qvtkWidget->GetInteractor(),
                            vtkCommand::EndEvent,
                            this,
                            SLOT(updateCoords()));  // reakcja na kliknięcie klawisza

    // debug poprawności VTK->Qt połączeń na konsolę:
    connections->PrintSelf(cout, vtkIndent());

}

// slot obsługuje StatusBar - wyświetlanie wartości INT
void MainWindow::showOnStatusBar(int i)
{
    // wystawienie wartości INT na StatusBar
    ui->statusBar->showMessage(QString::fromUtf8("Wartość: %1").arg(i));
}

// przykład pobrania wsp. kamery
void MainWindow::updateCoords()
{
    double camPosition[3];
    ren->GetActiveCamera()->GetPosition(camPosition);

    QString str = QString("x=%1 : y=%2 : z=%3")
                  .arg(camPosition[0]).arg( camPosition[1]).arg(camPosition[2]);

    qDebug() << str;

    ui->statusBar->showMessage(str);
}

// obsługa wyświetlania postępu obliczeń obiektu VTK
void MainWindow::updateProgressValue( vtkObject *caller,
                                      unsigned long vtkEvent,
                                      void* client_data,
                                      void* call_data)
{

        if( vtkEvent == vtkCommand::ProgressEvent )     // weryfikacja zdarzenia
        {
                double v = *((double*) call_data);  // rzutowanie danych
                int value = 100*v;
                emit newProgress( value );  // powiadomienie o postępach w %
        }
        QCoreApplication::processEvents();  // "odblokowanie" kolejki zdarzeń w GUI
}

// zmiana parametrów obiektu VTK
void MainWindow::setResolution(int res)
{
    if (source!=NULL)
    {
        source->SetResolution(res);
        ui->qvtkWidget->repaint();
    }
}

// zbudowanie strumienia wizualziacji - miejsce na kod VTK
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

// destruktor okna
MainWindow::~MainWindow()
{
    ren->Delete();
    delete ui;
}

// metoda wygenerowana automatycznie przez kreator
void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
