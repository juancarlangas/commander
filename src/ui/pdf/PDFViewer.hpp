#pragma once

#include <QWidget>
#include <QPdfDocument>

class PDFViewer : public QWidget {
    Q_OBJECT

public:
    explicit PDFViewer(QWidget *parent = nullptr);
    ~PDFViewer();

    void loadFile(const QString &path);
    void showPage(int page);   // Mostrar página específica
    int pageCount() const;
    void nextPage();
    void previousPage();

private:
    QPdfDocument *document;
    QPdfView *view;
    int currentPage;
};

