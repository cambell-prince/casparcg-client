#include "LibraryWidget.h"

#include "Global.h"

#include "DatabaseManager.h"
#include "Events/AddRudnownItemEvent.h"
#include "Events/DataChangedEvent.h"
#include "Events/LibraryItemSelectedEvent.h"
#include "Events/MediaChangedEvent.h"
#include "Events/StatusbarEvent.h"
#include "Events/TemplateChangedEvent.h"
#include "Models/LibraryModel.h"

#include <QtCore/QPoint>
#include <QtCore/QSharedDataPointer>

#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QColor>
#include <QtGui/QIcon>
#include <QtGui/QTreeWidgetItem>

LibraryWidget::LibraryWidget(QWidget* parent)
    : QWidget(parent)
{
    setupUi(this);
    setupUiMenu();

    this->treeWidgetAudio->setColumnWidth(0, 5);
    this->treeWidgetAudio->setColumnHidden(0, true);
    this->treeWidgetAudio->setColumnHidden(2, true);
    this->treeWidgetAudio->setColumnHidden(3, true);
    this->treeWidgetAudio->setColumnHidden(4, true);
    this->treeWidgetAudio->setColumnHidden(5, true);

    this->treeWidgetImage->setColumnWidth(0, 5);
    this->treeWidgetImage->setColumnHidden(0, true);
    this->treeWidgetImage->setColumnHidden(2, true);
    this->treeWidgetImage->setColumnHidden(3, true);
    this->treeWidgetImage->setColumnHidden(4, true);
    this->treeWidgetImage->setColumnHidden(5, true);

    this->treeWidgetTemplate->setColumnWidth(0, 5);
    this->treeWidgetTemplate->setColumnHidden(0, true);
    this->treeWidgetTemplate->setColumnHidden(2, true);
    this->treeWidgetTemplate->setColumnHidden(3, true);
    this->treeWidgetTemplate->setColumnHidden(4, true);
    this->treeWidgetTemplate->setColumnHidden(5, true);

    this->treeWidgetVideo->setColumnWidth(0, 5);
    this->treeWidgetVideo->setColumnHidden(0, true);
    this->treeWidgetVideo->setColumnHidden(2, true);
    this->treeWidgetVideo->setColumnHidden(3, true);
    this->treeWidgetVideo->setColumnHidden(4, true);
    this->treeWidgetVideo->setColumnHidden(5, true);

    this->treeWidgetStoredData->setColumnWidth(0, 5);
    this->treeWidgetStoredData->setColumnHidden(0, true);
    this->treeWidgetStoredData->setColumnHidden(2, true);
    this->treeWidgetStoredData->setColumnHidden(3, true);
    this->treeWidgetStoredData->setColumnHidden(4, true);
    this->treeWidgetStoredData->setColumnHidden(5, true);

    this->toolBoxLibrary->setItemIcon(0, QIcon(":/Graphics/Images/AudioColor.png"));
    this->toolBoxLibrary->setItemIcon(1, QIcon(":/Graphics/Images/StillColor.png"));
    this->toolBoxLibrary->setItemIcon(2, QIcon(":/Graphics/Images/TemplateColor.png"));
    this->toolBoxLibrary->setItemIcon(3, QIcon(":/Graphics/Images/MovieColor.png"));
    this->toolBoxLibrary->setItemIcon(4, QIcon(":/Graphics/Images/StoredDataColor.png"));

    QObject::connect(this->treeWidgetAudio, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(customContextMenuRequested(const QPoint &)));
    QObject::connect(this->treeWidgetTemplate, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(customContextMenuRequested(const QPoint &)));
    QObject::connect(this->treeWidgetVideo, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(customContextMenuRequested(const QPoint &)));
    QObject::connect(this->treeWidgetImage, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(customContextMenuImageRequested(const QPoint &)));

    qApp->installEventFilter(this);

    qApp->postEvent(qApp, new MediaChangedEvent());
    qApp->postEvent(qApp, new TemplateChangedEvent());
    qApp->postEvent(qApp, new DataChangedEvent());
}

void LibraryWidget::setupUiMenu()
{
    this->contextMenu = new QMenu(this);
    this->contextMenu->addAction("Add to rundown");

    this->contextMenuImage = new QMenu(this);
    this->contextMenuImage->addAction("Add to rundown as image");
    this->contextMenuImage->addAction("Add to rundown as image scroller");

    QObject::connect(this->contextMenu, SIGNAL(triggered(QAction*)), this, SLOT(contextMenuTriggered(QAction*)));
    QObject::connect(this->contextMenuImage, SIGNAL(triggered(QAction*)), this, SLOT(contextMenuImageTriggered(QAction*)));
}

bool LibraryWidget::eventFilter(QObject* target, QEvent* event)
{
    if (event->type() == static_cast<QEvent::Type>(Enum::EventType::MediaChanged))
    {
        MediaChangedEvent* mediaChangedEvent = dynamic_cast<MediaChangedEvent*>(event);

        // TODO: Only add / remove necessary items.
        this->treeWidgetAudio->clear();
        this->treeWidgetImage->clear();
        this->treeWidgetVideo->clear();
        this->treeWidgetAudio->clearSelection();
        this->treeWidgetImage->clearSelection();
        this->treeWidgetVideo->clearSelection();

        QList<LibraryModel> models;
        if (this->lineEditFilter->text().isEmpty())
            models = DatabaseManager::getInstance().getLibraryMedia();
        else
            models = DatabaseManager::getInstance().getLibraryMediaByFilter(this->lineEditFilter->text());

        if (models.count() > 0)
        {
            foreach (LibraryModel model, models)
            {
                if (model.getType() == "AUDIO")
                {
                    QTreeWidgetItem* widget = new QTreeWidgetItem(this->treeWidgetAudio);
                    widget->setBackgroundColor(0, QColor(Color::DEFAULT_AUDIO_COLOR));
                    widget->setIcon(1, QIcon(":/Graphics/Images/Audio.png"));
                    widget->setText(1, model.getName());
                    widget->setText(2, "-1");
                    widget->setText(3, model.getLabel());
                    widget->setText(4, model.getDeviceName());
                    widget->setText(5, model.getType());
                }    
                else if (model.getType() == "STILL")
                {
                    QTreeWidgetItem* widget = new QTreeWidgetItem(this->treeWidgetImage);
                    widget->setBackgroundColor(0, QColor(Color::DEFAULT_STILL_COLOR));
                    widget->setIcon(1, QIcon(":/Graphics/Images/Still.png"));
                    widget->setText(1, model.getName());
                    widget->setText(2, "-1");
                    widget->setText(3, model.getLabel());
                    widget->setText(4, model.getDeviceName());
                    widget->setText(5, model.getType());
                }
                else if (model.getType() == "MOVIE")
                {
                    QTreeWidgetItem* widget = new QTreeWidgetItem(this->treeWidgetVideo);
                    widget->setBackgroundColor(0, QColor(Color::DEFAULT_MOVIE_COLOR));
                    widget->setIcon(1, QIcon(":/Graphics/Images/Movie.png"));
                    widget->setText(1, model.getName());
                    widget->setText(2, "-1");
                    widget->setText(3, model.getLabel());
                    widget->setText(4, model.getDeviceName());
                    widget->setText(5, model.getType());
                }
            }
        }

        this->toolBoxLibrary->setItemText(0, QString("Audio (%1)").arg(this->treeWidgetAudio->topLevelItemCount()));
        this->toolBoxLibrary->setItemText(1, QString("Images (%1)").arg(this->treeWidgetImage->topLevelItemCount()));
        this->toolBoxLibrary->setItemText(3, QString("Videos (%1)").arg(this->treeWidgetVideo->topLevelItemCount()));
    }
    else if(event->type() == static_cast<QEvent::Type>(Enum::EventType::TemplateChanged))
    {
        TemplateChangedEvent* templateChangedEvent = dynamic_cast<TemplateChangedEvent*>(event);

        // TODO: Only add / remove necessary items.
        this->treeWidgetTemplate->clear();
        this->treeWidgetTemplate->clearSelection();

        QList<LibraryModel> models;
        if (this->lineEditFilter->text().isEmpty())
            models = DatabaseManager::getInstance().getLibraryTemplate();
        else
            models = DatabaseManager::getInstance().getLibraryTemplateByFilter(this->lineEditFilter->text());

        if (models.count() > 0)
        {
            foreach (LibraryModel model, models)
            {
                QTreeWidgetItem* widget = new QTreeWidgetItem(this->treeWidgetTemplate);
                widget->setBackgroundColor(0, QColor(Color::DEFAULT_TEMPLATE_COLOR));
                widget->setIcon(1, QIcon(":/Graphics/Images/Template.png"));
                widget->setText(1, model.getName());
                widget->setText(2, "-1");
                widget->setText(3, model.getLabel());
                widget->setText(4, model.getDeviceName());
                widget->setText(5, model.getType());
            }
        }

        this->toolBoxLibrary->setItemText(2, QString("Templates (%1)").arg(this->treeWidgetTemplate->topLevelItemCount()));
    }
    else if(event->type() == static_cast<QEvent::Type>(Enum::EventType::DataChanged))
    {
        DataChangedEvent* dataChangedEvent = dynamic_cast<DataChangedEvent*>(event);

        // TODO: Only add / remove necessary items.
        this->treeWidgetStoredData->clear();
        this->treeWidgetStoredData->clearSelection();

        QList<LibraryModel> models;
        if (this->lineEditFilter->text().isEmpty())
            models = DatabaseManager::getInstance().getLibraryStoredData();
        else
            models = DatabaseManager::getInstance().getLibraryStoredDataByFilter(this->lineEditFilter->text());

        if (models.count() > 0)
        {
            foreach (LibraryModel model, models)
            {
                QTreeWidgetItem* widget = new QTreeWidgetItem(this->treeWidgetStoredData);
                widget->setBackgroundColor(0, QColor(Color::DEFAULT_STORED_DATA_COLOR));
                widget->setIcon(1, QIcon(":/Graphics/Images/StoredData.png"));
                widget->setText(1, model.getName());
                widget->setText(2, "-1");
                widget->setText(3, model.getLabel());
                widget->setText(4, model.getDeviceName());
                widget->setText(5, model.getType());
            }
        }

        this->toolBoxLibrary->setItemText(4, QString("Stored (%1)").arg(this->treeWidgetStoredData->topLevelItemCount()));
    }

    return QObject::eventFilter(target, event);
}

void LibraryWidget::customContextMenuRequested(const QPoint& point)
{
    if (this->toolBoxLibrary->currentIndex() == Library::AUDIO_PAGE_INDEX)
    {
        if (this->treeWidgetAudio->selectedItems().count() == 0)
            return;

        this->contextMenu->exec(this->treeWidgetAudio->mapToGlobal(point));
    }
    else if (this->toolBoxLibrary->currentIndex() == Library::STILL_PAGE_INDEX)
    {
        if (this->treeWidgetImage->selectedItems().count() == 0)
            return;

        this->contextMenu->exec(this->treeWidgetImage->mapToGlobal(point));
    }
    else if (this->toolBoxLibrary->currentIndex() == Library::TEMPLATE_PAGE_INDEX)
    {
        if (this->treeWidgetTemplate->selectedItems().count() == 0)
            return;

        this->contextMenu->exec(this->treeWidgetTemplate->mapToGlobal(point));
    }
    else if (this->toolBoxLibrary->currentIndex() == Library::MOVIE_PAGE_INDEX)
    {
        if (this->treeWidgetVideo->selectedItems().count() == 0)
            return;

        this->contextMenu->exec(this->treeWidgetVideo->mapToGlobal(point));
    }
}

void LibraryWidget::customContextMenuImageRequested(const QPoint& point)
{
    if (this->treeWidgetImage->selectedItems().count() == 0)
        return;

    this->contextMenuImage->exec(this->treeWidgetImage->mapToGlobal(point));
}

void LibraryWidget::contextMenuTriggered(QAction* action)
{
    if (this->toolBoxLibrary->currentIndex() == Library::AUDIO_PAGE_INDEX)
    {
        foreach (QTreeWidgetItem* item, this->treeWidgetAudio->selectedItems())
            qApp->postEvent(qApp, new AddRudnownItemEvent(LibraryModel(item->text(2).toInt(), item->text(3), item->text(1), item->text(4), item->text(5))));
    }
    else if (this->toolBoxLibrary->currentIndex() == Library::TEMPLATE_PAGE_INDEX)
    {
        foreach (QTreeWidgetItem* item, this->treeWidgetTemplate->selectedItems())
            qApp->postEvent(qApp, new AddRudnownItemEvent(LibraryModel(item->text(2).toInt(), item->text(3), item->text(1), item->text(4), item->text(5))));
    }
    else if (this->toolBoxLibrary->currentIndex() == Library::MOVIE_PAGE_INDEX)
    {
        foreach (QTreeWidgetItem* item, this->treeWidgetVideo->selectedItems())
            qApp->postEvent(qApp, new AddRudnownItemEvent(LibraryModel(item->text(2).toInt(), item->text(3), item->text(1), item->text(4), item->text(5))));
    }
}

void LibraryWidget::contextMenuImageTriggered(QAction* action)
{
    if (action->text() == "Add to rundown as image")
    {
        foreach (QTreeWidgetItem* item, this->treeWidgetImage->selectedItems())
            qApp->postEvent(qApp, new AddRudnownItemEvent(LibraryModel(item->text(2).toInt(), item->text(3), item->text(1), item->text(4), item->text(5))));
    }
    else if (action->text() == "Add to rundown as image scroller")
    {
        foreach (QTreeWidgetItem* item, this->treeWidgetImage->selectedItems())
            qApp->postEvent(qApp, new AddRudnownItemEvent(LibraryModel(item->text(2).toInt(), item->text(3), item->text(1), item->text(5), "IMAGESCROLLER")));
    }
}

void LibraryWidget::filterLibrary()
{
    qApp->postEvent(qApp, new MediaChangedEvent());
    qApp->postEvent(qApp, new TemplateChangedEvent());
    qApp->postEvent(qApp, new DataChangedEvent());
}

void LibraryWidget::itemClicked(QTreeWidgetItem* current, int i)
{
    if (current == NULL)
        return;

    this->model = QSharedPointer<LibraryModel>(new LibraryModel(current->text(2).toInt(), current->text(3), current->text(1), current->text(2), current->text(5)));

    // Use synchronous event through sendEvent(). The inspector will update the selected
    // rundown item. We want to be absolutely sure that we update the right item, which
    // will not be the case with postEvent() if we trigger up/down arrow key really fast.
    qApp->sendEvent(qApp, new LibraryItemSelectedEvent(NULL, this->model.data()));
}

void LibraryWidget::currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{ 
    if (current == NULL)
        return;

    this->model = QSharedPointer<LibraryModel>(new LibraryModel(current->text(2).toInt(), current->text(3), current->text(1), current->text(4), current->text(5)));

    // Use synchronous event through sendEvent(). The inspector will update the selected
    // rundown item. We want to be absolutely sure that we update the right item, which
    // will not be the case with postEvent() if we trigger up/down arrow key really fast.
    qApp->sendEvent(qApp, new LibraryItemSelectedEvent(NULL, this->model.data()));
}
