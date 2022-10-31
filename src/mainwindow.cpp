#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QClipboard>
#include "openssl/provider.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
        OSSL_PROVIDER_load(NULL, "legacy");
        OSSL_PROVIDER_load(NULL, "default");

    ui->setupUi(this);
    this->setWindowTitle("HashTool");
    setWindowIcon(QIcon(":/img/hashtag.png"));
    init();
    addhash();
    signalattach();

    end();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    mainlayut = new QBoxLayout(tb);
    hashselector = new QBoxLayout(QBoxLayout::LeftToRight);
    type = new QBoxLayout(QBoxLayout::LeftToRight);
    hashalgo = new QLabel("Select Hash Algorithm ", this);
    hashcombobox = new QComboBox(this);
    text = new QRadioButton("Text", this);
    r1 = new QRadioButton("File", this);
    typelabel = new QLabel("Input Type", this);
    sublayout = new QBoxLayout(QBoxLayout::LeftToRight);
    Browse = new QPushButton("Browse", this);
    copytoclipboard = new QPushButton("", this);
    copytoclipboard->setIcon(QIcon(":/img/copuico2.png"));
    copytoclipboard->setToolTip("copy hash to clipboard");
    textbrowser = new QTextBrowser(this);
    name = new QLabel("Select File ", this);
    browselayout = new QBoxLayout(QBoxLayout::LeftToRight);

    buttonlayout = new QBoxLayout(QBoxLayout::LeftToRight);
    edit = new QLineEdit(this);
    calculate = new QPushButton("Calculate", this);
    diag = new QFileDialog(this, "select File For Hash");
    textbrowser->setTextInteractionFlags(Qt::TextInteractionFlags::enum_type::TextEditable);
    text->setChecked(true);
    edit->setPlaceholderText("Hash of Text");
}

void MainWindow::end()
{

    hashselector->addWidget(hashalgo);
    hashselector->addWidget(hashcombobox);

    type->addWidget(typelabel);
    type->addWidget(text);
    type->addWidget(r1);

    sublayout->addWidget(textbrowser);

    browselayout->addWidget(name);
    browselayout->addWidget(Browse);
    name->hide();
    Browse->hide();
    buttonlayout->addWidget(edit);
    buttonlayout->addWidget(copytoclipboard);
    buttonlayout->addWidget(calculate);
    edit->setReadOnly(1);

    mainlayut->addLayout(hashselector);
    mainlayut->addLayout(type);
    mainlayut->addLayout(sublayout);
    mainlayut->addLayout(buttonlayout);
    ui->centralwidget->setLayout(mainlayut);
}

void MainWindow::addhash()
{
    char hasharray[21][11] = {"blake2b512", "blake2s256","md2", "md4", "md5","mdc2", "rmd160","RIPEMD160", "sha1", "sha224", "sha256", "sha3-224", "sha3-256", "sha3-384", "sha3-512", "sha384", "sha512", "sha512-224", "sha512-256", "sm3","WHIRLPOOL"};

    QList<QString> m;

    for (int i = 0; i < 21; i++)
    {

        m << hasharray[i];
    }

    hashlist = new QStringList(m);
    hashcombobox->insertItems(0, *hashlist);
}

void MainWindow::threadfunction(MainWindow *x, QString s)
{

    std::string name3 = s.toStdString();
    BIO *b = BIO_new_file(name3.c_str(), "r");
    unsigned char digest[EVP_MAX_MD_SIZE];
    std::string sp;
    EVP_MD_CTX *ctx;
    ctx = EVP_MD_CTX_new();
    const EVP_MD *md;
    unsigned int len;
    md = EVP_get_digestbyname(x->hashcombobox->currentText().toLower().toStdString().c_str());
    if (md == NULL)
        qInfo() << "error\n";

    EVP_DigestInit_ex(ctx, md, NULL);

    if (b != NULL)
    {
        while (BIO_eof(b) != 1)
        {
            char m[20480];
            size_t y;
            BIO_read_ex(b, (void *)m, 20480, &y);
            EVP_DigestUpdate(ctx, m, y);
        }

        EVP_DigestFinal_ex(ctx, digest, &len);
        for (unsigned int i = 0; i < len; i++)
        {
            char m[3];
            BIO_snprintf(m, 3, "%02x", digest[i]);
            sp.append(m);
        }
    }
    BIO_free(b);
    emit x->m(sp);
}

void MainWindow::updatehash(std::string cx)
{

    edit->setText(cx.c_str());
}
void MainWindow::copytoclipboardclicked()
{

    QClipboard *c = QGuiApplication::clipboard();
    c->setText(edit->text());
}

void MainWindow::signalattach()
{
    connect(r1, SIGNAL(toggled(bool)), this, SLOT(checked(bool)));
    connect(text, SIGNAL(toggled(bool)), this, SLOT(checked(bool)));
    connect(calculate, SIGNAL(clicked()), this, SLOT(calculateclicked()));
    connect(copytoclipboard, SIGNAL(clicked()), this, SLOT(copytoclipboardclicked()));

    connect(Browse, SIGNAL(clicked()), this, SLOT(browseclicked()));
    connect(textbrowser, SIGNAL(textChanged()), this, SLOT(calculateclicked()));
    connect(hashcombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(combochanged(int)));

    connect(diag, SIGNAL(fileSelected(QString)), this, SLOT(itemselected(QString)));
    connect(this, SIGNAL(m(std::string)), this, SLOT(updatehash(std::string)));
}

void MainWindow::checked(bool x)
{
    if (r1->isChecked())
    {
        edit->setPlaceholderText("Hash of File");
        sublayout->removeWidget(textbrowser);
        textbrowser->hide();
        sublayout->addWidget(name);
        sublayout->addWidget(Browse);
        name->show();
        Browse->show();
        edit->setText("");
        if (diag->selectedFiles().length() != 0)
            itemselected(diag->selectedFiles().at(0));
    }
    if (text->isChecked())
    {
        sublayout->removeWidget(name);
        sublayout->removeWidget(Browse);
        name->hide();
        Browse->hide();
        textbrowser->show();
        edit->setPlaceholderText("Hash of Text");
        sublayout->addWidget(textbrowser);
        edit->setText("");
        if (textbrowser->toPlainText().length() != 0)
            calculateclicked();
    }
}

void MainWindow::calculateclicked()
{
    if (text->isChecked() && textbrowser->toPlainText().length() == 0)
    {
        edit->setText("");
        edit->setPlaceholderText("Hash of Text");
    }

    if (textbrowser->toPlainText().length() != 0 || diag->selectedFiles().length() != 0)
    {
        edit->setPlaceholderText("calculating...");
        if (text->isChecked())
        {

            EVP_MD_CTX *ctx;
            ctx = EVP_MD_CTX_new();
            

            unsigned int len;
            const EVP_MD *md = EVP_get_digestbyname(hashcombobox->currentText().toLower().toStdString().c_str());

            unsigned char digest[EVP_MAX_MD_SIZE];
            EVP_DigestInit_ex(ctx, md, NULL);

            std::string s9 = textbrowser->toPlainText().toStdString();

            EVP_DigestUpdate(ctx, s9.c_str(), s9.length());

            EVP_DigestFinal_ex(ctx, digest, &len);

            std::string s;
            for (unsigned int i = 0; i < len; i++)
            {
                char m[3];
                BIO_snprintf(m, 3, "%02x", digest[i]);
                s.append(m);
            }

            edit->setText(QString(s.c_str()));
        }

        if (r1->isChecked())
        {
            itemselected(diag->selectedFiles().at(0));
        }
    }
}

void MainWindow::combochanged(int)
{
    calculateclicked();
}

void MainWindow::browseclicked()
{
    diag->exec();
}

void MainWindow::itemselected(QString s)
{

    Filename = s.toStdString();
    QString x = "selected file : ";
    std::filesystem::path p(Filename);
    x.append(p.filename().c_str());
    name->setText(x);
    edit->setPlaceholderText("calculating ...");
    edit->setText("");
    std::thread t(threadfunction, this, s);
    t.detach();
}
