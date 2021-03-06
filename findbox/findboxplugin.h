#pragma once
#include <QDesignerCustomWidgetInterface>

class FindBoxPlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QDesignerCustomWidgetInterface" FILE "findbox.json")
    Q_INTERFACES(QDesignerCustomWidgetInterface)
    bool initialized;

public:
    FindBoxPlugin (QObject * parent = 0);

    bool isContainer () const;
    bool isInitialized () const;
    QIcon icon () const;
    QString domXml () const;
    QString group () const;
    QString includeFile () const;
    QString name () const;
    QString toolTip () const;
    QString whatsThis () const;
    QWidget * createWidget (QWidget * parent);
    void initialize (QDesignerFormEditorInterface * core);
};
