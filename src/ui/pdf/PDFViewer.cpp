#include "PDFViewer.hpp"
#include <QVBoxLayout>

PDFViewer::PDFViewer(QWidget *parent)
    : QWidget(parent),
      document(new QPdfDocument(this)),
      view(new QPdfView(this)),
      currentPage(0)
{
    // No recibe foco
    setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_ShowWithoutActivating);

    view->setFocusPolicy(Qt::NoFocus);
    view->setZoomMode(QPdfView::FitInView);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(view);
    setLayout(layout);
}

PDFViewer::~PDFViewer() = default;

void PDFViewer::loadFile(const QString &path) {
    if (document->load(path) == QPdfDocument::NoError) {
        view->setDocument(document);
        currentPage = 0;
        view->setPage(currentPage);
        show();
    }
}

void PDFViewer::showPage(int page) {
    if (page >= 0 && page < document->pageCount()) {
        currentPage = page;
        view->setPage(currentPage);
    }
}

int PDFViewer::pageCount() const {
    return document->pageCount();
}

void PDFViewer::nextPage() {
    showPage(currentPage + 1);
}

void PDFViewer::previousPage() {
    showPage(currentPage - 1);
}

