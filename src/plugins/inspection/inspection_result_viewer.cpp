#include "inspection_result_viewer.h"
#include <QVBoxLayout>
InspectionResultViewer::InspectionResultViewer(QWidget*p):QWidget(p){auto*l=new QVBoxLayout(this);label_=new QLabel(tr("No inspection result"),this);label_->setWordWrap(true);l->addWidget(label_);}
void InspectionResultViewer::showResult(const QString&id,const QString&r,double c,const QString&path){label_->setText(QString("<b>%1</b><br/>%2<br/>Confidence: %3<br/>Image: %4").arg(id,r).arg(c,0,'f',2).arg(path));}
